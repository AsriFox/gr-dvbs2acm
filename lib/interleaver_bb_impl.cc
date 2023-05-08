/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2014,2016 Ron Economos.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "interleaver_bb_impl.h"
#include <gnuradio/io_signature.h>

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

void interleaver_bb_impl::get_rows(dvbs2_framesize_t framesize,
                                   dvbs2_code_rate_t rate,
                                   dvbs2_constellation_t constellation,
                                   int& frame_size,
                                   int& mod_order)
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
    dvbs2_framesize_t framesize;
    dvbs2_code_rate_t rate;
    dvbs2_constellation_t constellation;
    int frame_size, mod_order, rows;
    const input_type* c[5];

    std::vector<tag_t> tags;
    const uint64_t nread = this->nitems_read(0);

    // Read all tags on the input buffer
    this->get_tags_in_range(tags, 0, nread, nread + noutput_items, pmt::string_to_symbol("modcod"));

    for (tag_t tag : tags) {
        auto tagmodcod = pmt::to_uint64(tag.value);
        framesize = (dvbs2_framesize_t)((tagmodcod >> 1) & 0x7f);
        rate = (dvbs2_code_rate_t)((tagmodcod >> 8) & 0xff);
        constellation = (dvbs2_constellation_t)((tagmodcod >> 16) & 0xff);
        get_rows(framesize, rate, constellation, frame_size, mod_order);

        if (produced + rows > noutput_items) {
            break;
        }
        const uint64_t tagoffset = this->nitems_written(0);
        this->add_item_tag(0, tagoffset, pmt::string_to_symbol("modcod"), pmt::from_uint64(tagmodcod));

        produced_per_iteration = 0;
        switch (constellation) {
        case MOD_BPSK:
            rows = frame_size;
            for (int j = 0; j < rows; j++) {
                out[produced++] = in[consumed++];
                produced_per_iteration++;
            }
            break;
        case MOD_BPSK_SF2:
            rows = frame_size;
            for (int j = 0; j < rows; j++) {
                out[produced++] = in[consumed];
                out[produced++] = in[consumed++];
                produced_per_iteration += 2;
            }
            break;
        case MOD_QPSK:
            rows = frame_size / 2;
            for (int j = 0; j < rows; j++) {
                out[produced] = in[consumed++] << 1;
                out[produced++] |= in[consumed++];
                produced_per_iteration++;
            }
            break;
        case MOD_8PSK:
        case MOD_8APSK:
            rows = frame_size / 3;
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
            rows = frame_size / 4;
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
            rows = frame_size / 5;
            c[0] = &in[consumed + rowaddr[0]];
            c[1] = &in[consumed + rowaddr[1]];
            c[2] = &in[consumed + rowaddr[2]];
            c[3] = &in[consumed + rowaddr[3]];
            c[4] = &in[consumed + rowaddr[4]];
            for (int j = 0; j < rows; j++) {
                out[produced++] =
                    (c[0][j] << 4) | (c[1][j] << 3) | (c[2][j] << 2) | (c[3][j] << 1) | c[4][j];
                produced_per_iteration++;
                consumed += 5;
            }
            break;
        default:
            rows = frame_size / 2;
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
