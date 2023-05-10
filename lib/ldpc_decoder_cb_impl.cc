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
#include "ldpc_decoder/dvb_s2_tables.hh"
#include "ldpc_decoder/dvb_s2x_tables.hh"
#include "ldpc_decoder_cb_impl.h"
#include "modcod.hh"
#include <gnuradio/io_signature.h>
#include <pmt/pmt.h>

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

constexpr int DVB_S2_TABLE_B1::DEG[];
constexpr int DVB_S2_TABLE_B1::LEN[];
constexpr int DVB_S2_TABLE_B1::POS[];

constexpr int DVB_S2_TABLE_B2::DEG[];
constexpr int DVB_S2_TABLE_B2::LEN[];
constexpr int DVB_S2_TABLE_B2::POS[];

constexpr int DVB_S2_TABLE_B3::DEG[];
constexpr int DVB_S2_TABLE_B3::LEN[];
constexpr int DVB_S2_TABLE_B3::POS[];

constexpr int DVB_S2_TABLE_B4::DEG[];
constexpr int DVB_S2_TABLE_B4::LEN[];
constexpr int DVB_S2_TABLE_B4::POS[];

constexpr int DVB_S2_TABLE_B5::DEG[];
constexpr int DVB_S2_TABLE_B5::LEN[];
constexpr int DVB_S2_TABLE_B5::POS[];

constexpr int DVB_S2_TABLE_B6::DEG[];
constexpr int DVB_S2_TABLE_B6::LEN[];
constexpr int DVB_S2_TABLE_B6::POS[];

constexpr int DVB_S2_TABLE_B7::DEG[];
constexpr int DVB_S2_TABLE_B7::LEN[];
constexpr int DVB_S2_TABLE_B7::POS[];

constexpr int DVB_S2_TABLE_B8::DEG[];
constexpr int DVB_S2_TABLE_B8::LEN[];
constexpr int DVB_S2_TABLE_B8::POS[];

constexpr int DVB_S2_TABLE_B9::DEG[];
constexpr int DVB_S2_TABLE_B9::LEN[];
constexpr int DVB_S2_TABLE_B9::POS[];

constexpr int DVB_S2_TABLE_B10::DEG[];
constexpr int DVB_S2_TABLE_B10::LEN[];
constexpr int DVB_S2_TABLE_B10::POS[];

constexpr int DVB_S2_TABLE_B11::DEG[];
constexpr int DVB_S2_TABLE_B11::LEN[];
constexpr int DVB_S2_TABLE_B11::POS[];

constexpr int DVB_S2X_TABLE_B1::DEG[];
constexpr int DVB_S2X_TABLE_B1::LEN[];
constexpr int DVB_S2X_TABLE_B1::POS[];

constexpr int DVB_S2X_TABLE_B2::DEG[];
constexpr int DVB_S2X_TABLE_B2::LEN[];
constexpr int DVB_S2X_TABLE_B2::POS[];

constexpr int DVB_S2X_TABLE_B3::DEG[];
constexpr int DVB_S2X_TABLE_B3::LEN[];
constexpr int DVB_S2X_TABLE_B3::POS[];

constexpr int DVB_S2X_TABLE_B4::DEG[];
constexpr int DVB_S2X_TABLE_B4::LEN[];
constexpr int DVB_S2X_TABLE_B4::POS[];

constexpr int DVB_S2X_TABLE_B5::DEG[];
constexpr int DVB_S2X_TABLE_B5::LEN[];
constexpr int DVB_S2X_TABLE_B5::POS[];

constexpr int DVB_S2X_TABLE_B6::DEG[];
constexpr int DVB_S2X_TABLE_B6::LEN[];
constexpr int DVB_S2X_TABLE_B6::POS[];

constexpr int DVB_S2X_TABLE_B7::DEG[];
constexpr int DVB_S2X_TABLE_B7::LEN[];
constexpr int DVB_S2X_TABLE_B7::POS[];

