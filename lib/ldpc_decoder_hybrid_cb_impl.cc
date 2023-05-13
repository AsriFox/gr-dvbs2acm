/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox
 * Copyright 2018,2019 Ahmet Inan, Ron Economos.
 *
 * This file is part of gr-dvbs2acm.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "modcod.hh"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "cpu_features_macros.h"
#include "debug_level.h"
#include "ldpc_decoder/dvb_s2_tables.hh"
#include "ldpc_decoder/dvb_s2x_tables.hh"
#include "ldpc_decoder_hybrid_cb_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/logger.h>
#include <boost/format.hpp>
#include <cmath>

#ifdef CPU_FEATURES_ARCH_ARM
#include "cpuinfo_arm.h"
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

ldpc_decoder_hybrid_cb::sptr
ldpc_decoder_hybrid_cb::make(bool infomode, int max_trials, int debug_level)
{
    return gnuradio::get_initial_sptr(
        new ldpc_decoder_hybrid_cb_impl(infomode, max_trials, debug_level));
}

/*
 * The private constructor
 */
ldpc_decoder_hybrid_cb_impl::ldpc_decoder_hybrid_cb_impl(bool infomode, int max_trials, int debug_level)
    : gr::block("ldpc_decoder_cb",
                gr::io_signature::make(1, 1, sizeof(input_type)),
                gr::io_signature::make(1, 1, sizeof(output_type))),
      d_debug_level(debug_level),
      info_mode(infomode),
      d_max_trials(max_trials),
      total_snr(0),
      frame(0),
      chunk(0),
      total_trials(0),
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
    d_logger->debug("LDPC decoder implementation: {:s}", impl);

    tempu.resize(FRAME_SIZE_NORMAL);
    tempv.resize(FRAME_SIZE_NORMAL);
    soft.resize(d_simd_size * FRAME_SIZE_NORMAL);
    aligned_buffer = aligned_alloc(d_simd_size, d_simd_size * FRAME_SIZE_NORMAL);
    set_output_multiple(FRAME_SIZE_NORMAL * d_simd_size);
    set_tag_propagation_policy(TPP_CUSTOM);
}

/*
 * Our virtual destructor.
 */
ldpc_decoder_hybrid_cb_impl::~ldpc_decoder_hybrid_cb_impl() { free(aligned_buffer); }

void ldpc_decoder_hybrid_cb_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    if (mod && ldpc) {
        ninput_items_required[0] =
            (noutput_items / ldpc->data_len()) * (ldpc->code_len() / mod->bits());
    } else {
        ninput_items_required[0] = noutput_items;
    }
}

