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
                     gr::io_signature::make(1, 1, sizeof(output_type))),
      kbch(FRAME_SIZE_NORMAL)
{
    init_bb_derandomiser();
    set_output_multiple(kbch);
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
        auto kbch = bch_code::select(modcod, vlsnr_header).kbch;
        if (this->kbch != kbch) {
            this->kbch = kbch;
            set_output_multiple(kbch);
        }

        if (kbch + produced_total > (unsigned int)noutput_items) {
            break;
        }

        for (int j = 0; j < (int)kbch; j++) {
            *out++ = *in++ ^ bb_derandomize[j];
        }
        produced_total += kbch;
    }
    return produced_total;
}

} /* namespace dvbs2acm */
} /* namespace gr */
