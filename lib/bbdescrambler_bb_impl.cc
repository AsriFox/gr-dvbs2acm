/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "bbdescrambler_bb_impl.h"
#include "bch_code.h"
#include "frame_stream.hh"
#include <gnuradio/io_signature.h>
#include <vector>

namespace gr {
namespace dvbs2acm {

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
                     gr::io_signature::make(1, 1, sizeof(output_type))),
      kbch(FRAME_SIZE_NORMAL)
{
    init_bb_derandomiser();
    // set_output_multiple(kbch);
    set_output_multiple(FRAME_SIZE_NORMAL);
    set_tag_propagation_policy(TPP_CUSTOM);
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
    int produced_total = 0;
    dvbs2_modcod_t modcod = MC_DUMMY;
    dvbs2_vlsnr_header_t vlsnr_header = VLSNR_DUMMY;

    std::vector<tag_t> tags;
    const uint64_t nread = this->nitems_read(0);

    // Read all tags on the input buffer
    this->get_tags_in_range(tags, 0, nread, nread + noutput_items, pmt::string_to_symbol("pls"));

    for (tag_t tag : tags) {
        FRAMESTREAM_HANDLE_TAG
        auto kbch = bch_code::select(modcod, vlsnr_header).kbch;
        // if (this->kbch != kbch) {
        //     this->kbch = kbch;
        //     set_output_multiple(kbch);
        // }

        if (kbch + produced_total > (unsigned int)noutput_items) {
            break;
        }

        FRAMESTREAM_PROPAGATE_TAG

        for (unsigned j = 0; j < kbch; j++) {
            out[produced_total + j] = in[produced_total + j] ^ bb_derandomize[j];
        }
        produced_total += kbch;
    }
    return produced_total;
}

} /* namespace dvbs2acm */
} /* namespace gr */
