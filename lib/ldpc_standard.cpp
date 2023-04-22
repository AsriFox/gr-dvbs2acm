#include "ldpc_standard.h"

using namespace gr::dvbs2;

ldpc_std_values::ldpc_std_values() {}

ldpc_std_values::ldpc_std_values(dvbs2_framesize_t framesize, dvbs2_code_rate_t rate)
{
    if (framesize == FECFRAME_NORMAL) {
        frame_size = frame_size_real = FRAME_SIZE_NORMAL;
        bch_code = BCH_CODE_N12;
        switch (rate) {
        case C1_4:
            kbch = 16008;
            nbch = 16200;
            q_val = 135;
            table = 0;
            break;
        case C1_3:
            kbch = 21408;
            nbch = 21600;
            q_val = 120;
            table = 1;
            break;
        case C2_5:
            kbch = 25728;
            nbch = 25920;
            q_val = 108;
            table = 2;
            break;
        case C1_2:
            kbch = 32208;
            nbch = 32400;
            q_val = 90;
            table = 3;
            break;
        case C3_5:
            kbch = 38688;
            nbch = 38880;
            q_val = 72;
            table = 4;
            break;
        case C2_3:
            kbch = 43040;
            nbch = 43200;
            q_val = 60;
            bch_code = BCH_CODE_N10;
            table = 5;
            break;
        case C3_4:
            kbch = 48408;
            nbch = 48600;
            q_val = 45;
            table = 6;
            break;
        case C4_5:
            kbch = 51648;
            nbch = 51840;
            q_val = 36;
            table = 7;
            break;
        case C5_6:
            kbch = 53840;
            nbch = 54000;
            q_val = 30;
            bch_code = BCH_CODE_N10;
            table = 8;
            break;
        case C8_9:
            kbch = 57472;
            nbch = 57600;
            q_val = 20;
            bch_code = BCH_CODE_N8;
            table = 9;
            break;
        case C9_10:
            kbch = 58192;
            nbch = 58320;
            q_val = 18;
            bch_code = BCH_CODE_N8;
            table = 10;
            break;
        case C2_9_VLSNR:
            kbch = 14208;
            nbch = 14400;
            q_val = 140;
            table = 11;
            frame_size -= NORMAL_PUNCTURING;
            frame_size_real -= NORMAL_PUNCTURING;
            P = 15;
            Xp = NORMAL_PUNCTURING;
            break;
        case C13_45:
            kbch = 18528;
            nbch = 18720;
            q_val = 128;
            table = 12;
            break;
        case C9_20:
            kbch = 28968;
            nbch = 29160;
            q_val = 99;
            table = 13;
            break;
        case C90_180:
            kbch = 32208;
            nbch = 32400;
            q_val = 90;
            table = 14;
            break;
        case C96_180:
            kbch = 34368;
            nbch = 34560;
            q_val = 84;
            table = 15;
            break;
        case C11_20:
            kbch = 35448;
            nbch = 35640;
            q_val = 81;
            table = 16;
            break;
        case C100_180:
            kbch = 35808;
            nbch = 36000;
            q_val = 80;
            table = 17;
            break;
        case C104_180:
            kbch = 37248;
            nbch = 37440;
            q_val = 76;
            table = 18;
            break;
        case C26_45:
            kbch = 37248;
            nbch = 37440;
            q_val = 76;
            table = 19;
            break;
        case C18_30:
            kbch = 38688;
            nbch = 38880;
            q_val = 72;
            table = 20;
            break;
        case C28_45:
            kbch = 40128;
            nbch = 40320;
            q_val = 68;
            table = 21;
            break;
        case C23_36:
            kbch = 41208;
            nbch = 41400;
            q_val = 65;
            table = 22;
            break;
        case C116_180:
            kbch = 41568;
            nbch = 41760;
            q_val = 64;
            table = 23;
            break;
        case C20_30:
            kbch = 43008;
            nbch = 43200;
            q_val = 60;
            table = 24;
            break;
        case C124_180:
            kbch = 44448;
            nbch = 44640;
            q_val = 56;
            table = 25;
            break;
        case C25_36:
            kbch = 44808;
            nbch = 45000;
            q_val = 55;
            table = 26;
            break;
        case C128_180:
            kbch = 45888;
            nbch = 46080;
            q_val = 52;
            table = 27;
            break;
        case C13_18:
            kbch = 46608;
            nbch = 46800;
            q_val = 50;
            table = 28;
            break;
        case C132_180:
            kbch = 47328;
            nbch = 47520;
            q_val = 48;
            table = 29;
            break;
        case C22_30:
            kbch = 47328;
            nbch = 47520;
            q_val = 48;
            table = 30;
            break;
        case C135_180:
            kbch = 48408;
            nbch = 48600;
            q_val = 45;
            table = 31;
            break;
        case C140_180:
            kbch = 50208;
            nbch = 50400;
            q_val = 40;
            table = 32;
            break;
        case C7_9:
            kbch = 50208;
            nbch = 50400;
            q_val = 40;
            table = 33;
            break;
        case C154_180:
            kbch = 55248;
            nbch = 55440;
            q_val = 26;
            table = 34;
            break;
        default:
            kbch = 0;
            nbch = 0;
            bch_code = 0;
            q_val = 0;
            break;
        }
    } else if (framesize == FECFRAME_SHORT) {
        frame_size = frame_size_real = FRAME_SIZE_SHORT;
        bch_code = BCH_CODE_S12;
        switch (rate) {
        case C1_4:
            kbch = 3072;
            nbch = 3240;
            q_val = 36;
            table = 35;
            break;
        case C1_3:
            kbch = 5232;
            nbch = 5400;
            q_val = 30;
            table = 36;
            break;
        case C2_5:
            kbch = 6312;
            nbch = 6480;
            q_val = 27;
            table = 37;
            break;
        case C1_2:
            kbch = 7032;
            nbch = 7200;
            q_val = 25;
            table = 38;
            break;
        case C3_5:
            kbch = 9552;
            nbch = 9720;
            q_val = 18;
            table = 39;
            break;
        case C2_3:
            kbch = 10632;
            nbch = 10800;
            q_val = 15;
            table = 40;
            break;
        case C3_4:
            kbch = 11712;
            nbch = 11880;
            q_val = 12;
            table = 41;
            break;
        case C4_5:
            kbch = 12432;
            nbch = 12600;
            q_val = 10;
            table = 42;
            break;
        case C5_6:
            kbch = 13152;
            nbch = 13320;
            q_val = 8;
            table = 43;
            break;
        case C8_9:
            kbch = 14232;
            nbch = 14400;
            q_val = 5;
            table = 44;
            break;
        case C11_45:
            kbch = 3792;
            nbch = 3960;
            q_val = 34;
            table = 45;
            break;
        case C4_15:
            kbch = 4152;
            nbch = 4320;
            q_val = 33;
            table = 46;
            break;
        case C14_45:
            kbch = 4872;
            nbch = 5040;
            q_val = 31;
            table = 47;
            break;
        case C7_15:
            kbch = 7392;
            nbch = 7560;
            q_val = 24;
            table = 48;
            break;
        case C8_15:
            kbch = 8472;
            nbch = 8640;
            q_val = 21;
            table = 49;
            break;
        case C26_45:
            kbch = 9192;
            nbch = 9360;
            q_val = 19;
            table = 50;
            break;
        case C32_45:
            kbch = 11352;
            nbch = 11520;
            q_val = 13;
            table = 51;
            break;
        case C1_5_VLSNR_SF2:
            kbch = 2512;
            nbch = 2680;
            q_val = 135;
            table = 35;
            frame_size -= SHORT_PUNCTURING_SET1;
            frame_size_real -= SHORT_PUNCTURING_SET1;
            Xs = 560;
            P = 30;
            Xp = 250;
            break;
        case C11_45_VLSNR_SF2:
            kbch = 3792;
            nbch = 3960;
            q_val = 34;
            table = 45;
            frame_size -= SHORT_PUNCTURING_SET1;
            frame_size_real -= SHORT_PUNCTURING_SET1;
            P = 15;
            Xp = SHORT_PUNCTURING_SET1;
            break;
        case C1_5_VLSNR:
            kbch = 3072;
            nbch = 3240;
            q_val = 135;
            table = 35;
            frame_size -= SHORT_PUNCTURING_SET2;
            frame_size_real -= SHORT_PUNCTURING_SET2;
            P = 10;
            Xp = SHORT_PUNCTURING_SET2;
            break;
        case C4_15_VLSNR:
            kbch = 4152;
            nbch = 4320;
            q_val = 33;
            table = 46;
            frame_size -= SHORT_PUNCTURING_SET2;
            frame_size_real -= SHORT_PUNCTURING_SET2;
            P = 8;
            Xp = SHORT_PUNCTURING_SET2;
            break;
        case C1_3_VLSNR:
            kbch = 5232;
            nbch = 5400;
            q_val = 120;
            table = 36;
            frame_size -= SHORT_PUNCTURING_SET2;
            frame_size_real -= SHORT_PUNCTURING_SET2;
            P = 8;
            Xp = SHORT_PUNCTURING_SET2;
            break;
        default:
            kbch = 0;
            nbch = 0;
            q_val = 0;
            bch_code = 0;
            break;
        }
    } else {
        frame_size = frame_size_real = FRAME_SIZE_MEDIUM - MEDIUM_PUNCTURING;
        bch_code = BCH_CODE_M12;
        switch (rate) {
        case C1_5_MEDIUM:
            kbch = 5660;
            nbch = 5840;
            q_val = 72;
            table = 52;
            Xs = 640;
            P = 25;
            Xp = 980;
            break;
        case C11_45_MEDIUM:
            kbch = 7740;
            nbch = 7920;
            q_val = 68;
            table = 53;
            P = 15;
            Xp = MEDIUM_PUNCTURING;
            break;
        case C1_3_MEDIUM:
            kbch = 10620;
            nbch = 10800;
            q_val = 60;
            table = 54;
            P = 13;
            Xp = MEDIUM_PUNCTURING;
            break;
        default:
            kbch = 0;
            nbch = 0;
            q_val = 0;
            bch_code = 0;
            break;
        }
    }
}

ldpc_std_values ldpc_std_values::from_tag(uint64_t value)
{
    auto framesize = (dvbs2_framesize_t)((value >> 1) & 0x7f);
    auto rate = (dvbs2_code_rate_t)((value >> 8) & 0xff);
    return ldpc_std_values(framesize, rate);
}