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

#ifndef INCLUDED_DVBS2ACM_BB_HEADER_H
#define INCLUDED_DVBS2ACM_BB_HEADER_H

#define TRANSPORT_PACKET_LENGTH 188

struct BBHeader {
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

    void add_to_frame(unsigned char* frame, int count, bool nibble, bool dvbs2x_alternate);
    void parse(const unsigned char* in);
};

void build_crc8_table(unsigned char* crc_tab);

unsigned int check_crc8_bits(const unsigned char* in, int count);

int unpack_bits_8(int temp, unsigned char* out);

int unpack_bits_16(int temp, unsigned char* out);

#endif