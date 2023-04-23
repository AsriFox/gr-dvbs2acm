/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2018,2019 Ahmet Inan, Ron Economos.
 *
 * This file is part of gr-dvbs2rx.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "cpu_features/cpu_features_macros.h"
#include "debug_level.h"
#include "ldpc_decoder_cb_impl.h"
#include <gnuradio/io_signature.h>
#include <cstddef>
#include <string>

#ifdef CPU_FEATURES_ARCH_ARM
#include "cpu_features/cpuinfo_arm.h"
using namespace cpu_features;
static const ArmFeatures features = GetArmInfo().features;
static const bool has_neon = features.neon;
#endif

#ifdef CPU_FEATURES_ARCH_AARCH64
static const bool has_neon = true; // always available on aarch64
#endif

#ifdef CPU_FEATURES_ARCH_X86
#include "cpu_features/cpuinfo_x86.h"
using namespace cpu_features;
static const X86Features features = GetX86Info().features;
#endif

namespace ldpc_neon {
void ldpc_dec_init(LDPCInterface* it);
int ldpc_dec_decode(void* buffer, int8_t* code, int trials);
} // namespace ldpc_neon

namespace ldpc_avx2 {
void ldpc_dec_init(LDPCInterface* it);
int ldpc_dec_decode(void* buffer, int8_t* code, int trials);
} // namespace ldpc_avx2

namespace ldpc_sse41 {
void ldpc_dec_init(LDPCInterface* it);
int ldpc_dec_decode(void* buffer, int8_t* code, int trials);
} // namespace ldpc_sse41

namespace ldpc_generic {
void ldpc_dec_init(LDPCInterface* it);
int ldpc_dec_decode(void* buffer, int8_t* code, int trials);
} // namespace ldpc_generic

using namespace gr::dvbs2;

