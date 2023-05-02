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

#include "bbscrambler_bb_impl.h"
#include "bch_code.h"
#include <gnuradio/io_signature.h>
#include <pmt/pmt.h>

namespace gr {
namespace dvbs2acm {
using input_type = unsigned char;
using output_type = unsigned char;
bbscrambler_bb::sptr bbscrambler_bb::make() { return gnuradio::make_block_sptr<bbscrambler_bb_impl>(); }


/*
 * The private constructor
 */
bbscrambler_bb_impl::bbscrambler_bb_impl()
    : gr::sync_block("bbscrambler_bb",
                     gr::io_signature::make(1, 1, sizeof(input_type)),
                     gr::io_signature::make(1, 1, sizeof(output_type)))
{
    init_bb_randomizer();
    set_output_multiple(FRAME_SIZE_NORMAL);
}

/*
 * Our virtual destructor.
 */
bbscrambler_bb_impl::~bbscrambler_bb_impl() {}

void bbscrambler_bb_impl::init_bb_randomizer(void)
{
    int sr = 0x4A80;
    for (int i = 0; i < FRAME_SIZE_NORMAL; i++) {
        int b = ((sr) ^ (sr >> 1)) & 1;
        bb_randomize[i] = b;
        sr >>= 1;
        if (b) {
            sr |= 0x4000;
        }
    }
}

int bbscrambler_bb_impl::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    auto in = static_cast<const input_type*>(input_items[0]);
    auto out = static_cast<output_type*>(output_items[0]);
    unsigned int kbch;
    unsigned int produced = 0;
    std::vector<tag_t> tags;
    const uint64_t nread = this->nitems_read(0);

    // Read all tags on the input buffer
    this->get_tags_in_range(tags, 0, nread, nread + noutput_items, pmt::string_to_symbol("modcod"));

    for (tag_t tag : tags) {
        auto tagmodcod = pmt::to_uint64(tag.value);
        auto framesize = (dvbs2_framesize_t)((tagmodcod >> 1) & 0x7f);
        auto rate = (dvbs2_code_rate_t)((tagmodcod >> 8) & 0xff);
        kbch = 0;
        switch (framesize) {
        case FECFRAME_NORMAL:
            kbch = bch_code::select_normal(rate).kbch;
            break;
        case FECFRAME_SHORT:
            kbch = bch_code::select_short(rate).kbch;
            break;
        default:
            break;
        }
        if (kbch + produced <= (unsigned int)noutput_items) {
            for (int j = 0; j < (int)kbch; j++) {
                out[produced + j] = in[produced + j] ^ bb_randomize[j];
            }
            produced += kbch;
        } else {
            break;
        }
    }
    return produced;
}

} /* namespace dvbs2acm */
} /* namespace gr */
