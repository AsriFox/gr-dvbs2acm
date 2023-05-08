/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2014,2016,2017,2020 Ron Economos.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_PHYSICAL_CC_IMPL_H
#define INCLUDED_DVBS2ACM_PHYSICAL_CC_IMPL_H

#define VLSNR_HEADER_LENGTH 900

#include <gnuradio/dvbs2acm/dvbs2_config.h>
#include <gnuradio/dvbs2acm/physical_cc.h>

namespace gr {
namespace dvbs2acm {

class physical_cc_impl : public physical_cc
{
private:
    bool dummy_frames;
    int b[VLSNR_HEADER_LENGTH];
    gr_complex m_bpsk[4][2];
    gr_complex m_pl[90];
    gr_complex m_pl_dummy[90];
    gr_complex m_vlsnr_header[VLSNR_HEADER_LENGTH];
    gr_complex m_zero;
    int m_cscram_x;
    int m_cscram_y;
    void b_64_8_code(unsigned char, int*);
    void pl_header_encode(dvbs2_modcod_t, bool, dvbs2_vlsnr_header_t);
    inline int parity_chk(int, int);
    inline int symbol_scrambler(void);

    int root_code;
    dvbs2_modcod_t modcod;
    dvbs2_vlsnr_header_t vlsnr_header;
    int slots;
    int pilot_symbols;
    int get_slots(dvbs2_modcod_t, dvbs2_vlsnr_header_t);

    const static unsigned int g[7];
    const static int ph_scram_tab[64];
    const static int ph_sync_seq[26];
    const static int ph_vlsnr_seq[16][VLSNR_HEADER_LENGTH - 4];

protected:
    void parse_length_tags(const std::vector<std::vector<tag_t>>& tags,
                           gr_vector_int& n_input_items_reqd) override;

public:
    physical_cc_impl(bool dummyframes);
    ~physical_cc_impl();

    int work(int noutput_items,
             gr_vector_int& ninput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_PHYSICAL_CC_IMPL_H */
