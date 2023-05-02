/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "bbdescrambler_bb_impl.h"
#include "bch_code.h"
#include <gnuradio/io_signature.h>
#include <vector>

namespace gr {
namespace dvbs2acm {

using input_type = unsigned char;
using output_type = unsigned char;

bbdescrambler_bb::sptr bbdescrambler_bb::make()
{
    return gnuradio::make_block_sptr<bbdescrambler_bb_impl>();
}


/*
 * The private constructor
 */
bbdescrambler_bb_impl::bbdescrambler_bb_impl()
    : gr::sync_block("bbdescrambler_bb",
                     gr::io_signature::make(1, 1, sizeof(input_type)),
                     gr::io_signature::make(1, 1, sizeof(output_type)))
{
    init_bb_derandomiser();
    // set_output_multiple(kbch);
}

void bbdescrambler_bb_impl::init_bb_derandomiser()
{
    int sr = 0x4A80;
    for (int i = 0; i < FRAME_SIZE_NORMAL; i++) {
        int b = ((sr) ^ (sr >> 1)) & 1;
        bb_derandomize[i] = b;
        sr >>= 1;
        if (b) {
            sr |= 0x4000;
        }
    }
}

/*
 * Our virtual destructor.
 */
bbdescrambler_bb_impl::~bbdescrambler_bb_impl() {}

int bbdescrambler_bb_impl::work(int noutput_items,
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
                out[produced + j] = in[produced + j] ^ bb_derandomize[j];
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
