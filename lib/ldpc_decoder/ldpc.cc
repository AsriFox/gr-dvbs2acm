#include "dvb_s2_tables.hh"
#include "dvb_s2x_tables.hh"
#include "ldpc.hh"

using namespace gr::dvbs2acm;

constexpr int DVB_S2_TABLE_B1::DEG[];
constexpr int DVB_S2_TABLE_B1::LEN[];
constexpr int DVB_S2_TABLE_B1::POS[];

constexpr int DVB_S2_TABLE_B2::DEG[];
constexpr int DVB_S2_TABLE_B2::LEN[];
constexpr int DVB_S2_TABLE_B2::POS[];

constexpr int DVB_S2_TABLE_B3::DEG[];
constexpr int DVB_S2_TABLE_B3::LEN[];
constexpr int DVB_S2_TABLE_B3::POS[];

constexpr int DVB_S2_TABLE_B4::DEG[];
constexpr int DVB_S2_TABLE_B4::LEN[];
constexpr int DVB_S2_TABLE_B4::POS[];

constexpr int DVB_S2_TABLE_B5::DEG[];
constexpr int DVB_S2_TABLE_B5::LEN[];
constexpr int DVB_S2_TABLE_B5::POS[];

constexpr int DVB_S2_TABLE_B6::DEG[];
constexpr int DVB_S2_TABLE_B6::LEN[];
constexpr int DVB_S2_TABLE_B6::POS[];

constexpr int DVB_S2_TABLE_B7::DEG[];
constexpr int DVB_S2_TABLE_B7::LEN[];
constexpr int DVB_S2_TABLE_B7::POS[];

constexpr int DVB_S2_TABLE_B8::DEG[];
constexpr int DVB_S2_TABLE_B8::LEN[];
constexpr int DVB_S2_TABLE_B8::POS[];

constexpr int DVB_S2_TABLE_B9::DEG[];
constexpr int DVB_S2_TABLE_B9::LEN[];
constexpr int DVB_S2_TABLE_B9::POS[];

constexpr int DVB_S2_TABLE_B10::DEG[];
constexpr int DVB_S2_TABLE_B10::LEN[];
constexpr int DVB_S2_TABLE_B10::POS[];

constexpr int DVB_S2_TABLE_B11::DEG[];
constexpr int DVB_S2_TABLE_B11::LEN[];
constexpr int DVB_S2_TABLE_B11::POS[];

constexpr int DVB_S2X_TABLE_B1::DEG[];
constexpr int DVB_S2X_TABLE_B1::LEN[];
constexpr int DVB_S2X_TABLE_B1::POS[];

constexpr int DVB_S2X_TABLE_B2::DEG[];
constexpr int DVB_S2X_TABLE_B2::LEN[];
constexpr int DVB_S2X_TABLE_B2::POS[];

constexpr int DVB_S2X_TABLE_B3::DEG[];
constexpr int DVB_S2X_TABLE_B3::LEN[];
constexpr int DVB_S2X_TABLE_B3::POS[];

constexpr int DVB_S2X_TABLE_B4::DEG[];
constexpr int DVB_S2X_TABLE_B4::LEN[];
constexpr int DVB_S2X_TABLE_B4::POS[];

constexpr int DVB_S2X_TABLE_B5::DEG[];
constexpr int DVB_S2X_TABLE_B5::LEN[];
constexpr int DVB_S2X_TABLE_B5::POS[];

constexpr int DVB_S2X_TABLE_B6::DEG[];
constexpr int DVB_S2X_TABLE_B6::LEN[];
constexpr int DVB_S2X_TABLE_B6::POS[];

constexpr int DVB_S2X_TABLE_B7::DEG[];
constexpr int DVB_S2X_TABLE_B7::LEN[];
constexpr int DVB_S2X_TABLE_B7::POS[];

constexpr int DVB_S2X_TABLE_B8::DEG[];
constexpr int DVB_S2X_TABLE_B8::LEN[];
constexpr int DVB_S2X_TABLE_B8::POS[];

constexpr int DVB_S2X_TABLE_B9::DEG[];
constexpr int DVB_S2X_TABLE_B9::LEN[];
constexpr int DVB_S2X_TABLE_B9::POS[];

constexpr int DVB_S2X_TABLE_B10::DEG[];
constexpr int DVB_S2X_TABLE_B10::LEN[];
constexpr int DVB_S2X_TABLE_B10::POS[];

