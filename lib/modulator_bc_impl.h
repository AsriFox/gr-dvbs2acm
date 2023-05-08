/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2014,2016 Ron Economos.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_MODULATOR_BC_IMPL_H
#define INCLUDED_DVBS2ACM_MODULATOR_BC_IMPL_H

#include <gnuradio/dvbs2acm/dvbs2_config.h>
#include <gnuradio/dvbs2acm/modulator_bc.h>
#include <gnuradio/gr_complex.h>

namespace gr {
namespace dvbs2acm {

class modulator_bc_impl : public modulator_bc
{
private:
    gr_complex m_bpsk[2][2];
    gr_complex m_qpsk[4];
    gr_complex m_8psk[8];
    gr_complex m_8apsk[8][2];
    gr_complex m_16apsk[16][12];
    gr_complex m_8_8apsk[16][3];
    gr_complex m_32apsk[32][5];
    gr_complex m_4_12_16apsk[32][3];
    gr_complex m_4_8_4_16apsk[32][3];

    dvbs2_constellation_t constellation;
    int num_items;
    int constellation_index;

    dvbs2_constellation_t
    get_items(dvbs2_modcod_t, dvbs2_vlsnr_header_t, int& num_items, int& constellation_index);

protected:
    void parse_length_tags(const std::vector<std::vector<tag_t>>& tags,
                           gr_vector_int& n_input_items_reqd) override;

public:
    modulator_bc_impl();
    ~modulator_bc_impl();

    int work(int noutput_items,
             gr_vector_int& ninput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_MODULATOR_BC_IMPL_H */
