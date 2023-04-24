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

#include "ldpc_encode_tables.h"
#include "ldpc_encoder_bb_impl.h"
#include <gnuradio/io_signature.h>
#include <vector>

using namespace gr::dvbs2;

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
    : gr::block("ldpc_bb",
                gr::io_signature::make(1, 1, sizeof(unsigned char)),
                gr::io_signature::make(1, 1, sizeof(unsigned char)))
{
    set_min_output_buffer((FRAME_SIZE_NORMAL)*6);
    set_tag_propagation_policy(TPP_DONT);
    set_output_multiple(FRAME_SIZE_NORMAL);
}

/*
 * Our virtual destructor.
 */
ldpc_encoder_bb_impl::~ldpc_encoder_bb_impl() {}

void ldpc_encoder_bb_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    ninput_items_required[0] = noutput_items;
}

int ldpc_encoder_bb_impl::general_work(int noutput_items,
                                       gr_vector_int& ninput_items,
                                       gr_vector_const_void_star& input_items,
                                       gr_vector_void_star& output_items)
{
    auto in = static_cast<const input_type*>(input_items[0]);
    auto out = static_cast<output_type*>(output_items[0]);
    const output_type* d = in;
    output_type *p = out, *b = out, *s;
    int consumed = 0;
    int produced = 0;
    int produced_per_iteration;
    int plen, puncture, index;

    std::vector<tag_t> tags;
    const uint64_t nread = this->nitems_read(0); // number of items read on port 0

    // Read all tags on the input buffer
    this->get_tags_in_range(tags, 0, nread, nread + noutput_items, pmt::string_to_symbol("modcod"));

    for (tag_t tag : tags) {
        auto tagmodcod = pmt::to_uint64(tag.value);
        auto framesize = (dvbs2_framesize_t)((tagmodcod >> 1) & 0x7f);
        auto rate = (dvbs2_code_rate_t)((tagmodcod >> 8) & 0xff);
        auto params = gr::dvbs2::ldpc_encode_table::select(framesize, rate);
        if (params.frame_size + produced > (unsigned)noutput_items) {
            break;
        }
        produced_per_iteration = 0;
        const uint64_t tagoffset = this->nitems_written(0);
        pmt::pmt_t key = pmt::string_to_symbol("modcod");
        pmt::pmt_t value = pmt::from_uint64(tagmodcod);
        this->add_item_tag(0, tagoffset, key, value);

        auto nbch = params.nbch;
        plen = (params.frame_size_real + params.Xp) - nbch;
        p += nbch;
        if (params.Xs != 0) {
            s = &shortening_buffer[0];
            memset(s, 0, sizeof(unsigned char) * params.Xs);
            memcpy(&s[params.Xs], &in[consumed], sizeof(unsigned char) * nbch);
            d = s;
        }
        if (params.P != 0) {
            p = &puncturing_buffer[params.nbch];
            b = &out[produced + params.nbch];
        }
        // First zero all the parity bits
        memset(p, 0, sizeof(unsigned char) * plen);
        for (int j = 0; j < (int)nbch; j++) {
            out[produced + j] = in[consumed];
            consumed++;
        }
        produced += nbch;
        produced_per_iteration += nbch;
        // now do the parity checking
        unsigned j = 0;
        for (size_t row = 0; row < params.table.size(); row++) {
            for (int n = 0; n < 360; n++) {
                for (int col = 1; col < params.table[row][0]; col++) {
                    auto i = (params.table[row][col] + (n * params.q_val)) % plen;
                    // auto j = row * 360 + n;
                    p[i] ^= d[j];
                }
                j++;
            }
        }
        // for (int j = 0; j < ldpc_encode[table].table_length; j++) {
        //     p[ldpc_encode[table].p[j]] ^= d[ldpc_encode[table].d[j]];
        // }
        if (params.P != 0) {
            puncture = 0;
            for (int j = 0; j < plen; j += params.P) {
                p[j] = 0x55;
                puncture++;
                if (puncture == params.Xp) {
                    break;
                }
            }
            index = 0;
            for (int j = 0; j < plen; j++) {
                if (p[j] != 0x55) {
                    b[index++] = p[j];
                }
            }
            p = &out[nbch];
        }
        for (int j = 1; j < (plen - params.Xp); j++) {
            p[j] ^= p[j - 1];
        }
        produced += plen - params.Xp;
        produced_per_iteration += plen - params.Xp;
        d += nbch;
        p += params.frame_size - nbch;
        produce(0, produced_per_iteration);
    }
    consume_each(consumed);
    return WORK_CALLED_PRODUCE;
}

} /* namespace dvbs2acm */
} /* namespace gr */