constexpr int DVB_S2X_TABLE_B11::DEG[];
constexpr int DVB_S2X_TABLE_B11::LEN[];
constexpr int DVB_S2X_TABLE_B11::POS[];

constexpr int DVB_S2X_TABLE_B12::DEG[];
constexpr int DVB_S2X_TABLE_B12::LEN[];
constexpr int DVB_S2X_TABLE_B12::POS[];

constexpr int DVB_S2X_TABLE_B13::DEG[];
constexpr int DVB_S2X_TABLE_B13::LEN[];
constexpr int DVB_S2X_TABLE_B13::POS[];

constexpr int DVB_S2X_TABLE_B14::DEG[];
constexpr int DVB_S2X_TABLE_B14::LEN[];
constexpr int DVB_S2X_TABLE_B14::POS[];

constexpr int DVB_S2X_TABLE_B15::DEG[];
constexpr int DVB_S2X_TABLE_B15::LEN[];
constexpr int DVB_S2X_TABLE_B15::POS[];

constexpr int DVB_S2X_TABLE_B16::DEG[];
constexpr int DVB_S2X_TABLE_B16::LEN[];
constexpr int DVB_S2X_TABLE_B16::POS[];

constexpr int DVB_S2X_TABLE_B17::DEG[];
constexpr int DVB_S2X_TABLE_B17::LEN[];
constexpr int DVB_S2X_TABLE_B17::POS[];

constexpr int DVB_S2X_TABLE_B18::DEG[];
constexpr int DVB_S2X_TABLE_B18::LEN[];
constexpr int DVB_S2X_TABLE_B18::POS[];

constexpr int DVB_S2X_TABLE_B19::DEG[];
constexpr int DVB_S2X_TABLE_B19::LEN[];
constexpr int DVB_S2X_TABLE_B19::POS[];

constexpr int DVB_S2X_TABLE_B20::DEG[];
constexpr int DVB_S2X_TABLE_B20::LEN[];
constexpr int DVB_S2X_TABLE_B20::POS[];

constexpr int DVB_S2X_TABLE_B21::DEG[];
constexpr int DVB_S2X_TABLE_B21::LEN[];
constexpr int DVB_S2X_TABLE_B21::POS[];

constexpr int DVB_S2X_TABLE_B22::DEG[];
constexpr int DVB_S2X_TABLE_B22::LEN[];
constexpr int DVB_S2X_TABLE_B22::POS[];

constexpr int DVB_S2X_TABLE_B23::DEG[];
constexpr int DVB_S2X_TABLE_B23::LEN[];
constexpr int DVB_S2X_TABLE_B23::POS[];

constexpr int DVB_S2X_TABLE_B24::DEG[];
constexpr int DVB_S2X_TABLE_B24::LEN[];
constexpr int DVB_S2X_TABLE_B24::POS[];

constexpr int DVB_S2_TABLE_C1::DEG[];
constexpr int DVB_S2_TABLE_C1::LEN[];
constexpr int DVB_S2_TABLE_C1::POS[];

constexpr int DVB_S2_TABLE_C2::DEG[];
constexpr int DVB_S2_TABLE_C2::LEN[];
constexpr int DVB_S2_TABLE_C2::POS[];

constexpr int DVB_S2_TABLE_C3::DEG[];
constexpr int DVB_S2_TABLE_C3::LEN[];
constexpr int DVB_S2_TABLE_C3::POS[];

constexpr int DVB_S2_TABLE_C4::DEG[];
constexpr int DVB_S2_TABLE_C4::LEN[];
constexpr int DVB_S2_TABLE_C4::POS[];

constexpr int DVB_S2_TABLE_C5::DEG[];
constexpr int DVB_S2_TABLE_C5::LEN[];
constexpr int DVB_S2_TABLE_C5::POS[];

constexpr int DVB_S2_TABLE_C6::DEG[];
constexpr int DVB_S2_TABLE_C6::LEN[];
constexpr int DVB_S2_TABLE_C6::POS[];

constexpr int DVB_S2_TABLE_C7::DEG[];
constexpr int DVB_S2_TABLE_C7::LEN[];
constexpr int DVB_S2_TABLE_C7::POS[];

