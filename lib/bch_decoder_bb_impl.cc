/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2018 Ahmet Inan, Ron Economos.
 *
 * This file is part of gr-dvbs2rx.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "bch_decoder_bb_impl.h"
#include "debug_level.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/logger.h>
#include <functional>
#include <vector>

namespace gr {
namespace dvbs2acm {

using input_type = unsigned char;
using output_type = unsigned char;

bch_decoder_bb::sptr bch_decoder_bb::make(int debug_level)
{
    return gnuradio::make_block_sptr<bch_decoder_bb_impl>(debug_level);
}


/*
 * The private constructor
 */
bch_decoder_bb_impl::bch_decoder_bb_impl(int debug_level)
    : gr::block("bch_decoder_bb",
                gr::io_signature::make(1, 1, sizeof(input_type)),
                gr::io_signature::make(1, 1, sizeof(output_type))),
      d_debug_level(debug_level),
      d_frame_cnt(0),
      d_frame_error_cnt(0),
      params(bch_code::bch_code_invalid),
      instance_n(new GF_NORMAL()),
      instance_m(new GF_MEDIUM()),
      instance_s(new GF_SHORT()),
      decode_n_12(new BCH_NORMAL_12()),
      decode_n_10(new BCH_NORMAL_10()),
      decode_n_8(new BCH_NORMAL_8()),
      decode_m_12(new BCH_MEDIUM_12()),
      decode_s_12(new BCH_SHORT_12())
{
    code.fill(0);
    parity.fill(0);
    set_tag_propagation_policy(TPP_CUSTOM);
    set_output_multiple(FRAME_SIZE_NORMAL);
}

/*
 * Our virtual destructor.
 */
bch_decoder_bb_impl::~bch_decoder_bb_impl() {}

void bch_decoder_bb_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    if (params.kbch != 0) {
        ninput_items_required[0] = (noutput_items / params.kbch) * params.nbch;
    } else {
        ninput_items_required[0] = noutput_items;
    }
}

int bch_decoder_bb_impl::general_work(int noutput_items,
                                      gr_vector_int& ninput_items,
                                      gr_vector_const_void_star& input_items,
                                      gr_vector_void_star& output_items)
{
    auto in = static_cast<const input_type*>(input_items[0]);
    auto out = static_cast<output_type*>(output_items[0]);
    int consumed_total = 0;
    int produced_total = 0;
    dvbs2_modcod_t modcod = MC_DUMMY;
    dvbs2_vlsnr_header_t vlsnr_header = VLSNR_DUMMY;

    std::vector<tag_t> tags;
    const uint64_t nread = this->nitems_read(0); // number of items read on port 0

    // Read all tags on the input buffer
    this->get_tags_in_range(tags, 0, nread, nread + ninput_items[0], pmt::string_to_symbol("pls"));

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
        auto params = bch_code::select(modcod, vlsnr_header);
        if (this->params != params) {
            this->params = params;
            set_relative_rate(params.kbch, params.nbch);
            set_output_multiple(params.kbch);
        }
        if (this->params.kbch + produced_total > (unsigned int)noutput_items) {
            break;
        }

        auto abs_offset = nitems_written(0);
        add_item_tag(0, abs_offset, tag.key, tag.value);

        int consumed, produced;
        decode_bch(in, out, consumed, produced);

        consumed_total += consumed;
        produced_total += produced;
        produce(0, produced);
    }
    consume_each(consumed_total);
    return WORK_CALLED_PRODUCE;
}

void bch_decoder_bb_impl::decode_bch(const unsigned char* in,
                                     unsigned char* out,
                                     int& consumed,
                                     int& produced)
{
    auto kbch = params.kbch;
    auto nbch = params.nbch;
    int corrections = 0;
    for (unsigned int j = 0; j < kbch; j++) {
        CODE::set_be_bit(code.data(), j, *in++);
    }
    for (unsigned int j = 0; j < nbch - kbch; j++) {
        CODE::set_be_bit(parity.data(), j, *in++);
    }
    switch (params.code) {
    case BCH_CODE_N12:
        corrections = decode_n_12->decode(code.data(), parity.data(), 0, 0, kbch);
        break;
    case BCH_CODE_N10:
        corrections = decode_n_10->decode(code.data(), parity.data(), 0, 0, kbch);
        break;
    case BCH_CODE_N8:
        corrections = decode_n_8->decode(code.data(), parity.data(), 0, 0, kbch);
        break;
    case BCH_CODE_S12:
        corrections = decode_s_12->decode(code.data(), parity.data(), 0, 0, kbch);
        break;
    case BCH_CODE_M12:
        corrections = decode_m_12->decode(code.data(), parity.data(), 0, 0, kbch);
        break;
    }
    if (corrections > 0) {
        d_logger->debug("frame = {:d}, BCH decoder corrections = {:d}", d_frame_cnt, corrections);
    } else if (corrections == -1) {
        d_frame_error_cnt++;
        d_logger->debug("frame = {:d}, BCH decoder too many bit errors (FER = {:g})",
                        d_frame_cnt,
                        ((double)d_frame_error_cnt / (d_frame_cnt + 1)));
    }
    d_frame_cnt++;
    for (unsigned int j = 0; j < kbch; j++) {
        *out++ = CODE::get_be_bit(code.data(), j);
    }
    consumed = nbch;
    produced = kbch;
    // if (output_size == nbch) {
    //     for (unsigned int j = 0; j < nbch - kbch; j++) {
    //         *out++ = CODE::get_be_bit(parity, j);
    //     }
    // }
}

} /* namespace dvbs2acm */
} /* namespace gr */
