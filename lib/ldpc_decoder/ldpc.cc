#include "dvb_s2_tables.hh"
#include "dvb_s2x_tables.hh"
#include "ldpc.hh"

using namespace gr::dvbs2acm;

[[nodiscard]] LDPCInterface* build_decoder(dvbs2_framesize_t framesize, dvbs2_code_rate_t rate)
{
    if (framesize == FECFRAME_NORMAL) {
        switch (rate) {
        case C1_4:
            return new LDPC<DVB_S2_TABLE_B1>();
        case C1_3:
            return new LDPC<DVB_S2_TABLE_B2>();
        case C2_5:
            return new LDPC<DVB_S2_TABLE_B3>();
        case C1_2:
            return new LDPC<DVB_S2_TABLE_B4>();
        case C3_5:
            return new LDPC<DVB_S2_TABLE_B5>();
        case C2_3:
            return new LDPC<DVB_S2_TABLE_B6>();
        case C3_4:
            return new LDPC<DVB_S2_TABLE_B7>();
        case C4_5:
            return new LDPC<DVB_S2_TABLE_B8>();
        case C5_6:
            return new LDPC<DVB_S2_TABLE_B9>();
        case C8_9:
            return new LDPC<DVB_S2_TABLE_B10>();
        case C9_10:
            return new LDPC<DVB_S2_TABLE_B11>();
        case C2_9_VLSNR:
            return new LDPC<DVB_S2X_TABLE_B1>();
        case C13_45:
            return new LDPC<DVB_S2X_TABLE_B2>();
        case C9_20:
            return new LDPC<DVB_S2X_TABLE_B3>();
        case C90_180:
            return new LDPC<DVB_S2X_TABLE_B11>();
        case C96_180:
            return new LDPC<DVB_S2X_TABLE_B12>();
        case C11_20:
            return new LDPC<DVB_S2X_TABLE_B4>();
        case C100_180:
            return new LDPC<DVB_S2X_TABLE_B13>();
        case C104_180:
            return new LDPC<DVB_S2X_TABLE_B14>();
        case C26_45:
            return new LDPC<DVB_S2X_TABLE_B5>();
        case C18_30:
            return new LDPC<DVB_S2X_TABLE_B22>();
        case C28_45:
            return new LDPC<DVB_S2X_TABLE_B6>();
        case C23_36:
            return new LDPC<DVB_S2X_TABLE_B7>();
        case C116_180:
            return new LDPC<DVB_S2X_TABLE_B15>();
        case C20_30:
            return new LDPC<DVB_S2X_TABLE_B23>();
        case C124_180:
            return new LDPC<DVB_S2X_TABLE_B16>();
        case C25_36:
            return new LDPC<DVB_S2X_TABLE_B8>();
        case C128_180:
            return new LDPC<DVB_S2X_TABLE_B17>();
        case C13_18:
            return new LDPC<DVB_S2X_TABLE_B9>();
        case C132_180:
            return new LDPC<DVB_S2X_TABLE_B18>();
        case C22_30:
            return new LDPC<DVB_S2X_TABLE_B24>();
        case C135_180:
            return new LDPC<DVB_S2X_TABLE_B19>();
        case C140_180:
            return new LDPC<DVB_S2X_TABLE_B20>();
        case C7_9:
            return new LDPC<DVB_S2X_TABLE_B10>();
        case C154_180:
            return new LDPC<DVB_S2X_TABLE_B21>();
        default:
            return nullptr;
        }
    } else if (framesize == FECFRAME_SHORT) {
        switch (rate) {
        case C1_4:
            return new LDPC<DVB_S2_TABLE_C1>();
        case C1_3:
            return new LDPC<DVB_S2_TABLE_C2>();
        case C2_5:
            return new LDPC<DVB_S2_TABLE_C3>();
        case C1_2:
            return new LDPC<DVB_S2_TABLE_C4>();
        case C3_5:
            return new LDPC<DVB_S2_TABLE_C5>();
        case C2_3:
            return new LDPC<DVB_S2_TABLE_C6>();
        case C3_4:
            return new LDPC<DVB_S2_TABLE_C7>();
        case C4_5:
            return new LDPC<DVB_S2_TABLE_C8>();
        case C5_6:
            return new LDPC<DVB_S2_TABLE_C9>();
        case C8_9:
            return new LDPC<DVB_S2_TABLE_C10>();
        case C11_45:
            return new LDPC<DVB_S2X_TABLE_C1>();
        case C4_15:
            return new LDPC<DVB_S2X_TABLE_C2>();
        case C14_45:
            return new LDPC<DVB_S2X_TABLE_C3>();
        case C7_15:
            return new LDPC<DVB_S2X_TABLE_C4>();
        case C8_15:
            return new LDPC<DVB_S2X_TABLE_C5>();
        case C26_45:
            return new LDPC<DVB_S2X_TABLE_C6>();
        case C32_45:
            return new LDPC<DVB_S2X_TABLE_C7>();
        case C1_5_VLSNR_SF2:
            return new LDPC<DVB_S2_TABLE_C1>();
        case C11_45_VLSNR_SF2:
            return new LDPC<DVB_S2X_TABLE_C1>();
        case C1_5_VLSNR:
            return new LDPC<DVB_S2_TABLE_C1>();
        case C4_15_VLSNR:
            return new LDPC<DVB_S2X_TABLE_C2>();
        case C1_3_VLSNR:
            return new LDPC<DVB_S2_TABLE_C2>();
        default:
            return nullptr;
        }
    } else {
        return nullptr;
        switch (rate) {
        case C1_5_MEDIUM:
            return new LDPC<DVB_S2X_TABLE_C8>();
        case C11_45_MEDIUM:
            return new LDPC<DVB_S2X_TABLE_C9>();
        case C1_3_MEDIUM:
            return new LDPC<DVB_S2X_TABLE_C10>();
        default:
            return nullptr;
        }
    }
}