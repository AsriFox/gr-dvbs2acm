/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2014,2016 Ron Economos.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_INTERLEAVER_BB_IMPL_H
#define INCLUDED_DVBS2ACM_INTERLEAVER_BB_IMPL_H

#include <gnuradio/dvbs2acm/dvbs2_config.h>
#include <gnuradio/dvbs2acm/interleaver_bb.h>

namespace gr {
namespace dvbs2acm {

class interleaver_bb_impl : public interleaver_bb
{
private:
    int rowaddr[5];
    void get_rows(dvbs2_framesize_t, dvbs2_code_rate_t, dvbs2_constellation_t, int&, int&);

public:
    interleaver_bb_impl();
    ~interleaver_bb_impl();

    // Where all the action really happens
    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_INTERLEAVER_BB_IMPL_H */
