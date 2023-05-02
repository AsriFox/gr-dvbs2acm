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

inline gr::dvbs2acm::dvbs2_code_rate_t modcod_rate(int modcod)
{
    using namespace gr::dvbs2acm;
    if (modcod <= 64) {
        // DVB-S2
        switch (modcod >> 1) {
        case MC_QPSK_1_4:
            return C1_4;
        case MC_QPSK_1_3:
            return C1_3;
        case MC_QPSK_2_5:
            return C2_5;
        case MC_QPSK_1_2:
            return C1_2;
        case MC_QPSK_3_5:
            return C3_5;
        case MC_QPSK_2_3:
            return C2_3;
        case MC_QPSK_3_4:
            return C3_4;
        case MC_QPSK_4_5:
            return C4_5;
        case MC_QPSK_5_6:
            return C5_6;
        case MC_QPSK_8_9:
            return C8_9;
        case MC_QPSK_9_10:
            return C9_10;
        case MC_8PSK_3_5:
            return C3_5;
        case MC_8PSK_2_3:
            return C2_3;
        case MC_8PSK_3_4:
            return C3_4;
        case MC_8PSK_5_6:
            return C5_6;
        case MC_8PSK_8_9:
            return C8_9;
        case MC_8PSK_9_10:
            return C9_10;
        case MC_16APSK_2_3:
            return C2_3;
        case MC_16APSK_3_4:
            return C3_4;
        case MC_16APSK_4_5:
            return C4_5;
        case MC_16APSK_5_6:
            return C5_6;
        case MC_16APSK_8_9:
            return C8_9;
        case MC_16APSK_9_10:
            return C9_10;
        case MC_32APSK_3_4:
            return C3_4;
        case MC_32APSK_4_5:
            return C4_5;
        case MC_32APSK_5_6:
            return C5_6;
        case MC_32APSK_8_9:
            return C8_9;
        case MC_32APSK_9_10:
            return C9_10;
        default:
            return C_OTHER;
        }
    } else {
        // DVB-S2X
        switch (modcod) {
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
}

inline gr::dvbs2acm::dvbs2_constellation_t modcod_constellation(int modcod)
{
    using namespace gr::dvbs2acm;
    if (modcod <= 64) {
        // DVB-S2
        switch (modcod >> 1) {
        case MC_QPSK_1_4:
        case MC_QPSK_1_3:
        case MC_QPSK_2_5:
        case MC_QPSK_1_2:
        case MC_QPSK_3_5:
        case MC_QPSK_2_3:
        case MC_QPSK_3_4:
        case MC_QPSK_4_5:
        case MC_QPSK_5_6:
        case MC_QPSK_8_9:
        case MC_QPSK_9_10:
            return MOD_QPSK;
        case MC_8PSK_3_5:
        case MC_8PSK_2_3:
        case MC_8PSK_3_4:
        case MC_8PSK_5_6:
        case MC_8PSK_8_9:
        case MC_8PSK_9_10:
            return MOD_8PSK;
        case MC_16APSK_2_3:
        case MC_16APSK_3_4:
        case MC_16APSK_4_5:
        case MC_16APSK_5_6:
        case MC_16APSK_8_9:
        case MC_16APSK_9_10:
            return MOD_16APSK;
        case MC_32APSK_3_4:
        case MC_32APSK_4_5:
        case MC_32APSK_5_6:
        case MC_32APSK_8_9:
        case MC_32APSK_9_10:
            return MOD_32APSK;
        default:
            return MOD_OTHER;
        }
    } else {
        // DVB-S2X
        switch (modcod) {
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
}

#endif /* INCLUDED_DVBS2_MODCOD_H */