constexpr int DVB_S2X_TABLE_B8::DEG[];
constexpr int DVB_S2X_TABLE_B8::LEN[];
constexpr int DVB_S2X_TABLE_B8::POS[];

constexpr int DVB_S2X_TABLE_B9::DEG[];
constexpr int DVB_S2X_TABLE_B9::LEN[];
constexpr int DVB_S2X_TABLE_B9::POS[];

constexpr int DVB_S2X_TABLE_B10::DEG[];
constexpr int DVB_S2X_TABLE_B10::LEN[];
constexpr int DVB_S2X_TABLE_B10::POS[];

constexpr int DVB_S2X_TABLE_B11::DEG[];
constexpr int DVB_S2X_TABLE_B11::LEN[];
constexpr int DVB_S2X_TABLE_B11::POS[];

constexpr int DVB_S2X_TABLE_B12::DEG[];
constexpr int DVB_S2X_TABLE_B12::LEN[];
constexpr int DVB_S2X_TABLE_B12::POS[];

constexpr int DVB_S2X_TABLE_B13::DEG[];
constexpr int DVB_S2X_TABLE_B13::LEN[];
constexpr int DVB_S2X_TABLE_B13::POS[];

constexpr int DVB_S2X_TABLE_B14::DEG[];
constexpr int DVB_S2X_TABLE_B14::LEN[];
constexpr int DVB_S2X_TABLE_B14::POS[];

constexpr int DVB_S2X_TABLE_B15::DEG[];
constexpr int DVB_S2X_TABLE_B15::LEN[];
constexpr int DVB_S2X_TABLE_B15::POS[];

constexpr int DVB_S2X_TABLE_B16::DEG[];
constexpr int DVB_S2X_TABLE_B16::LEN[];
constexpr int DVB_S2X_TABLE_B16::POS[];

constexpr int DVB_S2X_TABLE_B17::DEG[];
constexpr int DVB_S2X_TABLE_B17::LEN[];
constexpr int DVB_S2X_TABLE_B17::POS[];

constexpr int DVB_S2X_TABLE_B18::DEG[];
constexpr int DVB_S2X_TABLE_B18::LEN[];
constexpr int DVB_S2X_TABLE_B18::POS[];

constexpr int DVB_S2X_TABLE_B19::DEG[];
constexpr int DVB_S2X_TABLE_B19::LEN[];
constexpr int DVB_S2X_TABLE_B19::POS[];

constexpr int DVB_S2X_TABLE_B20::DEG[];
constexpr int DVB_S2X_TABLE_B20::LEN[];
constexpr int DVB_S2X_TABLE_B20::POS[];

constexpr int DVB_S2X_TABLE_B21::DEG[];
constexpr int DVB_S2X_TABLE_B21::LEN[];
constexpr int DVB_S2X_TABLE_B21::POS[];

constexpr int DVB_S2X_TABLE_B22::DEG[];
constexpr int DVB_S2X_TABLE_B22::LEN[];
constexpr int DVB_S2X_TABLE_B22::POS[];

constexpr int DVB_S2X_TABLE_B23::DEG[];
constexpr int DVB_S2X_TABLE_B23::LEN[];
constexpr int DVB_S2X_TABLE_B23::POS[];

constexpr int DVB_S2X_TABLE_B24::DEG[];
constexpr int DVB_S2X_TABLE_B24::LEN[];
constexpr int DVB_S2X_TABLE_B24::POS[];

constexpr int DVB_S2_TABLE_C1::DEG[];
constexpr int DVB_S2_TABLE_C1::LEN[];
constexpr int DVB_S2_TABLE_C1::POS[];

constexpr int DVB_S2_TABLE_C2::DEG[];
constexpr int DVB_S2_TABLE_C2::LEN[];
constexpr int DVB_S2_TABLE_C2::POS[];

constexpr int DVB_S2_TABLE_C3::DEG[];
constexpr int DVB_S2_TABLE_C3::LEN[];
constexpr int DVB_S2_TABLE_C3::POS[];