void ldpc_decoder_hybrid_cb_impl::get_rows(dvbs2_framesize_t framesize,
                                           dvbs2_code_rate_t rate,
                                           dvbs2_constellation_t constellation)
{
    int rows;

    if (framesize == FECFRAME_NORMAL) {
        frame_size = FRAME_SIZE_NORMAL;
        if (rate == C2_9_VLSNR) {
            frame_size = FRAME_SIZE_NORMAL - NORMAL_PUNCTURING;
        }
    } else if (framesize == FECFRAME_SHORT) {
        frame_size = FRAME_SIZE_SHORT;
        if (rate == C1_5_VLSNR_SF2 || rate == C11_45_VLSNR_SF2) {
            frame_size = FRAME_SIZE_SHORT - SHORT_PUNCTURING_SET1;
        }
        if (rate == C1_5_VLSNR || rate == C4_15_VLSNR || rate == C1_3_VLSNR) {
            frame_size = FRAME_SIZE_SHORT - SHORT_PUNCTURING_SET2;
        }
    } else {
        frame_size = FRAME_SIZE_MEDIUM - MEDIUM_PUNCTURING;
    }

    switch (constellation) {
    case MOD_BPSK:
        mod_order = 1;
        rows = frame_size / mod_order;
        break;
    case MOD_BPSK_SF2:
        mod_order = 1;
        rows = frame_size / mod_order;
        break;
    case MOD_QPSK:
        mod_order = 2;
        rows = frame_size / mod_order;
        break;
    case MOD_8PSK:
        mod_order = 3;
        rows = frame_size / mod_order;
        /* 210 */
        if (rate == C3_5) {
            rowaddr[0] = rows * 2;
            rowaddr[1] = rows;
            rowaddr[2] = 0;
        }
        /* 102 */
        else if (rate == C25_36 || rate == C13_18 || rate == C7_15 || rate == C8_15 || rate == C26_45) {
            rowaddr[0] = rows;
            rowaddr[1] = 0;
            rowaddr[2] = rows * 2;
        }
        /* 012 */
        else {
            rowaddr[0] = 0;
            rowaddr[1] = rows;
            rowaddr[2] = rows * 2;
        }
        break;
    case MOD_8APSK:
        mod_order = 3;
        rows = frame_size / mod_order;
        /* 012 */
        rowaddr[0] = 0;
        rowaddr[1] = rows;
        rowaddr[2] = rows * 2;
        break;
    case MOD_16APSK:
        mod_order = 4;
        rows = frame_size / mod_order;
        if (rate == C26_45) {
            /* 3201 */
            if (frame_size == FRAME_SIZE_NORMAL) {
                rowaddr[0] = rows * 3;
                rowaddr[1] = rows * 2;
                rowaddr[2] = 0;
                rowaddr[3] = rows;
            }
            /* 2130 */
            else {
                rowaddr[0] = rows * 2;
                rowaddr[1] = rows;
                rowaddr[2] = rows * 3;
                rowaddr[3] = 0;
            }
        } else if (rate == C3_5) {
            /* 3210 */
            if (frame_size == FRAME_SIZE_NORMAL) {
                rowaddr[0] = rows * 3;
                rowaddr[1] = rows * 2;
                rowaddr[2] = rows;
                rowaddr[3] = 0;
            }
            /* 3201 */
            else {
                rowaddr[0] = rows * 3;
                rowaddr[1] = rows * 2;
                rowaddr[2] = 0;
                rowaddr[3] = rows;
            }
        }
        /* 3012 */
        else if (rate == C28_45) {
            rowaddr[0] = rows * 3;
            rowaddr[1] = 0;
            rowaddr[2] = rows;
            rowaddr[3] = rows * 2;
        }
        /* 3021 */
        else if (rate == C23_36 || rate == C13_18) {
            rowaddr[0] = rows * 3;
            rowaddr[1] = 0;
            rowaddr[2] = rows * 2;
            rowaddr[3] = rows;
        }
        /* 2310 */
        else if (rate == C25_36) {
            rowaddr[0] = rows * 2;
            rowaddr[1] = rows * 3;
            rowaddr[2] = rows;
            rowaddr[3] = 0;
        }
        /* 2103 */
        else if (rate == C7_15 || rate == C8_15) {
            rowaddr[0] = rows * 2;
            rowaddr[1] = rows;
            rowaddr[2] = 0;
            rowaddr[3] = rows * 3;
        }
        /* 3210 */
        else if (rate == C140_180) {
            rowaddr[0] = rows * 3;
            rowaddr[1] = rows * 2;
            rowaddr[2] = rows;
            rowaddr[3] = 0;
        }
        /* 0321 */
        else if (rate == C154_180) {
            rowaddr[0] = 0;
            rowaddr[1] = rows * 3;
            rowaddr[2] = rows * 2;
            rowaddr[3] = rows;
        }
        /* 0123 */
        else {
            rowaddr[0] = 0;
            rowaddr[1] = rows;
            rowaddr[2] = rows * 2;
            rowaddr[3] = rows * 3;
        }
        break;
    case MOD_8_8APSK:
        mod_order = 4;
        rows = frame_size / mod_order;
        /* 3210 */
        if (rate == C90_180) {
            rowaddr[0] = rows * 3;
            rowaddr[1] = rows * 2;
            rowaddr[2] = rows;
            rowaddr[3] = 0;
        }
        /* 2310 */
        else if (rate == C96_180) {
            rowaddr[0] = rows * 2;
            rowaddr[1] = rows * 3;
            rowaddr[2] = rows;
            rowaddr[3] = 0;
        }
        /* 2301 */
        else if (rate == C100_180) {
            rowaddr[0] = rows * 2;
            rowaddr[1] = rows * 3;
            rowaddr[2] = 0;
            rowaddr[3] = rows;
        }
        /* 0123 */
        else {
            rowaddr[0] = 0;
            rowaddr[1] = rows;
            rowaddr[2] = rows * 2;
            rowaddr[3] = rows * 3;
        }
        break;
    case MOD_32APSK:
        mod_order = 5;
        rows = frame_size / mod_order;
        /* 01234 */
        rowaddr[0] = 0;
        rowaddr[1] = rows;
        rowaddr[2] = rows * 2;
        rowaddr[3] = rows * 3;
        rowaddr[4] = rows * 4;
        break;
    case MOD_4_12_16APSK:
        mod_order = 5;
        rows = frame_size / mod_order;
        /* 21430 */
        if (frame_size == FRAME_SIZE_NORMAL) {
            rowaddr[0] = rows * 2;
            rowaddr[1] = rows;
            rowaddr[2] = rows * 4;
            rowaddr[3] = rows * 3;
            rowaddr[4] = 0;
        } else {
            /* 41230 */
            if (rate == C2_3) {
                rowaddr[0] = rows * 4;
                rowaddr[1] = rows;
                rowaddr[2] = rows * 2;
                rowaddr[3] = rows * 3;
                rowaddr[4] = 0;
            }
            /* 10423 */
            else if (rate == C32_45) {
                rowaddr[0] = rows;
                rowaddr[1] = 0;
                rowaddr[2] = rows * 4;
                rowaddr[3] = rows * 2;
                rowaddr[4] = rows * 3;
            }
        }
        break;
    case MOD_4_8_4_16APSK:
        mod_order = 5;
        rows = frame_size / mod_order;
        /* 40213 */
        if (rate == C140_180) {
            rowaddr[0] = rows * 4;
            rowaddr[1] = 0;
            rowaddr[2] = rows * 2;
            rowaddr[3] = rows;
            rowaddr[4] = rows * 3;
        }
        /* 40312 */
        else {
            rowaddr[0] = rows * 4;
            rowaddr[1] = 0;
            rowaddr[2] = rows * 3;
            rowaddr[3] = rows;
            rowaddr[4] = rows * 2;
        }
        break;
    default:
        mod_order = 2;
        rows = frame_size / mod_order;
        break;
    }
}

