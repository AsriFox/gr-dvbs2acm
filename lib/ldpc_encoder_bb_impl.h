/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2014,2016 Ron Economos.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_LDPC_ENCODER_BB_IMPL_H
#define INCLUDED_DVBS2ACM_LDPC_ENCODER_BB_IMPL_H

#include "ldpc_encode_tables.h"
#include <gnuradio/dvbs2acm/dvbs2_config.h>
#include <gnuradio/dvbs2acm/ldpc_encoder_bb.h>

namespace gr {
namespace dvbs2acm {

class ldpc_encoder_bb_impl : public ldpc_encoder_bb
{
private:
    ldpc_encode_table ldpc_tab;
    unsigned char puncturing_buffer[FRAME_SIZE_NORMAL];
    unsigned char shortening_buffer[FRAME_SIZE_NORMAL];

protected:
    void parse_length_tags(const std::vector<std::vector<tag_t>>& tags,
                           gr_vector_int& n_input_items_reqd) override;

public:
    ldpc_encoder_bb_impl();
    ~ldpc_encoder_bb_impl();

    int work(int noutput_items,
             gr_vector_int& ninput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_LDPC_ENCODER_BB_IMPL_H */
