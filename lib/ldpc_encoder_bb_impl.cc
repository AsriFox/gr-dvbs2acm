/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2014,2016,2017,2020 Ron Economos.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ldpc_encoder_bb_impl.h"
#include "modcod.hh"
#include <gnuradio/io_signature.h>

namespace gr {
namespace dvbs2acm {

using input_type = unsigned char;
using output_type = unsigned char;

ldpc_encoder_bb::sptr ldpc_encoder_bb::make()
{
    return gnuradio::make_block_sptr<ldpc_encoder_bb_impl>();
}


/*
 * The private constructor
 */
ldpc_encoder_bb_impl::ldpc_encoder_bb_impl()
    : gr::tagged_stream_block("ldpc_bb",
                              gr::io_signature::make(1, 1, sizeof(unsigned char)),
                              gr::io_signature::make(1, 1, sizeof(unsigned char)),
                              "frame_length"),
      ldpc_tab(ldpc_encode_table::ldpc_tab_invalid)
{
    set_min_output_buffer((FRAME_SIZE_NORMAL)*6);
    set_tag_propagation_policy(TPP_DONT);
    set_output_multiple(FRAME_SIZE_NORMAL);
}

/*
 * Our virtual destructor.
 */
ldpc_encoder_bb_impl::~ldpc_encoder_bb_impl() {}

void ldpc_encoder_bb_impl::parse_length_tags(const std::vector<std::vector<tag_t>>& tags,
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
    this->ldpc_tab = ldpc_encode_table::select(modcod, vlsnr_header);
    if (n_input_items_reqd[0] != (int)ldpc_tab.nbch) {
        n_input_items_reqd[0] = (int)ldpc_tab.nbch;
    }
}

int ldpc_encoder_bb_impl::work(int noutput_items,
                               gr_vector_int& ninput_items,
                               gr_vector_const_void_star& input_items,
                               gr_vector_void_star& output_items)
{
    if (ninput_items[0] < (int)ldpc_tab.nbch) {
        consume_each(0);
        return 0;
    }

    auto in = static_cast<const input_type*>(input_items[0]);
    auto out = static_cast<output_type*>(output_items[0]);

    const output_type* d = in;
    output_type *p = out, *b = out, *s;
    int plen, puncture, index;

    add_item_tag(0, 0, pmt::intern("frame_length"), pmt::from_long((long)ldpc_tab.frame_size));

    plen = (ldpc_tab.frame_size_real + ldpc_tab.Xp) - ldpc_tab.nbch;
    p += ldpc_tab.nbch;
    if (ldpc_tab.Xs != 0) {
        s = &shortening_buffer[0];
        memset(s, 0, sizeof(output_type) * ldpc_tab.Xs);
        memcpy(&s[ldpc_tab.Xs], in, sizeof(input_type) * ldpc_tab.nbch);
        d = s;
    }
    if (ldpc_tab.P != 0) {
        p = &puncturing_buffer[ldpc_tab.nbch];
        b = &out[ldpc_tab.nbch];
    }
    // First zero all the parity bits
    memset(p, 0, sizeof(output_type) * plen);
    memcpy(out, in, sizeof(input_type) * ldpc_tab.nbch);
    // now do the parity checking
    unsigned j = 0;
    for (size_t row = 0; row < ldpc_tab.table.size(); row++) {
        for (int n = 0; n < 360; n++) {
            for (int col = 1; col < ldpc_tab.table[row][0]; col++) {
                auto i = (ldpc_tab.table[row][col] + (n * ldpc_tab.q_val)) % plen;
                // auto j = row * 360 + n;
                p[i] ^= d[j];
            }
            j++;
        }
    }

    // Puncturing
    if (ldpc_tab.P != 0) {
        puncture = 0;
        for (int j = 0; j < plen; j += ldpc_tab.P) {
            p[j] = 0x55;
            puncture++;
            if (puncture == ldpc_tab.Xp) {
                break;
            }
        }
        index = 0;
        for (int j = 0; j < plen; j++) {
            if (p[j] != 0x55) {
                b[index++] = p[j];
            }
        }
        p = &out[ldpc_tab.nbch];
    }
    for (int j = 1; j < (plen - ldpc_tab.Xp); j++) {
        p[j] ^= p[j - 1];
    }

    consume_each(ldpc_tab.nbch);
    return ldpc_tab.frame_size;
}

} /* namespace dvbs2acm */
} /* namespace gr */
