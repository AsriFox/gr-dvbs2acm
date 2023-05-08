/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2014,2016 Ron Economos.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_BBSCRAMBLER_BB_IMPL_H
#define INCLUDED_DVBS2ACM_BBSCRAMBLER_BB_IMPL_H

#include <gnuradio/dvbs2acm/bbscrambler_bb.h>

namespace gr {
namespace dvbs2acm {

class bbscrambler_bb_impl : public bbscrambler_bb
{
private:
    unsigned char bb_randomize[FRAME_SIZE_NORMAL];
    void init_bb_randomizer(void);


protected:
    void parse_length_tags(const std::vector<std::vector<tag_t>>& tags,
                           gr_vector_int& n_input_items_reqd) override;

public:
    bbscrambler_bb_impl();
    ~bbscrambler_bb_impl();

    int work(int noutput_items,
             gr_vector_int& ninput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_BBSCRAMBLER_BB_IMPL_H */