namespace gr {
namespace dvbs2acm {

using input_type = gr_complex;
using output_type = unsigned char;

ldpc_decoder_cb::sptr
ldpc_decoder_cb::make(dvbs2_outputmode_t outputmode, dvbs2_infomode_t infomode, int max_trials, int debug_level)
{
    return gnuradio::make_block_sptr<ldpc_decoder_cb_impl>(outputmode, infomode, max_trials, debug_level);
}


/*
 * The private constructor
 */
ldpc_decoder_cb_impl::ldpc_decoder_cb_impl(dvbs2_outputmode_t outputmode,
                                           dvbs2_infomode_t infomode,
                                           int max_trials,
                                           int debug_level)
    : gr::block("ldpc_decoder_cb",
                gr::io_signature::make(1, 1, sizeof(input_type)),
                gr::io_signature::make(1, 1, sizeof(output_type))),
      d_debug_level(debug_level),
      output_mode(outputmode),
      info_mode(infomode),
      frame(0),
      chunk(0),
      total_trials(0),
      d_max_trials(max_trials),
      total_snr(0.0),
      decode(nullptr)
{
    std::string impl = "generic";
#ifdef CPU_FEATURES_ARCH_ANY_ARM
    d_simd_size = 16;
    if (has_neon) {
        ldpc_neon::ldpc_dec_init(ldpc);
        decode = &ldpc_neon::ldpc_dec_decode;
        impl = "neon";
    } else {
        ldpc_generic::ldpc_dec_init(ldpc);
        decode = &ldpc_generic::ldpc_dec_decode;
    }
#else
#ifdef CPU_FEATURES_ARCH_X86
    d_simd_size = features.avx2 ? 32 : 16;
    if (features.avx2) {
        ldpc_avx2::ldpc_dec_init(ldpc);
        decode = &ldpc_avx2::ldpc_dec_decode;
        impl = "avx2";
    } else if (features.sse4_1) {
        ldpc_sse41::ldpc_dec_init(ldpc);
        decode = &ldpc_sse41::ldpc_dec_decode;
        impl = "sse4_1";
    } else {
        ldpc_generic::ldpc_dec_init(ldpc);
        decode = &ldpc_generic::ldpc_dec_decode;
    }
#else
    // Not ARM, nor x86. Use generic implementation.
    d_simd_size = 16;
    ldpc_generic::ldpc_dec_init(ldpc);
    decode = &ldpc_generic::ldpc_dec_decode;
#endif
#endif
    assert(decode != nullptr);
    d_debug_logger->debug("LDPC decoder implementation: {:s}", impl);

    soft = new int8_t[ldpc->code_len() * d_simd_size];
    dint = new int8_t[ldpc->code_len() * d_simd_size];
    tempu = new int8_t[ldpc->code_len()];
    tempv = new int8_t[ldpc->code_len()];
    aligned_buffer = aligned_alloc(d_simd_size, d_simd_size * ldpc->code_len());
    generate_interleave_lookup();
    generate_deinterleave_lookup();
    if (outputmode == OM_MESSAGE) {
        set_output_multiple(nbch * d_simd_size);
        set_relative_rate((double)nbch / frame_size);
    } else {
        set_output_multiple(frame_size * d_simd_size);
    }
}

/*
 * Our virtual destructor.
 */
ldpc_decoder_cb_impl::~ldpc_decoder_cb_impl()
{
    free(aligned_buffer);
    delete[] tempv;
    delete[] tempu;
    delete[] dint;
    delete[] soft;
    delete mod;
    delete ldpc;
}

void ldpc_decoder_cb_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    if (output_mode == OM_MESSAGE) {
        ninput_items_required[0] = (noutput_items / nbch) * (frame_size / mod->bits());
    } else {
        ninput_items_required[0] = noutput_items / mod->bits();
    }
}

const int DEFAULT_TRIALS = 25;
#define FACTOR 2 // same factor used on the decoder implementation

int ldpc_decoder_cb_impl::general_work(int noutput_items,
                                       gr_vector_int& ninput_items,
                                       gr_vector_const_void_star& input_items,
                                       gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    const gr_complex* insnr = (const gr_complex*)input_items[0];
    unsigned char* out = (unsigned char*)output_items[0];
    const int CODE_LEN = ldpc->code_len();
    const int MOD_BITS = mod->bits();
    int8_t tmp[MOD_BITS];
    int8_t* code = nullptr;
    float sp, np, sigma, precision_sum;
    gr_complex s, e;
    const int SYMBOLS = CODE_LEN / MOD_BITS;
    const int trials = (d_max_trials == 0) ? DEFAULT_TRIALS : d_max_trials;
    int consumed = 0;
    int rows, offset, indexin, indexout;
    const int* mux;
    int8_t *c1, *c2, *c3;
    int output_size = output_mode ? nbch : frame_size;

    for (int i = 0; i < noutput_items; i += output_size * d_simd_size) {
        for (int blk = 0; blk < d_simd_size; blk++) {
            if (frame == 0) {
                sp = 0;
                np = 0;
                for (int j = 0; j < SYMBOLS; j++) {
                    mod->hard(tmp, in[j]);
                    s = mod->map(tmp);
                    e = in[j] - s;
                    sp += std::norm(s);
                    np += std::norm(e);
                }
                if (!(np > 0)) {
                    np = 1e-12;
                }
                snr = 10 * std::log10(sp / np);
                sigma = std::sqrt(np / (2 * sp));
                precision = FACTOR / (sigma * sigma);
            }
            for (int j = 0; j < SYMBOLS; j++) {
                mod->soft(soft + (j * MOD_BITS + (blk * CODE_LEN)), in[j], precision);
            }
            switch (signal_constellation) {
            case MOD_QPSK:
                if (dvb_standard == STANDARD_DVBT2) {
                    if (code_rate == C1_3 || code_rate == C2_5) {
                        for (unsigned int t = 0; t < q_val; t++) {
                            for (int s = 0; s < 360; s++) {
                                dint[(nbch + (q_val * s) + t) + (blk * CODE_LEN)] =
                                    soft[(nbch + (360 * t) + s) + (blk * CODE_LEN)];
                            }
                        }
                        for (unsigned int k = 0; k < nbch; k++) {
                            dint[k + (blk * CODE_LEN)] = soft[k + (blk * CODE_LEN)];
                        }
                        code = dint;
                    } else {
                        code = soft;
                    }
                } else {
                    code = soft;
                }
                break;
            case MOD_8PSK:
                rows = frame_size / MOD_BITS;
                c1 = &dint[rowaddr0 + (blk * CODE_LEN)];
                c2 = &dint[rowaddr1 + (blk * CODE_LEN)];
                c3 = &dint[rowaddr2 + (blk * CODE_LEN)];
                indexin = 0;
                for (int j = 0; j < rows; j++) {
                    c1[j] = soft[indexin++ + (blk * CODE_LEN)];
                    c2[j] = soft[indexin++ + (blk * CODE_LEN)];
                    c3[j] = soft[indexin++ + (blk * CODE_LEN)];
                }
                code = dint;
                break;
            case MOD_16QAM:
                if (code_rate == C3_5 && frame_size == FRAME_SIZE_NORMAL) {
                    mux = &mux16_35[0];
                } else if (code_rate == C1_3 && frame_size == FRAME_SIZE_SHORT) {
                    mux = &mux16_13[0];
                } else if (code_rate == C2_5 && frame_size == FRAME_SIZE_SHORT) {
                    mux = &mux16_25[0];
                } else {
                    mux = &mux16[0];
                }
                indexin = 0;
                indexout = 0;
                for (unsigned int d = 0; d < frame_size / (MOD_BITS * 2); d++) {
                    for (int e = 0; e < (MOD_BITS * 2); e++) {
                        offset = mux[e];
                        tempu[indexout++] = soft[(offset + indexin) + (blk * CODE_LEN)];
                    }
                    indexin += MOD_BITS * 2;
                }
                indexin = 0;
                indexout = 0;
                for (unsigned int j = 0; j < frame_size; j++) {
                    tempv[indexout++] = tempu[deinterleave_lookup_table[indexin++]];
                }
                for (unsigned int t = 0; t < q_val; t++) {
                    for (int s = 0; s < 360; s++) {
                        dint[(nbch + (q_val * s) + t) + (blk * CODE_LEN)] = tempv[(nbch + (360 * t) + s)];
                    }
                }
                for (unsigned int k = 0; k < nbch; k++) {
                    dint[k + (blk * CODE_LEN)] = tempv[k];
                }
                code = dint;
                break;
            case MOD_64QAM:
                if (code_rate == C3_5 && frame_size == FRAME_SIZE_NORMAL) {
                    mux = &mux64_35[0];
                } else if (code_rate == C1_3 && frame_size == FRAME_SIZE_SHORT) {
                    mux = &mux64_13[0];
                } else if (code_rate == C2_5 && frame_size == FRAME_SIZE_SHORT) {
                    mux = &mux64_25[0];
                } else {
                    mux = &mux64[0];
                }
                indexin = 0;
                indexout = 0;
                for (unsigned int d = 0; d < frame_size / (MOD_BITS * 2); d++) {
                    for (int e = 0; e < (MOD_BITS * 2); e++) {
                        offset = mux[e];
                        tempu[indexout++] = soft[(offset + indexin) + (blk * CODE_LEN)];
                    }
                    indexin += MOD_BITS * 2;
                }
                indexin = 0;
                indexout = 0;
                for (unsigned int j = 0; j < frame_size; j++) {
                    tempv[indexout++] = tempu[deinterleave_lookup_table[indexin++]];
                }
                for (unsigned int t = 0; t < q_val; t++) {
                    for (int s = 0; s < 360; s++) {
                        dint[(nbch + (q_val * s) + t) + (blk * CODE_LEN)] = tempv[(nbch + (360 * t) + s)];
                    }
                }
                for (unsigned int k = 0; k < nbch; k++) {
                    dint[k + (blk * CODE_LEN)] = tempv[k];
                }
                code = dint;
                break;
            case MOD_256QAM:
                if (frame_size == FRAME_SIZE_NORMAL) {
                    if (code_rate == C3_5) {
                        mux = &mux256_35[0];
                    } else if (code_rate == C2_3) {
                        mux = &mux256_23[0];
                    } else {
                        mux = &mux256[0];
                    }
                    indexin = 0;
                    indexout = 0;
                    for (unsigned int d = 0; d < frame_size / (MOD_BITS * 2); d++) {
                        for (int e = 0; e < (MOD_BITS * 2); e++) {
                            offset = mux[e];
                            tempu[indexout++] = soft[(offset + indexin) + (blk * CODE_LEN)];
                        }
                        indexin += MOD_BITS * 2;
                    }
                    indexin = 0;
                    indexout = 0;
                    for (unsigned int j = 0; j < frame_size; j++) {
                        tempv[indexout++] = tempu[deinterleave_lookup_table[indexin++]];
                    }
                    for (unsigned int t = 0; t < q_val; t++) {
                        for (int s = 0; s < 360; s++) {
                            dint[(nbch + (q_val * s) + t) + (blk * CODE_LEN)] = tempv[(nbch + (360 * t) + s)];
                        }
                    }
                    for (unsigned int k = 0; k < nbch; k++) {
                        dint[k + (blk * CODE_LEN)] = tempv[k];
                    }
                    code = dint;
                } else {
                    if (code_rate == C1_3) {
                        mux = &mux256s_13[0];
                    } else if (code_rate == C2_5) {
                        mux = &mux256s_25[0];
                    } else {
                        mux = &mux256s[0];
                    }
                    indexin = 0;
                    indexout = 0;
                    for (unsigned int d = 0; d < frame_size / MOD_BITS; d++) {
                        for (int e = 0; e < MOD_BITS; e++) {
                            offset = mux[e];
                            tempu[indexout++] = soft[(offset + indexin) + (blk * CODE_LEN)];
                        }
                        indexin += MOD_BITS;
                    }
                    indexin = 0;
                    indexout = 0;
                    for (unsigned int j = 0; j < frame_size; j++) {
                        tempv[indexout++] = tempu[deinterleave_lookup_table[indexin++]];
                    }
                    for (unsigned int t = 0; t < q_val; t++) {
                        for (int s = 0; s < 360; s++) {
                            dint[(nbch + (q_val * s) + t) + (blk * CODE_LEN)] = tempv[(nbch + (360 * t) + s)];
                        }
                    }
                    for (unsigned int k = 0; k < nbch; k++) {
                        dint[k + (blk * CODE_LEN)] = tempv[k];
                    }
                    code = dint;
                }
                break;
            default:
                code = soft;
                break;
            }
            in += frame_size / MOD_BITS;
            consumed += frame_size / MOD_BITS;
        }
        int count = decode(aligned_buffer, code, trials);
        if (count < 0) {
            total_trials += trials;
            GR_LOG_DEBUG_LEVEL(1, "frame = {:d}, snr = {:.2f}, trials = {:d} (max)", (chunk * d_simd_size), snr, trials);
        } else {
            total_trials += (trials - count);
            GR_LOG_DEBUG_LEVEL(1, "frame = {:d}, snr = {:.2f}, trials = {:d}", (chunk * d_simd_size), snr, (trials - count));
        }
        chunk++;
        precision_sum = 0;
        for (int blk = 0; blk < d_simd_size; blk++) {
            switch (signal_constellation) {
            case MOD_QPSK:
                for (int j = 0; j < CODE_LEN; j++) {
                    tempv[j] = code[j + (blk * CODE_LEN)] < 0 ? -1 : 1;
                }
                break;
            case MOD_8PSK:
                for (int j = 0; j < CODE_LEN; j++) {
                    tempu[j] = code[j + (blk * CODE_LEN)] < 0 ? -1 : 1;
                }
                rows = frame_size / MOD_BITS;
                c1 = &tempu[rowaddr0];
                c2 = &tempu[rowaddr1];
                c3 = &tempu[rowaddr2];
                indexout = 0;
                for (int j = 0; j < rows; j++) {
                    tempv[indexout++] = c1[j];
                    tempv[indexout++] = c2[j];
                    tempv[indexout++] = c3[j];
                }
                break;
            case MOD_16QAM:
                if (code_rate == C3_5 && frame_size == FRAME_SIZE_NORMAL) {
                    mux = &mux16_35[0];
                } else if (code_rate == C1_3 && frame_size == FRAME_SIZE_SHORT) {
                    mux = &mux16_13[0];
                } else if (code_rate == C2_5 && frame_size == FRAME_SIZE_SHORT) {
                    mux = &mux16_25[0];
                } else {
                    mux = &mux16[0];
                }
                for (int j = 0; j < CODE_LEN; j++) {
                    tempu[j] = code[j + (blk * CODE_LEN)] < 0 ? -1 : 1;
                }
                indexin = 0;
                indexout = 0;
                for (unsigned int d = 0; d < frame_size / (MOD_BITS * 2); d++) {
                    for (int e = 0; e < (MOD_BITS * 2); e++) {
                        offset = mux[e];
                        tempv[offset + indexout] = tempu[interleave_lookup_table[indexin++]];
                    }
                    indexout += MOD_BITS * 2;
                }
                break;
            case MOD_64QAM:
                if (code_rate == C3_5 && frame_size == FRAME_SIZE_NORMAL) {
                    mux = &mux64_35[0];
                } else if (code_rate == C1_3 && frame_size == FRAME_SIZE_SHORT) {
                    mux = &mux64_13[0];
                } else if (code_rate == C2_5 && frame_size == FRAME_SIZE_SHORT) {
                    mux = &mux64_25[0];
                } else {
                    mux = &mux64[0];
                }
                for (int j = 0; j < CODE_LEN; j++) {
                    tempu[j] = code[j + (blk * CODE_LEN)] < 0 ? -1 : 1;
                }
                indexin = 0;
                indexout = 0;
                for (unsigned int d = 0; d < frame_size / (MOD_BITS * 2); d++) {
                    for (int e = 0; e < (MOD_BITS * 2); e++) {
                        offset = mux[e];
                        tempv[offset + indexout] = tempu[interleave_lookup_table[indexin++]];
                    }
                    indexout += MOD_BITS * 2;
                }
                break;
            case MOD_256QAM:
                if (frame_size == FRAME_SIZE_NORMAL) {
                    if (code_rate == C3_5) {
                        mux = &mux256_35[0];
                    } else if (code_rate == C2_3) {
                        mux = &mux256_23[0];
                    } else {
                        mux = &mux256[0];
                    }
                    for (int j = 0; j < CODE_LEN; j++) {
                        tempu[j] = code[j + (blk * CODE_LEN)] < 0 ? -1 : 1;
                    }
                    indexin = 0;
                    indexout = 0;
                    for (unsigned int d = 0; d < frame_size / (MOD_BITS * 2); d++) {
                        for (int e = 0; e < (MOD_BITS * 2); e++) {
                            offset = mux[e];
                            tempv[offset + indexout] = tempu[interleave_lookup_table[indexin++]];
                        }
                        indexout += MOD_BITS * 2;
                    }
                } else {
                    if (code_rate == C1_3) {
                        mux = &mux256s_13[0];
                    } else if (code_rate == C2_5) {
                        mux = &mux256s_25[0];
                    } else {
                        mux = &mux256s[0];
                    }
                    for (int j = 0; j < CODE_LEN; j++) {
                        tempu[j] = code[j + (blk * CODE_LEN)] < 0 ? -1 : 1;
                    }
                    indexin = 0;
                    indexout = 0;
                    for (unsigned int d = 0; d < frame_size / MOD_BITS; d++) {
                        for (int e = 0; e < MOD_BITS; e++) {
                            offset = mux[e];
                            tempv[offset + indexout] = tempu[interleave_lookup_table[indexin++]];
                        }
                        indexout += MOD_BITS;
                    }
                }
                break;
            default:
                break;
            }
            sp = 0;
            np = 0;
            for (int j = 0; j < SYMBOLS; j++) {
                s = mod->map(&tempv[(j * MOD_BITS)]);
                e = insnr[j] - s;
                sp += std::norm(s);
                np += std::norm(e);
            }
            if (!(np > 0)) {
                np = 1e-12;
            }
            snr = 10 * std::log10(sp / np);
            sigma = std::sqrt(np / (2 * sp));
            precision_sum += FACTOR / (sigma * sigma);
            total_snr += snr;
            if (info_mode) {
                GR_LOG_DEBUG_LEVEL(1,
                                   "frame = {:d}, snr = {:.2f}, average trials = {:d}, "
                                   "average snr =,.2f",
                                   frame,
                                   snr,
                                   (total_trials / chunk),
                                   (total_snr / (frame + 1)));
            }
            insnr += frame_size / MOD_BITS;
            frame++;
        }
        precision = precision_sum / d_simd_size;
        for (int blk = 0; blk < d_simd_size; blk++) {
            for (int j = 0; j < output_size; j++) {
                if (code[j + (blk * CODE_LEN)] >= 0) {
                    *out++ = 0;
                } else {
                    *out++ = 1;
                }
            }
        }
    }

    // Tell runtime system how many input items we consumed on
    // each input stream.
    consume_each(consumed);

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace dvbs2acm */
} /* namespace gr */