constexpr int DVB_S2_TABLE_C8::DEG[];
constexpr int DVB_S2_TABLE_C8::LEN[];
constexpr int DVB_S2_TABLE_C8::POS[];

constexpr int DVB_S2_TABLE_C9::DEG[];
constexpr int DVB_S2_TABLE_C9::LEN[];
constexpr int DVB_S2_TABLE_C9::POS[];

constexpr int DVB_S2_TABLE_C10::DEG[];
constexpr int DVB_S2_TABLE_C10::LEN[];
constexpr int DVB_S2_TABLE_C10::POS[];

constexpr int DVB_S2X_TABLE_C1::DEG[];
constexpr int DVB_S2X_TABLE_C1::LEN[];
constexpr int DVB_S2X_TABLE_C1::POS[];

constexpr int DVB_S2X_TABLE_C2::DEG[];
constexpr int DVB_S2X_TABLE_C2::LEN[];
constexpr int DVB_S2X_TABLE_C2::POS[];

constexpr int DVB_S2X_TABLE_C3::DEG[];
constexpr int DVB_S2X_TABLE_C3::LEN[];
constexpr int DVB_S2X_TABLE_C3::POS[];

constexpr int DVB_S2X_TABLE_C4::DEG[];
constexpr int DVB_S2X_TABLE_C4::LEN[];
constexpr int DVB_S2X_TABLE_C4::POS[];

constexpr int DVB_S2X_TABLE_C5::DEG[];
constexpr int DVB_S2X_TABLE_C5::LEN[];
constexpr int DVB_S2X_TABLE_C5::POS[];

constexpr int DVB_S2X_TABLE_C6::DEG[];
constexpr int DVB_S2X_TABLE_C6::LEN[];
constexpr int DVB_S2X_TABLE_C6::POS[];

constexpr int DVB_S2X_TABLE_C7::DEG[];
constexpr int DVB_S2X_TABLE_C7::LEN[];
constexpr int DVB_S2X_TABLE_C7::POS[];

constexpr int DVB_S2X_TABLE_C8::DEG[];
constexpr int DVB_S2X_TABLE_C8::LEN[];
constexpr int DVB_S2X_TABLE_C8::POS[];

constexpr int DVB_S2X_TABLE_C9::DEG[];
constexpr int DVB_S2X_TABLE_C9::LEN[];
constexpr int DVB_S2X_TABLE_C9::POS[];

