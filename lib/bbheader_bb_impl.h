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

#include "bb_header.hh"
#include <gnuradio/dvbs2acm/bbheader_bb.h>
#include <array>

namespace gr {
namespace dvbs2acm {

class bbheader_bb_impl : public bbheader_bb
{
private:
    unsigned int kbch;
    unsigned int count;
    unsigned char crc;
    dvbs2_modcod_t modcod;
    dvbs2_vlsnr_header_t vlsnr_header;
    bool pilots;
    unsigned int root_code;
    unsigned char bsave;

    bool dvbs2x;
    bool alternate;
    bool nibble;
    BBHeader header;
    std::array<unsigned char, 256> crc_tab;

    gr::thread::mutex d_mutex;

    void handle_cmd_msg(pmt::pmt_t msg);

    int gold_to_root(int);

public:
    bbheader_bb_impl(int, bool, dvbs2_rolloff_factor_t, int);
    ~bbheader_bb_impl();

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    void set_modcod(int modcod) override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_BBHEADER_BB_IMPL_H */
