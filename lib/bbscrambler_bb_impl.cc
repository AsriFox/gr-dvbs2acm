/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "bbscrambler_bb_impl.h"
#include "bch_code.h"
#include <gnuradio/io_signature.h>
#include <pmt/pmt.h>
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
    set_min_output_buffer(FRAME_SIZE_NORMAL * 6);
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
    // dvbs2_modcod_t modcod;
    // dvbs2_vlsnr_header_t vlsnr_header;
    dvbs2_framesize_t framesize;
    dvbs2_code_rate_t code_rate;
    for (tag_t tag : tags[0]) {
        if (tag.key == pmt::intern("modcod")) {
            auto tagmodcod = pmt::to_uint64(tag.value);
            framesize = (dvbs2_framesize_t)((tagmodcod >> 1) & 0x1);
            code_rate = (dvbs2_code_rate_t)((tagmodcod >> 8) & 0xff);
            // modcod = (dvbs2_modcod_t)((tagmodcod >> 25) & 0x7f);
            // vlsnr_header = (dvbs2_vlsnr_header_t)((tagmodcod >> 4) & 0x0f);
        }
    }
    // n_input_items_reqd[0] = bch_code::select(modcod, vlsnr_header).kbch;
    if (framesize == FECFRAME_NORMAL) {
        n_input_items_reqd[0] = bch_code::select_normal(code_rate).kbch;
    } else {
        n_input_items_reqd[0] = bch_code::select_short(code_rate).kbch;
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

    add_item_tag(0, 0, pmt::intern("frame_length"), pmt::from_long((long)kbch));

    for (unsigned j = 0; j < kbch; j++) {
        out[j] = in[j] ^ bb_randomize[j];
    }

    return kbch;
}

} /* namespace dvbs2acm */
} /* namespace gr */
