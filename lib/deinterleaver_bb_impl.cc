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

using input_type = int8_t;
using output_type = int8_t;

deinterleaver_bb::sptr deinterleaver_bb::make()
{
    return gnuradio::make_block_sptr<deinterleaver_bb_impl>();
}


/*
 * The private constructor
 */
deinterleaver_bb_impl::deinterleaver_bb_impl()
    : gr::block("deinterleaver_bb",
                gr::io_signature::make(1, 1, sizeof(input_type)),
                gr::io_signature::make(1, 1, sizeof(output_type)))
{
    set_output_multiple(FRAME_SIZE_NORMAL);
    set_tag_propagation_policy(TPP_CUSTOM);
}

/*
 * Our virtual destructor.
 */
deinterleaver_bb_impl::~deinterleaver_bb_impl() {}

void deinterleaver_bb_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    ninput_items_required[0] = noutput_items;
}

int deinterleaver_bb_impl::general_work(int noutput_items,
                                        gr_vector_int& ninput_items,
                                        gr_vector_const_void_star& input_items,
                                        gr_vector_void_star& output_items)
{
    auto in = static_cast<const input_type*>(input_items[0]);
    auto out = static_cast<output_type*>(output_items[0]);
    int consumed_total = 0;
    dvbs2_modcod_t modcod = MC_DUMMY;
    dvbs2_vlsnr_header_t vlsnr_header = VLSNR_DUMMY;
    std::vector<tag_t> tags;
    auto abs_start = nitems_read(0);
    get_tags_in_range(tags, 0, abs_start, abs_start + ninput_items[0]);
    for (tag_t tag : tags) {
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

        auto abs_offset = nitems_written(0);
        add_item_tag(0, abs_offset, tag.key, tag.value);

        int consumed, produced;
        deinterleave(in, out, consumed, produced);

        consumed_total += consumed;
        produce(0, produced);
    }
    consume_each(consumed_total);
    return WORK_CALLED_PRODUCE;
}

void deinterleaver_bb_impl::get_rows(dvbs2_framesize_t framesize,
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

void deinterleaver_bb_impl::deinterleave(const int8_t* in, int8_t* out, int& consumed, int& produced)
{
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
        memcpy(out, in, sizeof(input_type) * frame_size);
        in += frame_size;
        break;
    }
    out += frame_size;
    consumed = produced = frame_size;
}

} /* namespace dvbs2acm */
} /* namespace gr */
