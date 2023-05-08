/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2014,2016 Ron Economos.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_BCH_ENCODER_BB_IMPL_H
#define INCLUDED_DVBS2ACM_BCH_ENCODER_BB_IMPL_H

#include <gnuradio/dvbs2acm/bch_encoder_bb.h>

namespace gr {
namespace dvbs2acm {

class bch_encoder_bb_impl : public bch_encoder_bb
{
private:
    unsigned int m_poly_n_8[4];
    unsigned int m_poly_n_10[5];
    unsigned int m_poly_n_12[6];
    unsigned int m_poly_s_12[6];
    unsigned int m_poly_m_12[6];
    unsigned int get_kbch_nbch(dvbs2_framesize_t, dvbs2_code_rate_t, unsigned int&, unsigned int&);
    int poly_mult(const int*, int, const int*, int, int*);
    void poly_pack(const int*, unsigned int*, int);
    void poly_reverse(int*, int*, int);
    inline void reg_4_shift(unsigned int*);
    inline void reg_5_shift(unsigned int*);
    inline void reg_6_shift(unsigned int*);
    void bch_poly_build_tables(void);

public:
    bch_encoder_bb_impl();
    ~bch_encoder_bb_impl();

    // Where all the action really happens
    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_BCH_ENCODER_BB_IMPL_H */
