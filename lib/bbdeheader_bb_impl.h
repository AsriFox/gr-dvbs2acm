/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2018 Ron Economos.
 *
 * This file is part of gr-dvbs2rx.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_BBDEHEADER_BB_IMPL_H
#define INCLUDED_DVBS2ACM_BBDEHEADER_BB_IMPL_H

#include "bb_header.hh"
#include <gnuradio/dvbs2acm/bbdeheader_bb.h>

namespace gr {
namespace dvbs2acm {

class bbdeheader_bb_impl : public bbdeheader_bb
{
private:
    const int d_debug_level;
    unsigned int dvb_standard;
    unsigned int df_remaining;
    unsigned int count;
    bool synched;
    unsigned char crc;
    unsigned int distance;
    bool spanning;
    unsigned int index;
    uint64_t d_packet_cnt; /**< total packets received */
    uint64_t d_error_cnt;  /**< total packets with bit errors */
    FrameFormat m_format[1];
    unsigned char crc_tab[256];
    unsigned char packet[188];
    void build_crc8_table(void);
    unsigned int check_crc8_bits(const unsigned char*, int);

public:
    bbdeheader_bb_impl(int debug_level);
    ~bbdeheader_bb_impl();

    // Where all the action really happens
    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);

    uint64_t get_packet_count() { return d_packet_cnt; }
    uint64_t get_error_count() { return d_error_cnt; }
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_BBDEHEADER_BB_IMPL_H */
