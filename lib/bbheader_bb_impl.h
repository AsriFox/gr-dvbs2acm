/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2014,2016,2017,2020 Ron Economos.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_DVBS2ACM_BBHEADER_BB_IMPL_H
#define INCLUDED_DVBS2ACM_BBHEADER_BB_IMPL_H

#include <gnuradio/dvbs2acm/bbheader_bb.h>

typedef struct {
    int ts_gs;
    int sis_mis;
    int ccm_acm;
    int issyi;
    int npd;
    int ro;
    int isi;
    int upl;
    int dfl;
    int sync;
    int syncd;
} BBHeader;

typedef struct {
    BBHeader bb_header;
} FrameFormat;

namespace gr {
namespace dvbs2acm {

class bbheader_bb_impl : public bbheader_bb
{
private:
    unsigned int kbch;
    unsigned int count;
    unsigned char crc;
    gr::dvbs2::dvbs2_framesize_t framesize;
    gr::dvbs2::dvbs2_code_rate_t code_rate;
    gr::dvbs2::dvbs2_constellation_t constellation;
    gr::dvbs2::dvbs2_pilots_t pilots;
    unsigned int root_code;
    unsigned char bsave;

    bool dvbs2x;
    bool alternate;
    bool nibble;
    FrameFormat m_format[1];
    unsigned char crc_tab[256];

    void add_bbheader(unsigned char*, int, bool);
    int gold_to_root(int);
    void build_crc8_table(void);
    int add_crc8_bits(unsigned char*, int);

public:
    bbheader_bb_impl(gr::dvbs2::dvbs2_framesize_t framesize,
                     gr::dvbs2::dvbs2_code_rate_t code_rate,
                     gr::dvbs2::dvbs2_constellation_t constellation,
                     gr::dvbs2::dvbs2_pilots_t pilots,
                     gr::dvbs2::dvbs2_rolloff_factor_t rolloff,
                     int goldcode);
    ~bbheader_bb_impl();

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_BBHEADER_BB_IMPL_H */
