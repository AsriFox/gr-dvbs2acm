/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2014,2016 Ron Economos.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "bbscrambler_bb_impl.h"
#include <gnuradio/io_signature.h>

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

unsigned int bbscrambler_bb_impl::get_kbch(dvbs2_framesize_t framesize, dvbs2_code_rate_t rate)
{
    if (framesize == FECFRAME_NORMAL) {
        switch (rate) {
        case C1_4:
            return 16008;
        case C1_3:
            return 21408;
        case C2_5:
            return 25728;
        case C1_2:
            return 32208;
        case C3_5:
            return 38688;
        case C2_3:
            return 43040;
        case C3_4:
            return 48408;
        case C4_5:
            return 51648;
        case C5_6:
            return 53840;
        case C8_9:
            return 57472;
        case C9_10:
            return 58192;
        case C2_9_VLSNR:
            return 14208;
        case C13_45:
            return 18528;
        case C9_20:
            return 28968;
        case C90_180:
            return 32208;
        case C96_180:
            return 34368;
        case C11_20:
            return 35448;
        case C100_180:
            return 35808;
        case C104_180:
            return 37248;
        case C26_45:
            return 37248;
        case C18_30:
            return 38688;
        case C28_45:
            return 40128;
        case C23_36:
            return 41208;
        case C116_180:
            return 41568;
        case C20_30:
            return 43008;
        case C124_180:
            return 44448;
        case C25_36:
            return 44808;
        case C128_180:
            return 45888;
        case C13_18:
            return 46608;
        case C132_180:
            return 47328;
        case C22_30:
            return 47328;
        case C135_180:
            return 48408;
        case C140_180:
            return 50208;
        case C7_9:
            return 50208;
        case C154_180:
            return 55248;
        default:
            return 0;
        }
    } else if (framesize == FECFRAME_SHORT) {
        switch (rate) {
        case C1_4:
            return 3072;
        case C1_3:
            return 5232;
        case C2_5:
            return 6312;
        case C1_2:
            return 7032;
        case C3_5:
            return 9552;
        case C2_3:
            return 10632;
        case C3_4:
            return 11712;
        case C4_5:
            return 12432;
        case C5_6:
            return 13152;
        case C8_9:
            return 14232;
        case C11_45:
            return 3792;
        case C4_15:
            return 4152;
        case C14_45:
            return 4872;
        case C7_15:
            return 7392;
        case C8_15:
            return 8472;
        case C26_45:
            return 9192;
        case C32_45:
            return 11352;
        case C1_5_VLSNR_SF2:
            return 2512;
        case C11_45_VLSNR_SF2:
            return 3792;
        case C1_5_VLSNR:
            return 3072;
        case C4_15_VLSNR:
            return 4152;
        case C1_3_VLSNR:
            return 5232;
        default:
            return 0;
        }
    } else {
        switch (rate) {
        case C1_5_MEDIUM:
            return 5660;
        case C11_45_MEDIUM:
            return 7740;
        case C1_3_MEDIUM:
            return 10620;
        default:
            return 0;
        }
    }
}

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
        kbch = get_kbch(framesize, rate);
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
