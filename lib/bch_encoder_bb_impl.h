/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2014,2016 Ron Economos.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_BCH_ENCODER_BB_IMPL_H
#define INCLUDED_DVBS2ACM_BCH_ENCODER_BB_IMPL_H

#include "bch_code.h"
#include <gnuradio/dvbs2acm/bch_encoder_bb.h>

namespace gr {
namespace dvbs2acm {

class bch_encoder_bb_impl : public bch_encoder_bb
{
private:
    bch_code params;

protected:
    void parse_length_tags(const std::vector<std::vector<tag_t>>& tags,
                           gr_vector_int& n_input_items_reqd) override;

public:
    bch_encoder_bb_impl();
    ~bch_encoder_bb_impl();

    int work(int noutput_items,
             gr_vector_int& ninput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_BCH_ENCODER_BB_IMPL_H */
