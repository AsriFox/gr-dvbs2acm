/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2014,2016 Ron Economos.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "interleaver_bb_impl.h"
#include "modcod.hh"
#include <gnuradio/io_signature.h>
#include <pmt/pmt.h>
#include <cassert>
#include <cstdint>
#include <vector>

namespace gr {
namespace dvbs2acm {

using input_type = unsigned char;
using output_type = unsigned char;

interleaver_bb::sptr interleaver_bb::make() { return gnuradio::make_block_sptr<interleaver_bb_impl>(); }


/*
 * The private constructor
 */
interleaver_bb_impl::interleaver_bb_impl()
    : gr::tagged_stream_block("interleaver_bb",
                              gr::io_signature::make(1, 1, sizeof(input_type)),
                              gr::io_signature::make(1, 1, sizeof(output_type)),
                              "frame_length")
{
    set_tag_propagation_policy(TPP_ALL_TO_ALL);
}

/*
 * Our virtual destructor.
 */
interleaver_bb_impl::~interleaver_bb_impl() {}

void interleaver_bb_impl::parse_length_tags(const std::vector<std::vector<tag_t>>& tags,
                                            gr_vector_int& n_input_items_reqd)
{
    dvbs2_modcod_t modcod;
    dvbs2_vlsnr_header_t vlsnr_header;
    for (tag_t tag : tags[0]) {
        if (tag.key == pmt::intern("frame_length")) {
            n_input_items_reqd[0] = pmt::to_long(tag.value);
            remove_item_tag(0, tag);
        } else if (tag.key == pmt::intern("modcod")) {
            modcod = (dvbs2_modcod_t)(pmt::to_long(tag.value) & 0x7f);
        } else if (tag.key == pmt::intern("vlsnr_header")) {
            vlsnr_header = (dvbs2_vlsnr_header_t)(pmt::to_long(tag.value) & 0x0f);
        }
    }
    constellation = get_rows(modcod, vlsnr_header, frame_size, mod_order);
}

dvbs2_constellation_t interleaver_bb_impl::get_rows(dvbs2_modcod_t modcod,
                                                    dvbs2_vlsnr_header_t vlsnr_header,
                                                    int& frame_size,
                                                    int& mod_order)
{
    auto framesize = modcod_framesize(modcod);
    auto rate = modcod_rate(modcod);
    auto constellation = modcod_constellation(modcod);
    if (modcod == MC_VLSNR_SET1 || modcod == MC_VLSNR_SET2) {
        framesize = vlsnr_framesize(vlsnr_header);
        rate = vlsnr_rate(vlsnr_header);
        constellation = vlsnr_constellation(vlsnr_header);
    }

    if (framesize == FECFRAME_NORMAL) {
        if (rate == C2_9_VLSNR) {
            frame_size = FRAME_SIZE_NORMAL - NORMAL_PUNCTURING;
        } else {
            frame_size = FRAME_SIZE_NORMAL;
        }
    } else if (framesize == FECFRAME_SHORT) {
        switch (rate) {
        case C1_5_VLSNR_SF2:
        case C11_45_VLSNR_SF2:
            frame_size = FRAME_SIZE_SHORT - SHORT_PUNCTURING_SET1;
            break;
        case C1_5_VLSNR:
        case C4_15_VLSNR:
        case C1_3_VLSNR:
            frame_size = FRAME_SIZE_SHORT - SHORT_PUNCTURING_SET2;
            break;
        default:
            frame_size = FRAME_SIZE_SHORT;
            break;
        }
    } else {
        frame_size = FRAME_SIZE_MEDIUM - MEDIUM_PUNCTURING;
    }

    int rows;
    switch (constellation) {
    case MOD_BPSK:
    case MOD_BPSK_SF2:
        mod_order = 1;
        break;
    case MOD_QPSK:
        mod_order = 2;
        break;
    case MOD_8APSK:
        mod_order = 3;
        rows = frame_size / mod_order;
        /* 012 */
        rowaddr[0] = 0;
        rowaddr[1] = rows;
        rowaddr[2] = rows * 2;
        break;
    case MOD_8PSK:
        mod_order = 3;
        rows = frame_size / mod_order;
        switch (rate) {
        case C3_5:
            /* 210 */
            rowaddr[0] = rows * 2;
            rowaddr[1] = rows;
            rowaddr[2] = 0;
            break;
        case C25_36:
        case C13_18:
        case C7_15:
        case C8_15:
        case C26_45:
            /* 102 */
            rowaddr[0] = rows;
            rowaddr[1] = 0;
            rowaddr[2] = rows * 2;
            break;
        default:
            /* 012 */
            rowaddr[0] = 0;
            rowaddr[1] = rows;
            rowaddr[2] = rows * 2;
            break;
        }
        break;
    case MOD_16APSK:
        mod_order = 4;
        rows = frame_size / mod_order;
        switch (rate) {
        case C26_45:
            if (framesize == FECFRAME_NORMAL) {
                /* 3201 */
                rowaddr[0] = rows * 3;
                rowaddr[1] = rows * 2;
                rowaddr[2] = 0;
                rowaddr[3] = rows;
            } else {
                /* 2130 */
                rowaddr[0] = rows * 2;
                rowaddr[1] = rows;
                rowaddr[2] = rows * 3;
                rowaddr[3] = 0;
            }
            break;
        case C3_5:
            if (frame_size == FECFRAME_NORMAL) {
                /* 3210 */
                rowaddr[0] = rows * 3;
                rowaddr[1] = rows * 2;
                rowaddr[2] = rows;
                rowaddr[3] = 0;
            } else {
                /* 3201 */
                rowaddr[0] = rows * 3;
                rowaddr[1] = rows * 2;
                rowaddr[2] = 0;
                rowaddr[3] = rows;
            }
            break;
        case C28_45:
            /* 3012 */
            rowaddr[0] = rows * 3;
            rowaddr[1] = 0;
            rowaddr[2] = rows;
            rowaddr[3] = rows * 2;
            break;
        case C23_36:
        case C13_18:
            /* 3021 */
            rowaddr[0] = rows * 3;
            rowaddr[1] = 0;
            rowaddr[2] = rows * 2;
            rowaddr[3] = rows;
            break;
        case C25_36:
            /* 2310 */
            rowaddr[0] = rows * 2;
            rowaddr[1] = rows * 3;
            rowaddr[2] = rows;
            rowaddr[3] = 0;
            break;
        case C7_15:
        case C8_15:
            /* 2103 */
            rowaddr[0] = rows * 2;
            rowaddr[1] = rows;
            rowaddr[2] = 0;
            rowaddr[3] = rows * 3;
            break;
        case C140_180:
            /* 3210 */
            rowaddr[0] = rows * 3;
            rowaddr[1] = rows * 2;
            rowaddr[2] = rows;
            rowaddr[3] = 0;
            break;
        case C154_180:
            /* 0321 */
            rowaddr[0] = 0;
            rowaddr[1] = rows * 3;
            rowaddr[2] = rows * 2;
            rowaddr[3] = rows;
            break;
        default:
            /* 0123 */
            rowaddr[0] = 0;
            rowaddr[1] = rows;
            rowaddr[2] = rows * 2;
            rowaddr[3] = rows * 3;
            break;
        }
        break;
    case MOD_8_8APSK:
        mod_order = 4;
        rows = frame_size / mod_order;
        switch (rate) {
        case C90_180:
            /* 3210 */
            rowaddr[0] = rows * 3;
            rowaddr[1] = rows * 2;
            rowaddr[2] = rows;
            rowaddr[3] = 0;
            break;
        case C96_180:
            /* 2310 */
            rowaddr[0] = rows * 2;
            rowaddr[1] = rows * 3;
            rowaddr[2] = rows;
            rowaddr[3] = 0;
            break;
        case C100_180:
            /* 2301 */
            rowaddr[0] = rows * 2;
            rowaddr[1] = rows * 3;
            rowaddr[2] = 0;
            rowaddr[3] = rows;
            break;
        default:
            /* 0123 */
            rowaddr[0] = 0;
            rowaddr[1] = rows;
            rowaddr[2] = rows * 2;
            rowaddr[3] = rows * 3;
            break;
        }
        break;
    case MOD_4_12_16APSK:
        mod_order = 5;
        rows = frame_size / mod_order;
        if (framesize == FECFRAME_NORMAL) {
            /* 21430 */
            rowaddr[0] = rows * 2;
            rowaddr[1] = rows;
            rowaddr[2] = rows * 4;
            rowaddr[3] = rows * 2;
            rowaddr[4] = 0;
        } else {
            if (rate == C2_3) {
                /* 41230 */
                rowaddr[0] = rows * 4;
                rowaddr[1] = rows;
                rowaddr[2] = rows * 2;
                rowaddr[3] = rows * 3;
                rowaddr[4] = 0;
            } else if (rate == C32_45) {
                /* 10423 */
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
        if (rate == C140_180) {
            /* 40213 */
            rowaddr[0] = rows * 4;
            rowaddr[1] = 0;
            rowaddr[2] = rows * 2;
            rowaddr[3] = rows;
            rowaddr[4] = rows * 3;
        } else {
            /* 40312 */
            rowaddr[0] = rows * 4;
            rowaddr[1] = 0;
            rowaddr[2] = rows * 3;
            rowaddr[3] = rows;
            rowaddr[4] = rows * 2;
        }
        break;
    default:
        mod_order = 2;
        break;
    }

    return constellation;
}

int interleaver_bb_impl::work(int noutput_items,
                              gr_vector_int& ninput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    if (ninput_items[0] < frame_size) {
        consume_each(0);
        return 0;
    }

    auto in = static_cast<const input_type*>(input_items[0]);
    auto out = static_cast<output_type*>(output_items[0]);

    int produced = 0;
    int consumed = 0;
    int rows = frame_size / mod_order;
    const input_type* c[5];

    switch (constellation) {
    case MOD_BPSK:
        for (int j = 0; j < rows; j++) {
            out[produced++] = in[consumed++];
        }
        break;
    case MOD_BPSK_SF2:
        for (int j = 0; j < rows; j++) {
            out[produced++] = in[consumed];
            out[produced++] = in[consumed++];
        }
        break;
    case MOD_8PSK:
    case MOD_8APSK:
        c[0] = &in[consumed + rowaddr[0]];
        c[1] = &in[consumed + rowaddr[1]];
        c[2] = &in[consumed + rowaddr[2]];
        for (int j = 0; j < rows; j++) {
            out[produced++] = (c[0][j] << 2) | (c[1][j] << 1) | (c[2][j]);
            consumed += 3;
        }
        break;
    case MOD_16APSK:
    case MOD_8_8APSK:
        c[0] = &in[consumed + rowaddr[0]];
        c[1] = &in[consumed + rowaddr[1]];
        c[2] = &in[consumed + rowaddr[2]];
        c[3] = &in[consumed + rowaddr[3]];
        for (int j = 0; j < rows; j++) {
            out[produced++] = (c[0][j] << 3) | (c[1][j] << 2) | (c[2][j] << 1) | (c[3][j]);
            consumed += 4;
        }
        break;
    case MOD_32APSK:
    case MOD_4_12_16APSK:
    case MOD_4_8_4_16APSK:
        c[0] = &in[consumed + rowaddr[0]];
        c[1] = &in[consumed + rowaddr[1]];
        c[2] = &in[consumed + rowaddr[2]];
        c[3] = &in[consumed + rowaddr[3]];
        c[4] = &in[consumed + rowaddr[4]];
        for (int j = 0; j < rows; j++) {
            out[produced++] =
                (c[0][j] << 4) | (c[1][j] << 3) | (c[2][j] << 2) | (c[3][j] << 1) | (c[4][j]);
            consumed += 5;
        }
        break;
    case MOD_QPSK:
    default:
        for (int j = 0; j < rows; j++) {
            out[produced] = in[consumed++] << 1;
            out[produced++] |= in[consumed++];
        }
        break;
    }

    add_item_tag(0, 0, pmt::intern("frame_length"), pmt::from_long((long)produced));

    consume_each(consumed);
    return produced;
}

} /* namespace dvbs2acm */
} /* namespace gr */
