/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 *
 * This file is part of gr-dvbs2acm.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2_MODCOD_H
#define INCLUDED_DVBS2_MODCOD_H

#include <gnuradio/dvbs2acm/dvbs2_config.h>

inline bool modcod_is_dvbs2(dvbs2_modcod_t modcod) { return modcod < gr::dvbs2acm::MC_VLSNR_SET1; }

inline bool modcod_is_dvbs2x(dvbs2_modcod_t modcod) { return modcod >= gr::dvbs2acm::MC_VLSNR_SET1; }

inline dvbs2_framesize_t modcod_framesize(dvbs2_modcod_t modcod)
{
    using namespace gr::dvbs2acm;
    if ((modcod_is_dvbs2(modcod) && (modcod & 1)) || modcod >= MC_QPSK_11_45_S) {
        return FECFRAME_SHORT;
    } else {
        return FECFRAME_NORMAL;
    }
}

inline dvbs2_code_rate_t modcod_rate(dvbs2_modcod_t modcod)
{
    using namespace gr::dvbs2acm;
    switch (modcod) {
    // DVB-S2
    case MC_QPSK_1_4:
    case MC_QPSK_1_4_S:
        return C1_4;
    case MC_QPSK_1_3:
    case MC_QPSK_1_3_S:
        return C1_3;
    case MC_QPSK_2_5:
    case MC_QPSK_2_5_S:
        return C2_5;
    case MC_QPSK_1_2:
    case MC_QPSK_1_2_S:
        return C1_2;
    case MC_QPSK_3_5:
    case MC_QPSK_3_5_S:
        return C3_5;
    case MC_QPSK_2_3:
    case MC_QPSK_2_3_S:
        return C2_3;
    case MC_QPSK_3_4:
    case MC_QPSK_3_4_S:
        return C3_4;
    case MC_QPSK_4_5:
    case MC_QPSK_4_5_S:
        return C4_5;
    case MC_QPSK_5_6:
    case MC_QPSK_5_6_S:
        return C5_6;
    case MC_QPSK_8_9:
    case MC_QPSK_8_9_S:
        return C8_9;
    case MC_QPSK_9_10:
        // case MC_QPSK_9_10_S:
        return C9_10;
    case MC_8PSK_3_5:
    case MC_8PSK_3_5_S:
        return C3_5;
    case MC_8PSK_2_3:
    case MC_8PSK_2_3_S:
        return C2_3;
    case MC_8PSK_3_4:
    case MC_8PSK_3_4_S:
        return C3_4;
    case MC_8PSK_5_6:
    case MC_8PSK_5_6_S:
        return C5_6;
    case MC_8PSK_8_9:
    case MC_8PSK_8_9_S:
        return C8_9;
    case MC_8PSK_9_10:
        // case MC_8PSK_9_10_S:
        return C9_10;
    case MC_16APSK_2_3:
    case MC_16APSK_2_3_S:
        return C2_3;
    case MC_16APSK_3_4:
    case MC_16APSK_3_4_S:
        return C3_4;
    case MC_16APSK_4_5:
    case MC_16APSK_4_5_S:
        return C4_5;
    case MC_16APSK_5_6:
    case MC_16APSK_5_6_S:
        return C5_6;
    case MC_16APSK_8_9:
    case MC_16APSK_8_9_S:
        return C8_9;
    case MC_16APSK_9_10:
        // case MC_16APSK_9_10_S:
        return C9_10;
    case MC_32APSK_3_4:
    case MC_32APSK_3_4_S:
        return C3_4;
    case MC_32APSK_4_5:
    case MC_32APSK_4_5_S:
        return C4_5;
    case MC_32APSK_5_6:
    case MC_32APSK_5_6_S:
        return C5_6;
    case MC_32APSK_8_9:
    case MC_32APSK_8_9_S:
        return C8_9;
    case MC_32APSK_9_10:
        // case MC_32APSK_9_10_S:
        return C9_10;
    // DVB-S2X
    case MC_QPSK_13_45:
        return C13_45;
    case MC_QPSK_9_20:
        return C9_20;
    case MC_QPSK_11_20:
        return C11_20;
    case MC_8APSK_5_9_L:
        return C100_180;
    case MC_8APSK_26_45_L:
        return C104_180;
    case MC_8PSK_23_36:
        return C23_36;
    case MC_8PSK_25_36:
        return C25_36;
    case MC_8PSK_13_18:
        return C13_18;
    case MC_16APSK_1_2_L:
        return C90_180;
    case MC_16APSK_8_15_L:
        return C96_180;
    case MC_16APSK_5_9_L:
        return C100_180;
    case MC_16APSK_26_45:
        return C26_45;
    case MC_16APSK_3_5:
        return C3_5;
    case MC_16APSK_3_5_L:
        return C18_30;
    case MC_16APSK_28_45:
        return C28_45;
    case MC_16APSK_23_36:
        return C23_36;
    case MC_16APSK_2_3_L:
        return C20_30;
    case MC_16APSK_25_36:
        return C25_36;
    case MC_16APSK_13_18:
        return C13_18;
    case MC_16APSK_7_9:
        return C140_180;
    case MC_16APSK_77_90:
        return C154_180;
    case MC_32APSK_2_3_L:
        return C2_3;
    case MC_32APSK_32_45:
        return C128_180;
    case MC_32APSK_11_15:
        return C132_180;
    case MC_32APSK_7_9:
        return C140_180;
    case MC_64APSK_32_45_L:
        return C128_180;
    case MC_64APSK_11_15:
        return C132_180;
    case MC_64APSK_7_9:
        return C7_9;
    case MC_64APSK_4_5:
        return C4_5;
    case MC_64APSK_5_6:
        return C5_6;
    case MC_128APSK_3_4:
        return C135_180;
    case MC_128APSK_7_9:
        return C140_180;
    case MC_256APSK_29_45_L:
        return C116_180;
    case MC_256APSK_2_3_L:
        return C20_30;
    case MC_256APSK_31_45_L:
        return C124_180;
    case MC_256APSK_32_45:
        return C128_180;
    case MC_256APSK_11_15_L:
        return C22_30;
    case MC_256APSK_3_4:
        return C135_180;
    case MC_QPSK_11_45_S:
        return C11_45;
    case MC_QPSK_4_15_S:
        return C4_15;
    case MC_QPSK_14_45_S:
        return C14_45;
    case MC_QPSK_7_15_S:
        return C7_15;
    case MC_QPSK_8_15_S:
        return C8_15;
    case MC_QPSK_32_45_S:
        return C32_45;
    case MC_8PSK_7_15_S:
        return C7_15;
    case MC_8PSK_8_15_S:
        return C8_15;
    case MC_8PSK_26_45_S:
        return C26_45;
    case MC_8PSK_32_45_S:
        return C32_45;
    case MC_16APSK_7_15_S:
        return C7_15;
    case MC_16APSK_8_15_S:
        return C8_15;
    case MC_16APSK_26_45_S:
        return C26_45;
    case MC_16APSK_3_5_S:
        return C3_5;
    case MC_16APSK_32_45_S:
        return C32_45;
    case MC_32APSK_2_3_S:
        return C2_3;
    case MC_32APSK_32_45_S:
        return C32_45;
    default:
        return C_OTHER;
    }
}

