#include "ldpc_encode_tables.h"
#include "modcod.hh"

namespace gr {
namespace dvbs2acm {

ldpc_encode_table::ldpc_encode_table(unsigned int frame_size,
                                     unsigned int frame_size_real,
                                     unsigned int nbch,
                                     unsigned int q_val,
                                     int Xs,
                                     int P,
                                     int Xp,
                                     std::vector<std::vector<int>> table)
    : frame_size(frame_size),
      frame_size_real(frame_size_real),
      nbch(nbch),
      q_val(q_val),
      Xs(Xs),
      P(P),
      Xp(Xp),
      table(table)
{
}

ldpc_encode_table ldpc_encode_table::select(dvbs2_modcod_t modcod, dvbs2_vlsnr_header_t vlsnr_header)
{
    if (modcod == MC_VLSNR_SET1 || modcod == MC_VLSNR_SET2) {
        switch (vlsnr_header) {
        case VLSNR_N_QPSK_2_9:
            return ldpc_tab_VLSNR_2_9N;
        case VLSNR_M_BPSK_1_5:
            return ldpc_tab_VLSNR_1_5M;
        case VLSNR_M_BPSK_11_45:
            return ldpc_tab_VLSNR_11_45M;
        case VLSNR_M_BPSK_1_3:
            return ldpc_tab_VLSNR_1_3M;
        case VLSNR_S_BPSK_SF2_1_5:
            return ldpc_tab_VLSNR_1_5SF2;
        case VLSNR_S_BPSK_SF2_11_45:
            return ldpc_tab_VLSNR_11_45SF2;
        case VLSNR_S_BPSK_1_5:
            return ldpc_tab_VLSNR_1_5S;
        case VLSNR_S_BPSK_4_15:
            return ldpc_tab_VLSNR_4_15S;
        case VLSNR_S_BPSK_1_3:
            return ldpc_tab_VLSNR_1_3S;
        default:
            return { 0, 0, 0, 0, 0, 0, 0, {} };
        }
    }

    switch (modcod) {
    // DVB-S2
    // FECFRAME_NORMAL
    case MC_QPSK_1_4:
        return ldpc_tab_1_4N;
    case MC_QPSK_1_3:
        return ldpc_tab_1_3N;
    case MC_QPSK_2_5:
        return ldpc_tab_2_5N;
    case MC_QPSK_1_2:
        return ldpc_tab_1_2N;
    case MC_QPSK_3_5:
    case MC_8PSK_3_5:
        return ldpc_tab_3_5N;
    case MC_QPSK_2_3:
    case MC_8PSK_2_3:
    case MC_16APSK_2_3:
        return ldpc_tab_2_3N;
    case MC_QPSK_3_4:
    case MC_8PSK_3_4:
    case MC_16APSK_3_4:
    case MC_32APSK_3_4:
        return ldpc_tab_3_4N;
    case MC_QPSK_4_5:
    case MC_16APSK_4_5:
    case MC_32APSK_4_5:
        return ldpc_tab_4_5N;
    case MC_QPSK_5_6:
    case MC_8PSK_5_6:
    case MC_16APSK_5_6:
    case MC_32APSK_5_6:
        return ldpc_tab_5_6N;
    case MC_QPSK_8_9:
    case MC_8PSK_8_9:
    case MC_16APSK_8_9:
    case MC_32APSK_8_9:
        return ldpc_tab_8_9N;
    case MC_QPSK_9_10:
    case MC_8PSK_9_10:
    case MC_16APSK_9_10:
    case MC_32APSK_9_10:
        return ldpc_tab_9_10N;
    // FECFRAME_SHORT
    case MC_QPSK_1_4_S:
        return ldpc_tab_1_4S;
    case MC_QPSK_1_3_S:
        return ldpc_tab_1_3S;
    case MC_QPSK_2_5_S:
        return ldpc_tab_2_5S;
    case MC_QPSK_1_2_S:
        return ldpc_tab_1_2S;
    case MC_QPSK_3_5_S:
    case MC_8PSK_3_5_S:
        return ldpc_tab_3_5S;
    case MC_QPSK_2_3_S:
    case MC_8PSK_2_3_S:
    case MC_16APSK_2_3_S:
        return ldpc_tab_2_3S;
    case MC_8PSK_3_4_S:
    case MC_16APSK_3_4_S:
    case MC_32APSK_3_4_S:
        return ldpc_tab_3_4S;
    case MC_QPSK_4_5_S:
    case MC_16APSK_4_5_S:
    case MC_32APSK_4_5_S:
        return ldpc_tab_4_5S;
    case MC_8PSK_5_6_S:
    case MC_16APSK_5_6_S:
    case MC_32APSK_5_6_S:
        return ldpc_tab_5_6S;
    case MC_8PSK_8_9_S:
    case MC_16APSK_8_9_S:
    case MC_32APSK_8_9_S:
        return ldpc_tab_8_9S;

    // DVB-S2X
    // FECFRAME_NORMAL
    case MC_QPSK_13_45:
        return ldpc_tab_13_45N;
    case MC_QPSK_9_20:
        return ldpc_tab_9_20N;
    case MC_QPSK_11_20:
        return ldpc_tab_11_20N;
    case MC_8APSK_5_9_L:
        return ldpc_tab_100_180N;
    case MC_8APSK_26_45_L:
        return ldpc_tab_104_180N;
    case MC_8PSK_23_36:
        return ldpc_tab_23_36N;
    case MC_8PSK_25_36:
        return ldpc_tab_25_36N;
    case MC_8PSK_13_18:
        return ldpc_tab_13_18N;
    case MC_16APSK_1_2_L:
        return ldpc_tab_90_180N;
    case MC_16APSK_8_15_L:
        return ldpc_tab_96_180N;
    case MC_16APSK_5_9_L:
        return ldpc_tab_100_180N;
    case MC_16APSK_26_45:
        return ldpc_tab_26_45N;
    case MC_16APSK_3_5:
        return ldpc_tab_3_5N;
    case MC_16APSK_3_5_L:
        return ldpc_tab_18_30N;
    case MC_16APSK_28_45:
        return ldpc_tab_28_45N;
    case MC_16APSK_23_36:
        return ldpc_tab_23_36N;
    case MC_16APSK_2_3_L:
        return ldpc_tab_20_30N;
    case MC_16APSK_25_36:
        return ldpc_tab_25_36N;
    case MC_16APSK_13_18:
        return ldpc_tab_13_18N;
    case MC_16APSK_7_9:
        return ldpc_tab_140_180N;
    case MC_16APSK_77_90:
        return ldpc_tab_154_180N;
    case MC_32APSK_2_3_L:
        return ldpc_tab_2_3N;
    case MC_32APSK_32_45:
        return ldpc_tab_128_180N;
    case MC_32APSK_11_15:
        return ldpc_tab_132_180N;
    case MC_32APSK_7_9:
        return ldpc_tab_140_180N;
    case MC_64APSK_32_45_L:
        return ldpc_tab_128_180N;
    case MC_64APSK_11_15:
        return ldpc_tab_132_180N;
    case MC_64APSK_7_9:
        return ldpc_tab_7_9N;
    case MC_64APSK_4_5:
        return ldpc_tab_4_5N;
    case MC_64APSK_5_6:
        return ldpc_tab_5_6N;
    case MC_128APSK_3_4:
        return ldpc_tab_135_180N;
    case MC_128APSK_7_9:
        return ldpc_tab_140_180N;
    case MC_256APSK_29_45_L:
        return ldpc_tab_116_180N;
    case MC_256APSK_2_3_L:
        return ldpc_tab_20_30N;
    case MC_256APSK_31_45_L:
        return ldpc_tab_124_180N;
    case MC_256APSK_32_45:
        return ldpc_tab_128_180N;
    case MC_256APSK_11_15_L:
        return ldpc_tab_22_30N;
    case MC_256APSK_3_4:
        return ldpc_tab_135_180N;
    // FECFRAME_SHORT
    case MC_QPSK_11_45_S:
        return ldpc_tab_11_45S;
    case MC_QPSK_4_15_S:
        return ldpc_tab_4_15S;
    case MC_QPSK_14_45_S:
        return ldpc_tab_14_45S;
    case MC_QPSK_7_15_S:
    case MC_8PSK_7_15_S:
    case MC_16APSK_7_15_S:
        return ldpc_tab_7_15S;
    case MC_QPSK_8_15_S:
    case MC_8PSK_8_15_S:
    case MC_16APSK_8_15_S:
        return ldpc_tab_8_15S;
    case MC_8PSK_26_45_S:
    case MC_16APSK_26_45_S:
        return ldpc_tab_26_45S;
    case MC_QPSK_32_45_S:
    case MC_8PSK_32_45_S:
    case MC_16APSK_32_45_S:
    case MC_32APSK_32_45_S:
        return ldpc_tab_32_45S;
    case MC_16APSK_3_5_S:
        return ldpc_tab_3_5S;
    case MC_32APSK_2_3_S:
        return ldpc_tab_2_3S;
    default:
        auto framesize = modcod_framesize(modcod);
        if (framesize == FECFRAME_NORMAL) {
            return { FRAME_SIZE_NORMAL, FRAME_SIZE_NORMAL, 0, 0, 0, 0, 0, {} };
        } else {
            return { FRAME_SIZE_SHORT, FRAME_SIZE_SHORT, 0, 0, 0, 0, 0, {} };
        }
    }
}

#include "ldpc_encode_tables_normal.hxx"

#include "ldpc_encode_tables_normal_x.hxx"

#include "ldpc_encode_tables_short.hxx"

#include "ldpc_encode_tables_vlsnr.hxx"

} // namespace dvbs2acm
} // namespace gr