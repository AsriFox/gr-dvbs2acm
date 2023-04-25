/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2018,2019 Ahmet Inan, Ron Economos.
 *
 * This file is part of gr-dvbs2rx.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "cpu_features_macros.h"
#include "debug_level.h"
#include "ldpc_decoder_cb_impl.h"
#include <gnuradio/io_signature.h>
#include <pmt/pmt.h>
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
#include "cpuinfo_x86.h"
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

namespace gr {
namespace dvbs2acm {

using input_type = gr_complex;
using output_type = unsigned char;

ldpc_decoder_cb::sptr ldpc_decoder_cb::make(dvbs2_outputmode_t outputmode,
                                            dvbs2_infomode_t infomode,
                                            int max_trials,
                                            int debug_level)
{
    return gnuradio::make_block_sptr<ldpc_decoder_cb_impl>(
        outputmode, infomode, max_trials, debug_level);
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

    soft = new int8_t[FRAME_SIZE_NORMAL];
    dint = new int8_t[FRAME_SIZE_NORMAL];
    tempu = new int8_t[FRAME_SIZE_NORMAL];
    tempv = new int8_t[FRAME_SIZE_NORMAL];
    aligned_buffer = aligned_alloc(d_simd_size, d_simd_size * FRAME_SIZE_NORMAL);
    set_output_multiple(FRAME_SIZE_NORMAL);
    // if (outputmode == OM_MESSAGE) {
    //     set_output_multiple(nbch * d_simd_size);
    //     set_relative_rate((double)nbch / frame_size);
    // } else {
    //     set_output_multiple(frame_size * d_simd_size);
    // }
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
    ninput_items_required[0] = noutput_items;
    // if (output_mode == OM_MESSAGE) {
    //     ninput_items_required[0] = (noutput_items / nbch) * (frame_size / mod->bits());
    // } else {
    //     ninput_items_required[0] = noutput_items / mod->bits();
    // }
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
    float sp, np, sigma, precision_sum;
    gr_complex s, e;
    const int trials = (d_max_trials == 0) ? DEFAULT_TRIALS : d_max_trials;
    int consumed = 0;
    int rows, offset, indexin, indexout;
    const int* mux;
    int8_t *c1, *c2, *c3;
    // int output_size = output_mode ? nbch : frame_size;

    std::vector<tag_t> tags;
    const uint64_t nread = this->nitems_read(0); // number of items read on port 0

    // Read all tags on the input buffer
    this->get_tags_in_range(tags, 0, nread, nread + noutput_items, pmt::string_to_symbol("modcod"));

    // TODO: SIMD stuff
    for (tag_t tag : tags) {
        const uint64_t tagmodcod = pmt::to_uint64(tag.value);
        auto framesize = (dvbs2_framesize_t)((tagmodcod >> 1) & 0x7f);
        auto rate = (dvbs2_code_rate_t)((tagmodcod >> 8) & 0xff);
        auto constellation = (dvbs2_constellation_t)((tagmodcod >> 16) & 0xff);
        const uint64_t tagoffset = this->nitems_written(0);
        this->add_item_tag(0, tagoffset, pmt::string_to_symbol("modcod"), pmt::from_uint64(tagmodcod));

        ldpc = build_decoder(framesize, rate);

        // TODO: Make a class
        switch (constellation) {
        case MOD_QPSK:
            mod = new PhaseShiftKeying<4, gr_complex, int8_t>();
            break;
        case MOD_8PSK:
            mod = new PhaseShiftKeying<8, gr_complex, int8_t>();
            rows = ldpc->code_len() / mod->bits();
            /* 210 */
            if (rate == C3_5) {
                rowaddr0 = rows * 2;
                rowaddr1 = rows;
                rowaddr2 = 0;
            }
            /* 102 */
            else if (rate == C25_36 || rate == C13_18 || rate == C7_15 || rate == C8_15 ||
                     rate == C26_45) {
                rowaddr0 = rows;
                rowaddr1 = 0;
                rowaddr2 = rows * 2;
            }
            /* 012 */
            else {
                rowaddr0 = 0;
                rowaddr1 = rows;
                rowaddr2 = rows * 2;
            }
            break;
        // These modulations are from DVB-T2:
        // case MOD_16QAM:
        //     mod = new QuadratureAmplitudeModulation<16, gr_complex, int8_t>();
        //     break;
        // case MOD_64QAM:
        //     mod = new QuadratureAmplitudeModulation<64, gr_complex, int8_t>();
        //     break;
        // case MOD_256QAM:
        //     mod = new QuadratureAmplitudeModulation<256, gr_complex, int8_t>();
        //     break;
        default:
            break;
        }

        const int CODE_LEN = ldpc->code_len();
        const int MOD_BITS = mod->bits();
        int8_t tmp[MOD_BITS];
        int8_t* code = nullptr;
        const int SYMBOLS = CODE_LEN / MOD_BITS;

        // Determine target precision for demodulator based on noise level
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

        // Demodulate
        for (int j = 0; j < SYMBOLS; j++) {
            mod->soft(soft + (j * MOD_BITS /*+ (blk * CODE_LEN)*/), in[j], precision);
        }
        int frame_size;
        switch (framesize) {
        case FECFRAME_NORMAL:
            frame_size = FRAME_SIZE_NORMAL;
            break;
        case FECFRAME_SHORT:
            frame_size = FRAME_SIZE_SHORT;
            break;
        case FECFRAME_MEDIUM:
            frame_size = FRAME_SIZE_MEDIUM;
            break;
        }
        rows = frame_size / MOD_BITS;
        switch (constellation) {
        case MOD_8PSK:
            c1 = &dint[rowaddr0];
            c2 = &dint[rowaddr1];
            c3 = &dint[rowaddr2];
            indexin = 0;
            for (int j = 0; j < rows; j++) {
                c1[j] = soft[indexin++];
                c2[j] = soft[indexin++];
                c3[j] = soft[indexin++];
            }
            code = dint;
            break;
        case MOD_QPSK:
        default:
            code = soft;
            break;
        }
        in += rows;
        consumed += rows;

        // Decode LDPC
        int count = decode(aligned_buffer, code, trials);
        if (count < 0) {
            total_trials += trials;
            GR_LOG_DEBUG_LEVEL(1,
                               "frame = {:d}, snr = {:.2f}, trials = {:d} (max)",
                               (chunk * d_simd_size),
                               snr,
                               trials);
        } else {
            total_trials += (trials - count);
            GR_LOG_DEBUG_LEVEL(1,
                               "frame = {:d}, snr = {:.2f}, trials = {:d}",
                               (chunk * d_simd_size),
                               snr,
                               (trials - count));
        }

        // Deinterleave?
        chunk++;
        precision_sum = 0;
        switch (constellation) {
        case MOD_QPSK:
            for (int j = 0; j < CODE_LEN; j++) {
                tempv[j] = code[j] < 0 ? -1 : 1;
            }
            break;
        case MOD_8PSK:
            for (int j = 0; j < CODE_LEN; j++) {
                tempu[j] = code[j] < 0 ? -1 : 1;
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

        // Produce information bits
        precision = precision_sum / d_simd_size;
        for (int j = 0; j < CODE_LEN; j++) {
            if (code[j] >= 0) {
                *out++ = 0;
            } else {
                *out++ = 1;
            }
        }
        produce(0, CODE_LEN);
    }
    consume_each(consumed);
    return WORK_CALLED_PRODUCE;
}

} /* namespace dvbs2acm */
} /* namespace gr */
