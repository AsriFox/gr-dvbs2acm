/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "bbscrambler_bb_impl.h"
#include "bch_code.h"
#include <gnuradio/io_signature.h>
#include <vector>

namespace gr {
namespace dvbs2acm {

using input_type = unsigned char;
using output_type = unsigned char;

bbscrambler_bb::sptr bbscrambler_bb::make() { return gnuradio::make_block_sptr<bbscrambler_bb_impl>(); }


/*
 * The private constructor
 */
bbscrambler_bb_impl::bbscrambler_bb_impl()
    : gr::tagged_stream_block("bbscrambler_bb",
                              gr::io_signature::make(1, 1, sizeof(input_type)),
                              gr::io_signature::make(1, 1, sizeof(output_type)),
                              "frame_length")
{
    set_tag_propagation_policy(TPP_ALL_TO_ALL);
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

void bbscrambler_bb_impl::parse_length_tags(const std::vector<std::vector<tag_t>>& tags,
                                            gr_vector_int& n_input_items_reqd)
{
    n_input_items_reqd[0] = 0;
    for (tag_t tag : tags[0]) {
        if (tag.key == pmt::intern(d_length_tag_key_str)) {
            n_input_items_reqd[0] = pmt::to_long(tag.value);
        }
    }
}

int bbscrambler_bb_impl::work(int noutput_items,
                              gr_vector_int& ninput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    unsigned kbch = ninput_items[0];
    auto in = static_cast<const input_type*>(input_items[0]);
    auto out = static_cast<output_type*>(output_items[0]);

    // dvbs2_modcod_t modcod;
    // dvbs2_vlsnr_header_t vlsnr_header;
    // std::vector<tag_t> tags;
    // get_tags_in_window(tags, 0, 0, 1);
    // for (tag_t tag: tags) {
    //     if (tag.key == pmt::intern("modcod")) {
    //         modcod = (dvbs2_modcod_t)(pmt::to_long(tag.value) & 0x7f);
    //     } else if (tag.key == pmt::intern("vlsnr_header")) {
    //         vlsnr_header = (dvbs2_vlsnr_header_t)(pmt::to_long(tag.value) & 0x0f);
    //     }
    //     add_item_tag(0, tag);
    // }
    // auto kbch = bch_code::select(modcod, vlsnr_header).kbch;

    for (unsigned j = 0; j < kbch; j++) {
        out[j] = in[j] ^ bb_randomize[j];
    }

    return kbch;
}

} /* namespace dvbs2acm */
} /* namespace gr */
