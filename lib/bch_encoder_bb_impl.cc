/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2014,2016,2017,2020 Ron Economos.
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

#include "bch_code.h"
#include "bch_encoder_bb_impl.h"
#include <gnuradio/io_signature.h>
#include <pmt/pmt.h>
#include <vector>

namespace gr {
namespace dvbs2acm {
using input_type = unsigned char;
using output_type = unsigned char;
bch_encoder_bb::sptr bch_encoder_bb::make() { return gnuradio::make_block_sptr<bch_encoder_bb_impl>(); }


/*
 * The private constructor
 */
bch_encoder_bb_impl::bch_encoder_bb_impl()
    : gr::block("bch_encoder_bb",
                gr::io_signature::make(1, 1, sizeof(input_type)),
                gr::io_signature::make(1, 1, sizeof(output_type)))
{
}

/*
 * Our virtual destructor.
 */
bch_encoder_bb_impl::~bch_encoder_bb_impl() {}

void bch_encoder_bb_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    ninput_items_required[0] = noutput_items;
}

int bch_encoder_bb_impl::general_work(int noutput_items,
                                      gr_vector_int& ninput_items,
                                      gr_vector_const_void_star& input_items,
                                      gr_vector_void_star& output_items)
{
    auto in = static_cast<const input_type*>(input_items[0]);
    auto out = static_cast<output_type*>(output_items[0]);
    int consumed = 0;
    int produced = 0;

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
        const uint64_t tagoffset = this->nitems_written(0);
        pmt::pmt_t key = pmt::string_to_symbol("modcod");
        pmt::pmt_t value = pmt::from_uint64(tagmodcod);
        this->add_item_tag(0, tagoffset, key, value);
        // TODO: Some actual work???
        consumed += params.kbch;
        produced += params.nbch;
        produce(0, params.nbch);
    }
    consume_each(consumed);
    return WORK_CALLED_PRODUCE;
}

} /* namespace dvbs2acm */
} /* namespace gr */