constexpr int DVB_S2_TABLE_C4::DEG[];
constexpr int DVB_S2_TABLE_C4::LEN[];
constexpr int DVB_S2_TABLE_C4::POS[];

constexpr int DVB_S2_TABLE_C5::DEG[];
constexpr int DVB_S2_TABLE_C5::LEN[];
constexpr int DVB_S2_TABLE_C5::POS[];

constexpr int DVB_S2_TABLE_C6::DEG[];
constexpr int DVB_S2_TABLE_C6::LEN[];
constexpr int DVB_S2_TABLE_C6::POS[];

constexpr int DVB_S2_TABLE_C7::DEG[];
constexpr int DVB_S2_TABLE_C7::LEN[];
constexpr int DVB_S2_TABLE_C7::POS[];

constexpr int DVB_S2_TABLE_C8::DEG[];
constexpr int DVB_S2_TABLE_C8::LEN[];
constexpr int DVB_S2_TABLE_C8::POS[];

constexpr int DVB_S2_TABLE_C9::DEG[];
constexpr int DVB_S2_TABLE_C9::LEN[];
constexpr int DVB_S2_TABLE_C9::POS[];

constexpr int DVB_S2_TABLE_C10::DEG[];
constexpr int DVB_S2_TABLE_C10::LEN[];
constexpr int DVB_S2_TABLE_C10::POS[];

constexpr int DVB_S2X_TABLE_C1::DEG[];
constexpr int DVB_S2X_TABLE_C1::LEN[];
constexpr int DVB_S2X_TABLE_C1::POS[];

constexpr int DVB_S2X_TABLE_C2::DEG[];
constexpr int DVB_S2X_TABLE_C2::LEN[];
constexpr int DVB_S2X_TABLE_C2::POS[];

constexpr int DVB_S2X_TABLE_C3::DEG[];
constexpr int DVB_S2X_TABLE_C3::LEN[];
constexpr int DVB_S2X_TABLE_C3::POS[];

constexpr int DVB_S2X_TABLE_C4::DEG[];
constexpr int DVB_S2X_TABLE_C4::LEN[];
constexpr int DVB_S2X_TABLE_C4::POS[];

constexpr int DVB_S2X_TABLE_C5::DEG[];
constexpr int DVB_S2X_TABLE_C5::LEN[];
constexpr int DVB_S2X_TABLE_C5::POS[];

constexpr int DVB_S2X_TABLE_C6::DEG[];
constexpr int DVB_S2X_TABLE_C6::LEN[];
constexpr int DVB_S2X_TABLE_C6::POS[];

constexpr int DVB_S2X_TABLE_C7::DEG[];
constexpr int DVB_S2X_TABLE_C7::LEN[];
constexpr int DVB_S2X_TABLE_C7::POS[];

constexpr int DVB_S2X_TABLE_C8::DEG[];
constexpr int DVB_S2X_TABLE_C8::LEN[];
constexpr int DVB_S2X_TABLE_C8::POS[];

constexpr int DVB_S2X_TABLE_C9::DEG[];
constexpr int DVB_S2X_TABLE_C9::LEN[];
constexpr int DVB_S2X_TABLE_C9::POS[];

constexpr int DVB_S2X_TABLE_C10::DEG[];
constexpr int DVB_S2X_TABLE_C10::LEN[];
constexpr int DVB_S2X_TABLE_C10::POS[];

