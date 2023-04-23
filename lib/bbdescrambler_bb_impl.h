/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2018 Ron Economos.
 *
 * This file is part of gr-dvbs2rx.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_BBDESCRAMBLER_BB_IMPL_H
#define INCLUDED_DVBS2ACM_BBDESCRAMBLER_BB_IMPL_H

#include <gnuradio/dvbs2acm/bbdescrambler_bb.h>
#include <gnuradio/dvbs2acm/dvbs2_config.h>

namespace gr {
namespace dvbs2acm {

class bbdescrambler_bb_impl : public bbdescrambler_bb
{
private:
    unsigned char bb_derandomize[FRAME_SIZE_NORMAL];
    void init_bb_derandomiser(void);

public:
    bbdescrambler_bb_impl();
    ~bbdescrambler_bb_impl();

    // Where all the action really happens
    int work(int noutput_items, gr_vector_const_void_star& input_items, gr_vector_void_star& output_items);
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_BBDESCRAMBLER_BB_IMPL_H */
