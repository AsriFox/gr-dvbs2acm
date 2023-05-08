/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2014,2016,2017,2020 Ron Economos.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "bch_encoder_bb_impl.h"
#include <gnuradio/io_signature.h>
#include <pmt/pmt.h>

namespace gr {
namespace dvbs2acm {
using input_type = unsigned char;
using output_type = unsigned char;
bch_encoder_bb::sptr bch_encoder_bb::make() { return gnuradio::make_block_sptr<bch_encoder_bb_impl>(); }


/*
 * The private constructor
 */
bch_encoder_bb_impl::bch_encoder_bb_impl()
    : gr::tagged_stream_block("bch_encoder_bb",
                              gr::io_signature::make(1, 1, sizeof(input_type)),
                              gr::io_signature::make(1, 1, sizeof(output_type)),
                              "frame_length"),
      params(bch_code::bch_code_invalid)
{
    set_tag_propagation_policy(TPP_ALL_TO_ALL);
}

/*
 * Our virtual destructor.
 */
bch_encoder_bb_impl::~bch_encoder_bb_impl() {}

void bch_encoder_bb_impl::parse_length_tags(const std::vector<std::vector<tag_t>>& tags,
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
    this->params = bch_code::select(modcod, vlsnr_header);
    if (n_input_items_reqd[0] != (int)params.kbch) {
        n_input_items_reqd[0] = (int)params.kbch;
    }
}

int bch_encoder_bb_impl::work(int noutput_items,
                              gr_vector_int& ninput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    if (ninput_items[0] < (int)params.kbch) {
        consume_each(0);
        return 0;
    }

    auto in = static_cast<const input_type*>(input_items[0]);
    auto out = static_cast<output_type*>(output_items[0]);

    add_item_tag(0, 0, pmt::intern("frame_length"), pmt::from_long((long)params.nbch));

    params.encode(in, out);

    consume_each(params.kbch);
    return params.nbch;
}

} /* namespace dvbs2acm */
} /* namespace gr */
