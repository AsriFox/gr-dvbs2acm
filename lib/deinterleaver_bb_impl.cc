/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "deinterleaver_bb_impl.h"
#include "modcod.hh"
#include <gnuradio/io_signature.h>

namespace gr {
namespace dvbs2acm {

using input_type = char;
using output_type = char;
deinterleaver_bb::sptr deinterleaver_bb::make()
{
    return gnuradio::make_block_sptr<deinterleaver_bb_impl>();
}


/*
 * The private constructor
 */
deinterleaver_bb_impl::deinterleaver_bb_impl()
    : gr::tagged_stream_block(
          "deinterleaver_bb",
          gr::io_signature::make(1 /* min inputs */, 1 /* max inputs */, sizeof(input_type)),
          gr::io_signature::make(1 /* min outputs */, 1 /*max outputs */, sizeof(output_type)),
          "frame_size")
{
}

/*
 * Our virtual destructor.
 */
deinterleaver_bb_impl::~deinterleaver_bb_impl() {}

void deinterleaver_bb_impl::parse_length_tags(const std::vector<std::vector<tag_t>>& tags,
                                              gr_vector_int& n_input_items_reqd)
{
    dvbs2_modcod_t modcod = MC_DUMMY;
    dvbs2_vlsnr_header_t vlsnr_header;
    for (tag_t tag : tags[0]) {
        if (tag.key == pmt::intern("pls") && tag.value->is_dict()) {
            auto dict = tag.value;
            if (pmt::dict_has_key(dict, pmt::intern("modcod")) &&
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
            }
        }
    }
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
    get_rows(framesize, rate, constellation);
    n_input_items_reqd[0] = frame_size;
}

void deinterleaver_bb_impl::get_rows(dvbs2_framesize_t framesize,
                                     dvbs2_code_rate_t rate,
                                     dvbs2_constellation_t constellation)
{
    int mod, rows;

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
        mod = 1;
        rows = frame_size / mod;
        break;
    case MOD_BPSK_SF2:
        mod = 1;
        rows = frame_size / mod;
        break;
    case MOD_QPSK:
        mod = 2;
        rows = frame_size / mod;
        break;
    case MOD_8PSK:
        mod = 3;
        rows = frame_size / mod;
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
        mod = 3;
        rows = frame_size / mod;
        /* 012 */
        rowaddr[0] = 0;
        rowaddr[1] = rows;
        rowaddr[2] = rows * 2;
        break;
    case MOD_16APSK:
        mod = 4;
        rows = frame_size / mod;
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
        mod = 4;
        rows = frame_size / mod;
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
        mod = 5;
        rows = frame_size / mod;
        /* 01234 */
        rowaddr[0] = 0;
        rowaddr[1] = rows;
        rowaddr[2] = rows * 2;
        rowaddr[3] = rows * 3;
        rowaddr[4] = rows * 4;
        break;
    case MOD_4_12_16APSK:
        mod = 5;
        rows = frame_size / mod;
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
        mod = 5;
        rows = frame_size / mod;
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
        mod = 2;
        rows = frame_size / mod;
        break;
    }
    mod_order = mod;
}

int deinterleaver_bb_impl::work(int noutput_items,
                                gr_vector_int& ninput_items,
                                gr_vector_const_void_star& input_items,
                                gr_vector_void_star& output_items)
{
    auto in = static_cast<const input_type*>(input_items[0]);
    auto out = static_cast<output_type*>(output_items[0]);
    int rows = frame_size / mod_order;
    switch (constellation) {
    case MOD_8PSK:
    case MOD_8APSK:
        for (int j = 0; j < rows; j++) {
            out[rowaddr[0] + j] = *in++;
            out[rowaddr[1] + j] = *in++;
            out[rowaddr[2] + j] = *in++;
        }
        break;
    case MOD_16APSK:
    case MOD_8_8APSK:
        for (int j = 0; j < rows; j++) {
            out[rowaddr[0] + j] = *in++;
            out[rowaddr[1] + j] = *in++;
            out[rowaddr[2] + j] = *in++;
            out[rowaddr[3] + j] = *in++;
        }
        break;
    case MOD_32APSK:
    case MOD_4_12_16APSK:
    case MOD_4_8_4_16APSK:
        for (int j = 0; j < rows; j++) {
            out[rowaddr[0] + j] = *in++;
            out[rowaddr[1] + j] = *in++;
            out[rowaddr[2] + j] = *in++;
            out[rowaddr[3] + j] = *in++;
            out[rowaddr[4] + j] = *in++;
        }
        break;
    case MOD_BPSK:
    case MOD_BPSK_SF2:
    case MOD_QPSK:
    default:
        memcpy(out, in, sizeof(input_type) * ninput_items[0]);
        break;
    }
    return noutput_items;
}

} /* namespace dvbs2acm */
} /* namespace gr */
