/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_DEINTERLEAVER_BB_IMPL_H
#define INCLUDED_DVBS2ACM_DEINTERLEAVER_BB_IMPL_H

#include <gnuradio/dvbs2acm/deinterleaver_bb.h>
#include <gnuradio/dvbs2acm/dvbs2_config.h>

namespace gr {
namespace dvbs2acm {

class deinterleaver_bb_impl : public deinterleaver_bb
{
private:
    dvbs2_constellation_t constellation;
    int frame_size;
    int mod_order;
    int rowaddr[5];

    void get_rows(dvbs2_framesize_t, dvbs2_code_rate_t, dvbs2_constellation_t);

public:
    deinterleaver_bb_impl();
    ~deinterleaver_bb_impl();

    // Where all the action really happens
    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);

    void deinterleave(const int8_t* in, int8_t* out, int& consumed, int& produced);
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_DEINTERLEAVER_BB_IMPL_H */
