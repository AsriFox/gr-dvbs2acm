/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_DEMODULATOR_CB_IMPL_H
#define INCLUDED_DVBS2ACM_DEMODULATOR_CB_IMPL_H

#include "modulation.hh"
#include <gnuradio/dvbs2acm/demodulator_cb.h>
#include <gnuradio/dvbs2acm/dvbs2_config.h>

namespace gr {
namespace dvbs2acm {

class demodulator_cb_impl : public demodulator_cb
{
private:
    float precision;
    int frame_size;
    dvbs2_constellation_t constellation;
    std::unique_ptr<Modulation<gr_complex, int8_t>> mod;

public:
    demodulator_cb_impl(float precision);
    ~demodulator_cb_impl();

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;

    void demodulate(const gr_complex* in, int8_t* out, int& consumed, int& produced);

    void set_precision(float precision) override;

    void set_constellation(dvbs2_constellation_t);
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_DEMODULATOR_CB_IMPL_H */