inline dvbs2_constellation_t modcod_constellation(dvbs2_modcod_t modcod)
{
    using namespace gr::dvbs2acm;
    switch (modcod) {
    // DVB-S2
    case MC_QPSK_1_4:
    case MC_QPSK_1_4_S:
    case MC_QPSK_1_3:
    case MC_QPSK_1_3_S:
    case MC_QPSK_2_5:
    case MC_QPSK_2_5_S:
    case MC_QPSK_1_2:
    case MC_QPSK_1_2_S:
    case MC_QPSK_3_5:
    case MC_QPSK_3_5_S:
    case MC_QPSK_2_3:
    case MC_QPSK_2_3_S:
    case MC_QPSK_3_4:
    case MC_QPSK_3_4_S:
    case MC_QPSK_4_5:
    case MC_QPSK_4_5_S:
    case MC_QPSK_5_6:
    case MC_QPSK_5_6_S:
    case MC_QPSK_8_9:
    case MC_QPSK_8_9_S:
    case MC_QPSK_9_10:
        // case MC_QPSK_9_10_S:
        return MOD_QPSK;
    case MC_8PSK_3_5:
    case MC_8PSK_3_5_S:
    case MC_8PSK_2_3:
    case MC_8PSK_2_3_S:
    case MC_8PSK_3_4:
    case MC_8PSK_3_4_S:
    case MC_8PSK_5_6:
    case MC_8PSK_5_6_S:
    case MC_8PSK_8_9:
    case MC_8PSK_8_9_S:
    case MC_8PSK_9_10:
        // case MC_8PSK_9_10_S:
        return MOD_8PSK;
    case MC_16APSK_2_3:
    case MC_16APSK_2_3_S:
    case MC_16APSK_3_4:
    case MC_16APSK_3_4_S:
    case MC_16APSK_4_5:
    case MC_16APSK_4_5_S:
    case MC_16APSK_5_6:
    case MC_16APSK_5_6_S:
    case MC_16APSK_8_9:
    case MC_16APSK_8_9_S:
    case MC_16APSK_9_10:
        // case MC_16APSK_9_10_S:
        return MOD_16APSK;
    case MC_32APSK_3_4:
    case MC_32APSK_3_4_S:
    case MC_32APSK_4_5:
    case MC_32APSK_4_5_S:
    case MC_32APSK_5_6:
    case MC_32APSK_5_6_S:
    case MC_32APSK_8_9:
    case MC_32APSK_8_9_S:
    case MC_32APSK_9_10:
        // case MC_32APSK_9_10_S:
        return MOD_32APSK;
    // DVB-S2X
    case MC_QPSK_13_45:
    case MC_QPSK_9_20:
    case MC_QPSK_11_20:
        return MOD_QPSK;
    case MC_8APSK_5_9_L:
    case MC_8APSK_26_45_L:
        return MOD_8APSK;
    case MC_8PSK_23_36:
    case MC_8PSK_25_36:
    case MC_8PSK_13_18:
        return MOD_8PSK;
    case MC_16APSK_1_2_L:
    case MC_16APSK_8_15_L:
    case MC_16APSK_5_9_L:
    case MC_16APSK_3_5_L:
    case MC_16APSK_2_3_L:
        return MOD_8_8APSK;
    case MC_16APSK_26_45:
    case MC_16APSK_3_5:
    case MC_16APSK_28_45:
    case MC_16APSK_23_36:
    case MC_16APSK_25_36:
    case MC_16APSK_13_18:
    case MC_16APSK_7_9:
    case MC_16APSK_77_90:
        return MOD_16APSK;
    case MC_32APSK_2_3_L:
        return MOD_4_12_16APSK;
    case MC_32APSK_32_45:
    case MC_32APSK_11_15:
    case MC_32APSK_7_9:
        return MOD_4_8_4_16APSK;
    case MC_QPSK_11_45_S:
    case MC_QPSK_4_15_S:
    case MC_QPSK_14_45_S:
    case MC_QPSK_7_15_S:
    case MC_QPSK_8_15_S:
    case MC_QPSK_32_45_S:
        return MOD_QPSK;
    case MC_8PSK_7_15_S:
    case MC_8PSK_8_15_S:
    case MC_8PSK_26_45_S:
    case MC_8PSK_32_45_S:
        return MOD_8PSK;
    case MC_16APSK_7_15_S:
    case MC_16APSK_8_15_S:
    case MC_16APSK_26_45_S:
    case MC_16APSK_3_5_S:
    case MC_16APSK_32_45_S:
        return MOD_16APSK;
    case MC_32APSK_2_3_S:
    case MC_32APSK_32_45_S:
        return MOD_4_12_16APSK;
    default:
        // TODO: 64APSK, 128APSK, 256APSK
        return MOD_OTHER;
    }
}

