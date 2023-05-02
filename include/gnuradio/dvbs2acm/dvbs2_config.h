/* -*- c++ -*- */
/*
 * Copyright 2014,2016,2017,2018 Ron Economos.
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

#ifndef INCLUDED_DVBS2_CONFIG_H
#define INCLUDED_DVBS2_CONFIG_H

#define BB_HEADER_LENGTH_BITS 80

// BB HEADER fields
#define TS_GS_TRANSPORT 3
#define TS_GS_GENERIC_PACKETIZED 0
#define TS_GS_GENERIC_CONTINUOUS 1
#define TS_GS_RESERVED 2

#define SIS_MIS_SINGLE 1
#define SIS_MIS_MULTIPLE 0

#define CCM 1
#define ACM 0

#define ISSYI_ACTIVE 1
#define ISSYI_NOT_ACTIVE 0

#define NPD_ACTIVE 1
#define NPD_NOT_ACTIVE 0

#define FRAME_SIZE_NORMAL 64800
#define FRAME_SIZE_MEDIUM 32400
#define FRAME_SIZE_SHORT 16200

// BCH Code
#define BCH_CODE_N8 0
#define BCH_CODE_N10 1
#define BCH_CODE_N12 2
#define BCH_CODE_S12 3
#define BCH_CODE_M12 4

#define LDPC_ENCODE_TABLE_LENGTH (FRAME_SIZE_NORMAL * 10)

#define NORMAL_PUNCTURING 3240
#define MEDIUM_PUNCTURING 1620
#define SHORT_PUNCTURING_SET1 810
#define SHORT_PUNCTURING_SET2 1224

#define EXTRA_PILOT_SYMBOLS_SET1 ((18 * 34) + (3 * 36))
#define EXTRA_PILOT_SYMBOLS_SET2 ((9 * 32) + 36)

namespace gr {
namespace dvbs2acm {

enum dvbs2_modcod_t {
    MC_DUMMY = 0,
    MC_DUMMY_S,
    // DVB-S2:
    MC_QPSK_1_4,
    MC_QPSK_1_4_S,
    MC_QPSK_1_3,
    MC_QPSK_1_3_S,
    MC_QPSK_2_5,
    MC_QPSK_2_5_S,
    MC_QPSK_1_2,
    MC_QPSK_1_2_S,
    MC_QPSK_3_5,
    MC_QPSK_3_5_S,
    MC_QPSK_2_3,
    MC_QPSK_2_3_S,
    MC_QPSK_3_4,
    MC_QPSK_3_4_S,
    MC_QPSK_4_5,
    MC_QPSK_4_5_S,
    MC_QPSK_5_6,
    MC_QPSK_5_6_S,
    MC_QPSK_8_9,
    MC_QPSK_8_9_S,
    MC_QPSK_9_10,
    MC_QPSK_9_10_S,
    MC_8PSK_3_5,
    MC_8PSK_3_5_S,
    MC_8PSK_2_3,
    MC_8PSK_2_3_S,
    MC_8PSK_3_4,
    MC_8PSK_3_4_S,
    MC_8PSK_5_6,
    MC_8PSK_5_6_S,
    MC_8PSK_8_9,
    MC_8PSK_8_9_S,
    MC_8PSK_9_10,
    MC_8PSK_9_10_S,
    MC_16APSK_2_3,
    MC_16APSK_2_3_S,
    MC_16APSK_3_4,
    MC_16APSK_3_4_S,
    MC_16APSK_4_5,
    MC_16APSK_4_5_S,
    MC_16APSK_5_6,
    MC_16APSK_5_6_S,
    MC_16APSK_8_9,
    MC_16APSK_8_9_S,
    MC_16APSK_9_10,
    MC_16APSK_9_10_S,
    MC_32APSK_3_4,
    MC_32APSK_3_4_S,
    MC_32APSK_4_5,
    MC_32APSK_4_5_S,
    MC_32APSK_5_6,
    MC_32APSK_5_6_S,
    MC_32APSK_8_9,
    MC_32APSK_8_9_S,
    MC_32APSK_9_10,
    MC_32APSK_9_10_S,
    // DVB-S2X VL-SNR:
    MC_VLSNR_SET1 = 0x40,
    MC_VLSNR_SET2 = 0x41,
    // DVB-S2X NORMAL:
    MC_QPSK_13_45 = 0x42,
    MC_QPSK_9_20,
    MC_QPSK_11_20,
    MC_8APSK_5_9_L,
    MC_8APSK_26_45_L,
    MC_8PSK_23_36,
    MC_8PSK_25_36,
    MC_8PSK_13_18,
    MC_16APSK_1_2_L,
    MC_16APSK_8_15_L,
    MC_16APSK_5_9_L,
    MC_16APSK_26_45,
    MC_16APSK_3_5,
    MC_16APSK_3_5_L,
    MC_16APSK_28_45,
    MC_16APSK_23_36,
    MC_16APSK_2_3_L,
    MC_16APSK_25_36,
    MC_16APSK_13_18,
    MC_16APSK_7_9,
    MC_16APSK_77_90,
    MC_32APSK_2_3_L,
    MC_32APSK_32_45,
    MC_32APSK_11_15,
    MC_32APSK_7_9,
    MC_64APSK_32_45_L,
    MC_64APSK_11_15,
    MC_64APSK_7_9,
    MC_64APSK_4_5,
    MC_64APSK_5_6,
    MC_128APSK_3_4,
    MC_128APSK_7_9,
    MC_256APSK_29_45_L,
    MC_256APSK_2_3_L,
    MC_256APSK_31_45_L,
    MC_256APSK_32_45,
    MC_256APSK_11_15_L,
    MC_256APSK_3_4,
    // DVB-S2X SHORT:
    MC_QPSK_11_45_S,
    MC_QPSK_4_15_S,
    MC_QPSK_14_45_S,
    MC_QPSK_7_15_S,
    MC_QPSK_8_15_S,
    MC_QPSK_32_45_S,
    MC_8PSK_7_15_S,
    MC_8PSK_8_15_S,
    MC_8PSK_26_45_S,
    MC_8PSK_32_45_S,
    MC_16APSK_7_15_S,
    MC_16APSK_8_15_S,
    MC_16APSK_26_45_S,
    MC_16APSK_3_5_S,
    MC_16APSK_32_45_S,
    MC_32APSK_2_3_S,
    MC_32APSK_32_45_S,
};

enum dvbs2_vlsnr_header_t {
    VLSNR_N_QPSK_2_9 = 0,
    VLSNR_M_BPSK_1_5,
    VLSNR_M_BPSK_11_45,
    VLSNR_M_BPSK_1_3,
    VLSNR_S_BPSK_SF2_1_5,
    VLSNR_S_BPSK_SF2_11_45,
    VLSNR_S_BPSK_1_5 = 9,
    VLSNR_S_BPSK_4_15,
    VLSNR_S_BPSK_1_3,
    VLSNR_DUMMY,
};

enum dvbs2_framesize_t {
    FECFRAME_NORMAL = 0,
    FECFRAME_SHORT,
    FECFRAME_MEDIUM,
};

enum dvbs2_code_rate_t {
    C1_4 = 0,
    C1_3,
    C2_5,
    C1_2,
    C3_5,
    C2_3,
    C3_4,
    C4_5,
    C5_6,
    C8_9,
    C9_10,
    C13_45,
    C9_20,
    C90_180,
    C96_180,
    C11_20,
    C100_180,
    C104_180,
    C26_45,
    C18_30,
    C28_45,
    C23_36,
    C116_180,
    C20_30,
    C124_180,
    C25_36,
    C128_180,
    C13_18,
    C132_180,
    C22_30,
    C135_180,
    C140_180,
    C7_9,
    C154_180,
    C11_45,
    C4_15,
    C14_45,
    C7_15,
    C8_15,
    C32_45,
    C2_9_VLSNR,
    C1_5_MEDIUM,
    C11_45_MEDIUM,
    C1_3_MEDIUM,
    C1_5_VLSNR_SF2,
    C11_45_VLSNR_SF2,
    C1_5_VLSNR,
    C4_15_VLSNR,
    C1_3_VLSNR,
    C_OTHER,
};

enum dvbs2_rolloff_factor_t {
    RO_0_35 = 0,
    RO_0_25,
    RO_0_20,
    RO_RESERVED,
    RO_0_15,
    RO_0_10,
    RO_0_05,
};

enum dvbs2_constellation_t {
    MOD_QPSK = 0,
    MOD_8PSK,
    MOD_8APSK,
    MOD_16APSK,
    MOD_8_8APSK,
    MOD_32APSK,
    MOD_4_12_16APSK,
    MOD_4_8_4_16APSK,
    MOD_BPSK,
    MOD_BPSK_SF2,
    MOD_OTHER,
};

} // namespace dvbs2acm
} // namespace gr

typedef gr::dvbs2acm::dvbs2_modcod_t dvbs2_modcod_t;
typedef gr::dvbs2acm::dvbs2_vlsnr_header_t dvbs2_vlsnr_header_t;
typedef gr::dvbs2acm::dvbs2_framesize_t dvbs2_framesize_t;
typedef gr::dvbs2acm::dvbs2_code_rate_t dvbs2_code_rate_t;
typedef gr::dvbs2acm::dvbs2_rolloff_factor_t dvbs2_rolloff_factor_t;
typedef gr::dvbs2acm::dvbs2_constellation_t dvbs2_constellation_t;

#endif /* INCLUDED_DVBS2_CONFIG_H */
