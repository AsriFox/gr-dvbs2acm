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
    dvbs2_framesize_t framesize;
    dvbs2_constellation_t constellation;
    std::unique_ptr<Modulation<gr_complex, int8_t>> mod;
    float precision;

    gr::thread::mutex d_mutex;
    void handle_precision_msg(pmt::pmt_t msg);

protected:
    void parse_length_tags(const std::vector<std::vector<tag_t>>& tags,
                           gr_vector_int& n_input_items_reqd) override;
    int calculate_output_stream_length(const gr_vector_int& ninput_items) override;

public:
    demodulator_cb_impl(float precision);
    ~demodulator_cb_impl();

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_int& ninput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;

    void set_constellation(dvbs2_constellation_t);

    void set_precision(float precision) override;
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_DEMODULATOR_CB_IMPL_H */
