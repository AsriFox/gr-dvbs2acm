/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2018 Ahmet Inan, Ron Economos.
 *
 * This file is part of gr-dvbs2rx.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "bch_code.h"
#include "bch_decoder_bb_impl.h"
#include "debug_level.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/logger.h>
#include <functional>
#include <vector>

using namespace gr::dvbs2;

namespace gr {
namespace dvbs2acm {

using input_type = unsigned char;
using output_type = unsigned char;

bch_decoder_bb::sptr bch_decoder_bb::make(dvbs2_outputmode_t outputmode, int debug_level)
{
    return gnuradio::make_block_sptr<bch_decoder_bb_impl>(outputmode, debug_level);
}


/*
 * The private constructor
 */
bch_decoder_bb_impl::bch_decoder_bb_impl(dvbs2_outputmode_t outputmode, int debug_level)
    : gr::block("bch_decoder_bb",
                gr::io_signature::make(1, 1, sizeof(input_type)),
                gr::io_signature::make(1, 1, sizeof(output_type))),
      d_debug_level(debug_level),
      output_mode(outputmode),
      d_frame_cnt(0),
      d_frame_error_cnt(0)
{
    instance_n = new GF_NORMAL();
    instance_m = new GF_MEDIUM();
    instance_s = new GF_SHORT();
    decode_n_12 = new BCH_NORMAL_12();
    decode_n_10 = new BCH_NORMAL_10();
    decode_n_8 = new BCH_NORMAL_8();
    decode_s_12 = new BCH_SHORT_12();
    decode_m_12 = new BCH_MEDIUM_12();
    code = new uint8_t[8192];
    memset(code, 0, 8192);
    parity = new uint8_t[24];
    memset(parity, 0, 24);
    // if (output_mode == OM_MESSAGE) {
    //     set_output_multiple(kbch);
    //     set_relative_rate((double)kbch / nbch);
    // } else {
    //     set_output_multiple(nbch);
    // }
    // TODO: What is OM_MESSAGE?
}

/*
 * Our virtual destructor.
 */
bch_decoder_bb_impl::~bch_decoder_bb_impl()
{
    delete[] parity;
    delete[] code;
    delete decode_s_12;
    delete decode_m_12;
    delete decode_n_8;
    delete decode_n_10;
    delete decode_n_12;
    delete instance_s;
    delete instance_m;
    delete instance_n;
}

void bch_decoder_bb_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    // if (output_mode == OM_MESSAGE) {
    //     ninput_items_required[0] = (noutput_items / kbch) * nbch;
    // } else {
    ninput_items_required[0] = noutput_items;
    // }
}

int bch_decoder_bb_impl::general_work(int noutput_items,
                                      gr_vector_int& ninput_items,
                                      gr_vector_const_void_star& input_items,
                                      gr_vector_void_star& output_items)
{
    auto in = static_cast<const input_type*>(input_items[0]);
    auto out = static_cast<output_type*>(output_items[0]);

    int corrections = 0;
    int consumed = 0;
    int produced = 0;
    int produced_per_iteration;

    std::vector<tag_t> tags;
    const uint64_t nread = this->nitems_read(0); // number of items read on port 0

    // Read all tags on the input buffer
    this->get_tags_in_range(tags, 0, nread, nread + noutput_items, pmt::string_to_symbol("modcod"));

    for (tag_t tag : tags) {
        auto tagmodcod = pmt::to_uint64(tag.value);
        auto framesize = (dvbs2_framesize_t)((tagmodcod >> 1) & 0x7f);
        auto rate = (dvbs2_code_rate_t)((tagmodcod >> 8) & 0xff);
        auto params = bch_code::select(framesize, rate);
        if (params.nbch + produced > (unsigned int)noutput_items) {
            break;
        }
        produced_per_iteration = 0;
        const uint64_t tagoffset = this->nitems_written(0);
        pmt::pmt_t key = pmt::string_to_symbol("modcod");
        pmt::pmt_t value = pmt::from_uint64(tagmodcod);
        this->add_item_tag(0, tagoffset, key, value);

        auto kbch = params.kbch;
        auto nbch = params.nbch;
        unsigned int output_size = output_mode ? kbch : nbch;
        for (unsigned int j = 0; j < kbch; j++) {
            CODE::set_be_bit(code, j, *in++);
        }
        for (unsigned int j = 0; j < nbch - kbch; j++) {
            CODE::set_be_bit(parity, j, *in++);
        }
        switch (params.code) {
        case BCH_CODE_N12:
            corrections = (*decode_n_12)(code, parity, 0, 0, kbch);
            break;
        case BCH_CODE_N10:
            corrections = (*decode_n_10)(code, parity, 0, 0, kbch);
            break;
        case BCH_CODE_N8:
            corrections = (*decode_n_8)(code, parity, 0, 0, kbch);
            break;
        case BCH_CODE_S12:
            corrections = (*decode_s_12)(code, parity, 0, 0, kbch);
            break;
        case BCH_CODE_M12:
            corrections = (*decode_m_12)(code, parity, 0, 0, kbch);
            break;
        }
        if (corrections > 0) {
            GR_LOG_DEBUG_LEVEL(
                1, "frame = {:d}, BCH decoder corrections = {:d}", d_frame_cnt, corrections);
        } else if (corrections == -1) {
            d_frame_error_cnt++;
            GR_LOG_DEBUG_LEVEL(1,
                               "frame = {:d}, BCH decoder too many bit errors (FER = {:g})",
                               d_frame_cnt,
                               ((double)d_frame_error_cnt / (d_frame_cnt + 1)));
        }
        d_frame_cnt++;
        for (unsigned int j = 0; j < kbch; j++) {
            *out++ = CODE::get_be_bit(code, j);
        }
        if (output_size == nbch) {
            for (unsigned int j = 0; j < nbch - kbch; j++) {
                *out++ = CODE::get_be_bit(parity, j);
            }
        }
        consumed += nbch;
        produce(0, produced_per_iteration);
    }
    consume_each(consumed);
    return produced;
}

} /* namespace dvbs2acm */
} /* namespace gr */