inline dvbs2_framesize_t vlsnr_framesize(dvbs2_vlsnr_header_t vlsnr_header)
{
    using namespace gr::dvbs2acm;
    switch (vlsnr_header) {
    case VLSNR_N_QPSK_2_9:
    case VLSNR_DUMMY:
        return FECFRAME_NORMAL;
    case VLSNR_M_BPSK_1_5:
    case VLSNR_M_BPSK_11_45:
    case VLSNR_M_BPSK_1_3:
        return FECFRAME_MEDIUM;
    case VLSNR_S_BPSK_SF2_1_5:
    case VLSNR_S_BPSK_SF2_11_45:
    case VLSNR_S_BPSK_1_5:
    case VLSNR_S_BPSK_4_15:
    case VLSNR_S_BPSK_1_3:
        return FECFRAME_SHORT;
    }
    // TODO: fallback?
    return FECFRAME_NORMAL;
}

inline dvbs2_code_rate_t vlsnr_rate(dvbs2_vlsnr_header_t vlsnr_header)
{
    using namespace gr::dvbs2acm;
    switch (vlsnr_header) {
    case VLSNR_N_QPSK_2_9:
        return C2_9_VLSNR;
    case VLSNR_M_BPSK_1_5:
        return C1_5_MEDIUM;
    case VLSNR_M_BPSK_11_45:
        return C11_45_MEDIUM;
    case VLSNR_M_BPSK_1_3:
        return C1_3_MEDIUM;
    case VLSNR_S_BPSK_SF2_1_5:
        return C1_5_VLSNR_SF2;
    case VLSNR_S_BPSK_SF2_11_45:
        return C11_45_VLSNR_SF2;
    case VLSNR_S_BPSK_1_5:
        return C1_5_VLSNR;
    case VLSNR_S_BPSK_4_15:
        return C4_15_VLSNR;
    case VLSNR_S_BPSK_1_3:
        return C1_3_VLSNR;
    default:
        return C_OTHER;
    }
}

inline dvbs2_constellation_t vlsnr_constellation(dvbs2_vlsnr_header_t vlsnr_header)
{
    using namespace gr::dvbs2acm;
    switch (vlsnr_header) {
    case VLSNR_N_QPSK_2_9:
        return MOD_QPSK;
    case VLSNR_M_BPSK_1_5:
    case VLSNR_M_BPSK_11_45:
    case VLSNR_M_BPSK_1_3:
    case VLSNR_S_BPSK_1_5:
    case VLSNR_S_BPSK_4_15:
    case VLSNR_S_BPSK_1_3:
        return MOD_BPSK;
    case VLSNR_S_BPSK_SF2_1_5:
    case VLSNR_S_BPSK_SF2_11_45:
        return MOD_BPSK_SF2;
    default:
        return MOD_OTHER;
    }
}

#endif /* INCLUDED_DVBS2_MODCOD_H */