const int DEFAULT_TRIALS = 25;
#define FACTOR 2 // same factor used on the decoder implementation

int ldpc_decoder_hybrid_cb_impl::general_work(int noutput_items,
                                              gr_vector_int& ninput_items,
                                              gr_vector_const_void_star& input_items,
                                              gr_vector_void_star& output_items)
{
    auto in = static_cast<const input_type*>(input_items[0]);
    auto insnr = static_cast<const input_type*>(input_items[0]);
    auto out = static_cast<output_type*>(output_items[0]);
    auto code = reinterpret_cast<int8_t*>(output_items[0]);
    int consumed_total = 0;
    int produced_total = 0;
    const int trials = (d_max_trials == 0) ? DEFAULT_TRIALS : d_max_trials;

    std::vector<tag_t> tags;
    const uint64_t nread = this->nitems_read(0); // number of items read on port 0

    // Read all tags on the input buffer
    this->get_tags_in_range(tags, 0, nread, nread + ninput_items[0], pmt::intern("pls"));

    for (size_t i = 0; i < tags.size(); i += d_simd_size) {
        if (i + d_simd_size > tags.size()) {
            break;
        }
        dvbs2_modcod_t modcod = MC_DUMMY;
        dvbs2_vlsnr_header_t vlsnr_header = VLSNR_DUMMY;
        for (int blk = 0; blk < d_simd_size; blk++) {
            auto dict = tags[i + blk].value;
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
        }

        dvbs2_constellation_t constellation;
        dvbs2_framesize_t framesize;
        dvbs2_code_rate_t rate;
        if (modcod == MC_VLSNR_SET1 || modcod == MC_VLSNR_SET2) {
            constellation = vlsnr_constellation(vlsnr_header);
            framesize = vlsnr_framesize(vlsnr_header);
            rate = vlsnr_rate(vlsnr_header);
        } else {
            constellation = modcod_constellation(modcod);
            framesize = modcod_framesize(modcod);
            rate = modcod_rate(modcod);
        }

        if (this->constellation != constellation) {
            this->constellation = constellation;
            switch (constellation) {
            case MOD_BPSK:
            case MOD_BPSK_SF2:
                mod.reset(new PhaseShiftKeying<2, gr_complex, int8_t>());
                break;
            case MOD_QPSK:
                mod.reset(new PhaseShiftKeying<4, gr_complex, int8_t>());
                break;
            case MOD_8PSK:
                mod.reset(new PhaseShiftKeying<8, gr_complex, int8_t>());
                break;
            default:
                // TODO
                consume_each(consumed_total);
                return WORK_CALLED_PRODUCE;
            }
        }
        if (this->framesize != framesize || this->rate != rate) {
            this->framesize = framesize;
            this->rate = rate;

            ldpc.reset(build_decoder(framesize, rate));
            init(ldpc.get());

            auto interpolation = ldpc->data_len() * d_simd_size;
            auto decimation = ldpc->code_len() * d_simd_size * mod->bits();
            set_relative_rate(interpolation, decimation);
            set_output_multiple(interpolation);

            d_logger->debug("LDPC decoder changed to modcod {:d}", modcod);
            // GR_LOG_DEBUG_LEVEL(1, "LDPC decoder changed to modcod {:d}", modcod);
        }
        if (ldpc->data_len() * d_simd_size + produced_total > noutput_items) {
            break;
        }

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
        get_rows(framesize, rate, constellation);

        auto psoft = soft.data();
        auto abs_offset = nitems_written(0);
        for (int blk = 0; blk < d_simd_size; blk++) {
            auto key = tags[i + blk].key;
            auto value = tags[i + blk].value;
            add_item_tag(0, abs_offset, key, value);
            abs_offset += ldpc->data_len();

            int symbols = frame_size / mod_order;
            int8_t tmp[mod_order];
            float sp = 0, np = 0;
            gr_complex s, e;
            if (frame == 0) {
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
                float sigma = std::sqrt(np / (2 * sp));
                precision = FACTOR / (sigma * sigma);
            }
            for (int j = 0; j < symbols; j++) {
                mod->soft(psoft + j * mod_order, in[j], precision);
            }
            int rows = frame_size / mod_order; // = symbols;
            switch (constellation) {
            case MOD_8PSK:
            case MOD_8APSK:
                for (int j = 0; j < rows; j++) {
                    code[rowaddr[0] + j] = *psoft++;
                    code[rowaddr[1] + j] = *psoft++;
                    code[rowaddr[2] + j] = *psoft++;
                }
                break;
            case MOD_16APSK:
            case MOD_8_8APSK:
                for (int j = 0; j < rows; j++) {
                    code[rowaddr[0] + j] = *psoft++;
                    code[rowaddr[1] + j] = *psoft++;
                    code[rowaddr[2] + j] = *psoft++;
                    code[rowaddr[3] + j] = *psoft++;
                }
                break;
            case MOD_32APSK:
            case MOD_4_12_16APSK:
            case MOD_4_8_4_16APSK:
                for (int j = 0; j < rows; j++) {
                    code[rowaddr[0] + j] = *psoft++;
                    code[rowaddr[1] + j] = *psoft++;
                    code[rowaddr[2] + j] = *psoft++;
                    code[rowaddr[3] + j] = *psoft++;
                    code[rowaddr[4] + j] = *psoft++;
                }
                break;
            case MOD_BPSK:
            case MOD_BPSK_SF2:
            case MOD_QPSK:
            default:
                memcpy(code, psoft, sizeof(int8_t) * frame_size);
                psoft += frame_size;
                break;
            }
            in += symbols;
            consumed_total += symbols;
        }

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

        float precision_sum = 0;
        for (int blk = 0; blk < d_simd_size; blk++) {
            int rows = frame_size / mod_order;
            switch (constellation) {
            case MOD_QPSK:
                for (unsigned j = 0; j < frame_size; j++) {
                    tempv[j] = code[j + (blk * frame_size)] < 0 ? -1 : 1;
                }
                break;
            case MOD_8PSK:
                for (unsigned j = 0; j < frame_size; j++) {
                    tempu[j] = code[j + (blk * frame_size)] < 0 ? -1 : 1;
                }
                for (int j = 0; j < rows; j++) {
                    tempv[j] = tempu[j + rowaddr[0]];
                    tempv[j] = tempu[j + rowaddr[1]];
                    tempv[j] = tempu[j + rowaddr[2]];
                }
                break;
            default:
                break;
            }

            int symbols = frame_size / mod_order;
            float sp = 0, np = 0;
            gr_complex s, e;
            for (int j = 0; j < symbols; j++) {
                s = mod->map(&tempv[(j * mod_order)]);
                e = insnr[j] - s;
                sp += std::norm(s);
                np += std::norm(e);
            }
            if (!(np > 0)) {
                np = 1e-12;
            }
            snr = 10 * std::log10(sp / np);
            float sigma = std::sqrt(np / (2 * sp));
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
            insnr += frame_size / mod_order;
            frame++;
        }
        precision = precision_sum / d_simd_size;

        auto code_len = ldpc->code_len();
        auto data_len = ldpc->data_len();
        for (int blk = 0; blk < d_simd_size; blk++) {
            for (int j = 0; j < data_len; j++) {
                *out++ = code[j] >= 0 ? 0 : 1;
            }
            code += code_len;
        }
        produced_total += data_len;
        produce(0, data_len * d_simd_size);
    }
    consume_each(consumed_total);
    return WORK_CALLED_PRODUCE;
}

} // namespace dvbs2acm
} /* namespace gr */
