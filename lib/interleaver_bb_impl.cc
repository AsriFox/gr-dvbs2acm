/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2014,2016 Ron Economos.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include "interleaver_bb_impl.h"
#include "modcod.hh"
#include <gnuradio/io_signature.h>
#include <pmt/pmt.h>
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
    : gr::block("interleaver_bb",
                gr::io_signature::make(1, 1, sizeof(input_type)),
                gr::io_signature::make(1, 1, sizeof(output_type)))
{
    set_tag_propagation_policy(TPP_DONT);
    set_output_multiple(FRAME_SIZE_NORMAL);
}

/*
 * Our virtual destructor.
 */
interleaver_bb_impl::~interleaver_bb_impl() {}

void interleaver_bb_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    ninput_items_required[0] = noutput_items * 5;
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

int interleaver_bb_impl::general_work(int noutput_items,
                                      gr_vector_int& ninput_items,
                                      gr_vector_const_void_star& input_items,
                                      gr_vector_void_star& output_items)
{
    auto in = static_cast<const input_type*>(input_items[0]);
    auto out = static_cast<output_type*>(output_items[0]);
    int consumed = 0;
    int produced = 0;
    int produced_per_iteration;
    int frame_size, mod_order, rows;
    const input_type* c[5];

    std::vector<tag_t> tags;
    const uint64_t nread = this->nitems_read(0);

    // Read all tags on the input buffer
    this->get_tags_in_range(tags, 0, nread, nread + noutput_items, pmt::string_to_symbol("modcod"));

    for (tag_t tag : tags) {
        const uint64_t tagmodcod = pmt::to_uint64(tag.value);
        auto modcod = (dvbs2_modcod_t)((tagmodcod >> 2) & 0x7f);
        auto vlsnr_header = (dvbs2_vlsnr_header_t)((tagmodcod >> 9) & 0x0f);
        auto constellation = get_rows(modcod, vlsnr_header, frame_size, mod_order);
        rows = frame_size / mod_order;
        if (produced + rows > noutput_items) {
            break;
        }
        const uint64_t tagoffset = this->nitems_written(0);
        this->add_item_tag(0, tagoffset, pmt::string_to_symbol("modcod"), pmt::from_uint64(tagmodcod));

        produced_per_iteration = 0;
        switch (constellation) {
        case MOD_BPSK:
            for (int j = 0; j < rows; j++) {
                out[produced++] = in[consumed++];
                produced_per_iteration++;
            }
            break;
        case MOD_BPSK_SF2:
            for (int j = 0; j < rows; j++) {
                out[produced++] = in[consumed];
                out[produced++] = in[consumed++];
                produced_per_iteration += 2;
            }
            break;
        case MOD_8PSK:
        case MOD_8APSK:
            c[0] = &in[consumed + rowaddr[0]];
            c[1] = &in[consumed + rowaddr[1]];
            c[2] = &in[consumed + rowaddr[2]];
            for (int j = 0; j < rows; j++) {
                out[produced++] = (c[0][j] << 2) | (c[1][j] << 1) | (c[2][j]);
                produced_per_iteration++;
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
                produced_per_iteration++;
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
                produced_per_iteration++;
                consumed += 5;
            }
            break;
        case MOD_QPSK:
        default:
            for (int j = 0; j < rows; j++) {
                out[produced] = in[consumed++] << 1;
                out[produced++] |= in[consumed++];
                produced_per_iteration++;
            }
            break;
        }
        produce(0, produced_per_iteration);
    }
    consume_each(consumed);
    return WORK_CALLED_PRODUCE;
}

} /* namespace dvbs2acm */
} /* namespace gr */