constexpr int DVB_S2X_TABLE_C10::DEG[];
constexpr int DVB_S2X_TABLE_C10::LEN[];
constexpr int DVB_S2X_TABLE_C10::POS[];

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
        // case C2_9_VLSNR:
        //     nbch = 14400;
        //     ldpc = new LDPC<DVB_S2X_TABLE_B1>();
        //     break;
        // case C13_45:
        //     nbch = 18720;
        //     ldpc = new LDPC<DVB_S2X_TABLE_B2>();
        //     break;
        // case C9_20:
        //     nbch = 29160;
        //     ldpc = new LDPC<DVB_S2X_TABLE_B3>();
        //     break;
        // case C90_180:
        //     nbch = 32400;
        //     ldpc = new LDPC<DVB_S2X_TABLE_B11>();
        //     break;
        // case C96_180:
        //     nbch = 34560;
        //     ldpc = new LDPC<DVB_S2X_TABLE_B12>();
        //     break;
        // case C11_20:
        //     nbch = 35640;
        //     ldpc = new LDPC<DVB_S2X_TABLE_B4>();
        //     break;
        // case C100_180:
        //     nbch = 36000;
        //     ldpc = new LDPC<DVB_S2X_TABLE_B13>();
        //     break;
        // case C104_180:
        //     nbch = 37440;
        //     ldpc = new LDPC<DVB_S2X_TABLE_B14>();
        //     break;
        // case C26_45:
        //     nbch = 37440;
        //     ldpc = new LDPC<DVB_S2X_TABLE_B5>();
        //     break;
        // case C18_30:
        //     nbch = 38880;
        //     ldpc = new LDPC<DVB_S2X_TABLE_B22>();
        //     break;
        // case C28_45:
        //     nbch = 40320;
        //     ldpc = new LDPC<DVB_S2X_TABLE_B6>();
        //     break;
        // case C23_36:
        //     nbch = 41400;
        //     ldpc = new LDPC<DVB_S2X_TABLE_B7>();
        //     break;
        // case C116_180:
        //     nbch = 41760;
        //     ldpc = new LDPC<DVB_S2X_TABLE_B15>();
        //     break;
        // case C20_30:
        //     nbch = 43200;
        //     ldpc = new LDPC<DVB_S2X_TABLE_B23>();
        //     break;
        // case C124_180:
        //     nbch = 44640;
        //     ldpc = new LDPC<DVB_S2X_TABLE_B16>();
        //     break;
        // case C25_36:
        //     nbch = 45000;
        //     ldpc = new LDPC<DVB_S2X_TABLE_B8>();
        //     break;
        // case C128_180:
        //     nbch = 46080;
        //     ldpc = new LDPC<DVB_S2X_TABLE_B17>();
        //     break;
        // case C13_18:
        //     nbch = 46800;
        //     ldpc = new LDPC<DVB_S2X_TABLE_B9>();
        //     break;
        // case C132_180:
        //     nbch = 47520;
        //     ldpc = new LDPC<DVB_S2X_TABLE_B18>();
        //     break;
        // case C22_30:
        //     nbch = 47520;
        //     ldpc = new LDPC<DVB_S2X_TABLE_B24>();
        //     break;
        // case C135_180:
        //     nbch = 48600;
        //     ldpc = new LDPC<DVB_S2X_TABLE_B19>();
        //     break;
        // case C140_180:
        //     nbch = 50400;
        //     ldpc = new LDPC<DVB_S2X_TABLE_B20>();
        //     break;
        // case C7_9:
        //     nbch = 50400;
        //     ldpc = new LDPC<DVB_S2X_TABLE_B10>();
        //     break;
        // case C154_180:
        //     nbch = 55440;
        //     ldpc = new LDPC<DVB_S2X_TABLE_B21>();
        //     break;
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
        // case C11_45:
        //     nbch = 3960;
        //     ldpc = new LDPC<DVB_S2X_TABLE_C1>();
        //     break;
        // case C4_15:
        //     nbch = 4320;
        //     ldpc = new LDPC<DVB_S2X_TABLE_C2>();
        //     break;
        // case C14_45:
        //     nbch = 5040;
        //     ldpc = new LDPC<DVB_S2X_TABLE_C3>();
        //     break;
        // case C7_15:
        //     nbch = 7560;
        //     ldpc = new LDPC<DVB_S2X_TABLE_C4>();
        //     break;
        // case C8_15:
        //     nbch = 8640;
        //     ldpc = new LDPC<DVB_S2X_TABLE_C5>();
        //     break;
        // case C26_45:
        //     nbch = 9360;
        //     ldpc = new LDPC<DVB_S2X_TABLE_C6>();
        //     break;
        // case C32_45:
        //     nbch = 11520;
        //     ldpc = new LDPC<DVB_S2X_TABLE_C7>();
        //     break;
        // case C1_5_VLSNR_SF2:
        //     nbch = 2680;
        //     ldpc = new LDPC<DVB_S2_TABLE_C1>();
        //     break;
        // case C11_45_VLSNR_SF2:
        //     nbch = 3960;
        //     ldpc = new LDPC<DVB_S2X_TABLE_C1>();
        //     break;
        // case C1_5_VLSNR:
        //     nbch = 3240;
        //     ldpc = new LDPC<DVB_S2_TABLE_C1>();
        //     break;
        // case C4_15_VLSNR:
        //     nbch = 4320;
        //     ldpc = new LDPC<DVB_S2X_TABLE_C2>();
        //     break;
        // case C1_3_VLSNR:
        //     nbch = 5400;
        //     ldpc = new LDPC<DVB_S2_TABLE_C2>();
        //     break;
        default:
            return nullptr;
        }
    } else {
        return nullptr;
        // switch (rate) {
        // case C1_5_MEDIUM:
        //     nbch = 5840;
        //     ldpc = new LDPC<DVB_S2X_TABLE_C8>();
        //     break;
        // case C11_45_MEDIUM:
        //     nbch = 7920;
        //     ldpc = new LDPC<DVB_S2X_TABLE_C9>();
        //     break;
        // case C1_3_MEDIUM:
        //     nbch = 10800;
        //     ldpc = new LDPC<DVB_S2X_TABLE_C10>();
        //     break;
        // default:
        //     nbch = 0;
        //     break;
        // }
    }
}