namespace gr {
namespace dvbs2acm {

using input_type = gr_complex;
using output_type = unsigned char;

ldpc_decoder_cb::sptr
ldpc_decoder_cb::make(bool outputmode, bool infomode, int max_trials, int debug_level)
{
    return gnuradio::make_block_sptr<ldpc_decoder_cb_impl>(
        outputmode, infomode, max_trials, debug_level);
}


/*
 * The private constructor
 */
ldpc_decoder_cb_impl::ldpc_decoder_cb_impl(bool outputmode,
                                           bool infomode,
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
      decode(nullptr),
      init(nullptr)
{
    std::string impl = "generic";
#ifdef CPU_FEATURES_ARCH_ANY_ARM
    d_simd_size = 16;
    if (has_neon) {
        init = &ldpc_neon::ldpc_dec_init;
        decode = &ldpc_neon::ldpc_dec_decode;
        impl = "neon";
    } else {
        init = &ldpc_generic::ldpc_dec_init;
        decode = &ldpc_generic::ldpc_dec_decode;
    }
#else
#ifdef CPU_FEATURES_ARCH_X86
    d_simd_size = features.avx2 ? 32 : 16;
    if (features.avx2) {
        init = &ldpc_avx2::ldpc_dec_init;
        decode = &ldpc_avx2::ldpc_dec_decode;
        impl = "avx2";
    } else if (features.sse4_1) {
        init = &ldpc_sse41::ldpc_dec_init;
        decode = &ldpc_sse41::ldpc_dec_decode;
        impl = "sse4_1";
    } else {
        init = &ldpc_generic::ldpc_dec_init;
        decode = &ldpc_generic::ldpc_dec_decode;
    }
#else
    // Not ARM, nor x86. Use generic implementation.
    d_simd_size = 16;
    init = &ldpc_generic::ldpc_dec_init;
    decode = &ldpc_generic::ldpc_dec_decode;
#endif
#endif
    assert(init != nullptr);
    assert(decode != nullptr);
    d_debug_logger->debug("LDPC decoder implementation: {:s}", impl);

    soft.resize(FRAME_SIZE_NORMAL * d_simd_size);
    dint.resize(FRAME_SIZE_NORMAL * d_simd_size);
    tempu.resize(FRAME_SIZE_NORMAL);
    tempv.resize(FRAME_SIZE_NORMAL);
    aligned_buffer = aligned_alloc(d_simd_size, d_simd_size * FRAME_SIZE_NORMAL);
    set_output_multiple(FRAME_SIZE_NORMAL * d_simd_size);
    // if (outputmode) { // == OM_MESSAGE
    //     set_output_multiple(nbch * d_simd_size);
    //     set_relative_rate((double)nbch / frame_size);
    // } else { // == OM_CODEWORD
    //     set_output_multiple(frame_size * d_simd_size);
    // }
}

/*
 * Our virtual destructor.
 */
ldpc_decoder_cb_impl::~ldpc_decoder_cb_impl()
{
    free(aligned_buffer);
    mod.reset();
    ldpc.reset();
}

void ldpc_decoder_cb_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    if (mod) {
        ninput_items_required[0] = noutput_items / mod->bits();
    } else {
        ninput_items_required[0] = noutput_items / 2; // QPSK is the default
    }
    // if (output_mode) { // == OM_MESSAGE
    //     ninput_items_required[0] = (noutput_items / nbch) * (frame_size / mod->bits());
    // } else { // == OM_CODEWORD
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
    auto in = static_cast<const input_type*>(input_items[0]);
    auto insnr = static_cast<const input_type*>(input_items[0]);
    auto out = static_cast<output_type*>(output_items[0]);
    float sp, np, sigma, precision_sum;
    gr_complex s, e;
    const int trials = (d_max_trials == 0) ? DEFAULT_TRIALS : d_max_trials;
    int consumed = 0;
    int rows, offset, indexin, indexout;
    const int* mux;
    // int output_size = output_mode ? nbch : frame_size;

    std::vector<tag_t> tags;
    const uint64_t nread = this->nitems_read(0); // number of items read on port 0

    // Read all tags on the input buffer
    this->get_tags_in_range(tags, 0, nread, nread + noutput_items, pmt::intern("pls"));

    // TODO: SIMD stuff ???
    for (tag_t tag : tags) {
        auto dict = tag.value;
        dvbs2_modcod_t modcod;
        dvbs2_vlsnr_header_t vlsnr_header;
        if (dict->is_dict() && pmt::dict_has_key(dict, pmt::intern("modcod")) &&
            pmt::dict_has_key(dict, pmt::intern("vlsnr_header"))) {
            auto not_found = pmt::get_PMT_NIL();

            auto modcod_r = pmt::dict_ref(dict, pmt::intern("modcod"), not_found);
            if (modcod_r == not_found) {
                continue;
            }
            modcod = (dvbs2_modcod_t)pmt::to_long(modcod_r);

            auto vlsnr_header_r = pmt::dict_ref(dict, pmt::intern("vlsnr_header"), not_found);
            if (vlsnr_header_r == not_found) {
                continue;
            }
            vlsnr_header = (dvbs2_vlsnr_header_t)pmt::to_long(vlsnr_header_r);
        } else {
            continue;
        }
        auto framesize = modcod_framesize(modcod);
        auto rate = modcod_rate(modcod);
        auto constellation = modcod_constellation(modcod);
        if (modcod == MC_VLSNR_SET1 || modcod == MC_VLSNR_SET2) {
            framesize = vlsnr_framesize(vlsnr_header);
            rate = vlsnr_rate(vlsnr_header);
            constellation = vlsnr_constellation(vlsnr_header);
        }

        ldpc.reset(build_decoder(framesize, rate));
        init(ldpc.get());

        // TODO: Make a class
        switch (constellation) {
        case MOD_QPSK:
            mod.reset(new ModulationQPSK<gr_complex, int8_t>());
            break;
        case MOD_8PSK:
            mod.reset(new Modulation8PSK<gr_complex, int8_t>());
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
        default:
            break;
        }

        int code_len = ldpc->code_len();
        int mod_bits = mod->bits();
        int8_t tmp[mod_bits];
        int8_t* code = nullptr;
        int symbols = code_len / mod_bits;

        // Determine target precision for demodulator based on noise level
        if (frame == 0) {
            sp = 0;
            np = 0;
            for (int j = 0; j < symbols; j++) {
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
        for (int j = 0; j < symbols; j++) {
            mod->soft(soft.data() + (j * mod_bits /*+ (blk * CODE_LEN)*/), in[j], precision);
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
        rows = frame_size / mod_bits;
        switch (constellation) {
        case MOD_8PSK:
            indexin = 0;
            for (int j = 0; j < rows; j++) {
                dint[rowaddr0 + j] = soft[indexin++];
                dint[rowaddr1 + j] = soft[indexin++];
                dint[rowaddr2 + j] = soft[indexin++];
            }
            code = dint.data();
            break;
        case MOD_QPSK:
        default:
            code = soft.data();
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
        chunk++;

        // Evaluate corrected SNR
        // For that, codeword needs to be interleaved and modulated
        switch (constellation) {
        case MOD_QPSK:
            for (int j = 0; j < code_len; j++) {
                tempv[j] = code[j] < 0 ? -1 : 1;
            }
            break;
        case MOD_8PSK:
            for (int j = 0; j < code_len; j++) {
                tempu[j] = code[j] < 0 ? -1 : 1;
            }
            rows = frame_size / mod_bits;
            indexout = 0;
            for (int j = 0; j < rows; j++) {
                tempv[indexout++] = tempu[rowaddr0 + j];
                tempv[indexout++] = tempu[rowaddr1 + j];
                tempv[indexout++] = tempu[rowaddr2 + j];
            }
            break;
        default:
            break;
        }
        sp = 0;
        np = 0;
        precision_sum = 0;
        for (int j = 0; j < symbols; j++) {
            s = mod->map(&tempv[(j * mod_bits)]);
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
        insnr += frame_size / mod_bits;
        frame++;

        // Produce information bits
        precision = precision_sum / d_simd_size;
        for (int j = 0; j < code_len; j++) {
            *out++ = code[j] < 0 ? 1 : 0;
        }
        produce(0, code_len);
    }
    consume_each(consumed);
    return WORK_CALLED_PRODUCE;
}

} /* namespace dvbs2acm */
} /* namespace gr */
