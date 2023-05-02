#include "bch_code.h"
#include "modcod.hh"
#include <cstring>

namespace gr {
namespace dvbs2acm {

/*
 * Polynomial calculation routines
 * multiply polynomials
 */
int poly_mult(const int* ina, int lena, const int* inb, int lenb, int* out)
{
    memset(out, 0, sizeof(int) * (lena + lenb));

    for (int i = 0; i < lena; i++) {
        for (int j = 0; j < lenb; j++) {
            if (ina[i] * inb[j] > 0) {
                out[i + j]++; // count number of terms for this pwr of x
            }
        }
    }
    int max = 0;
    for (int i = 0; i < lena + lenb; i++) {
        out[i] = out[i] & 1; // If even ignore the term
        if (out[i]) {
            max = i;
        }
    }
    // return the size of array to house the result.
    return max + 1;
}

/*
 * Pack the polynomial into a 32 bit array
 */
void poly_pack(const int* pin, unsigned int* pout, int len)
{
    int lw = len / 32;
    int ptr = 0;
    unsigned int temp;
    if (len % 32) {
        lw++;
    }

    for (int i = 0; i < lw; i++) {
        temp = 0x80000000;
        pout[i] = 0;
        for (int j = 0; j < 32; j++) {
            if (pin[ptr++]) {
                pout[i] |= temp;
            }
            temp >>= 1;
        }
    }
}

void poly_reverse(int* pin, int* pout, int len)
{
    int c;
    c = len - 1;

    for (int i = 0; i < len; i++) {
        pout[c--] = pin[i];
    }
}

bch_code::bch_code(unsigned int code) : code(code)
{
    switch (code) {
    case BCH_CODE_N8:
    case BCH_CODE_N10:
    case BCH_CODE_N12:
    case BCH_CODE_S12:
    case BCH_CODE_M12:
        break;
    default:
        return;
    }

    // Normal polynomials
    const int polyn01[] = { 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
    const int polyn02[] = { 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 };
    const int polyn03[] = { 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1 };
    const int polyn04[] = { 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1 };
    const int polyn05[] = { 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1 };
    const int polyn06[] = { 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1 };
    const int polyn07[] = { 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1 };
    const int polyn08[] = { 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1 };
    const int polyn09[] = { 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1 };
    const int polyn10[] = { 1, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1 };
    const int polyn11[] = { 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1 };
    const int polyn12[] = { 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1 };

    // Medium polynomials
    const int polym01[] = { 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
    const int polym02[] = { 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1 };
    const int polym03[] = { 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1 };
    const int polym04[] = { 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1 };
    const int polym05[] = { 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1 };
    const int polym06[] = { 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1 };
    const int polym07[] = { 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1 };
    const int polym08[] = { 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1 };
    const int polym09[] = { 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1 };
    const int polym10[] = { 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1 };
    const int polym11[] = { 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1 };
    const int polym12[] = { 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1 };

    // Short polynomials
    const int polys01[] = { 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
    const int polys02[] = { 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1 };
    const int polys03[] = { 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1 };
    const int polys04[] = { 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1 };
    const int polys05[] = { 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1 };
    const int polys06[] = { 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1 };
    const int polys07[] = { 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1 };
    const int polys08[] = { 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1 };
    const int polys09[] = { 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1 };
    const int polys10[] = { 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1 };
    const int polys11[] = { 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1 };
    const int polys12[] = { 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1 };

    int len;
    int polyout[2][200];
    if (code == BCH_CODE_S12) {
        len = poly_mult(polys01, 15, polys02, 15, polyout[0]);
        len = poly_mult(polys03, 15, polyout[0], len, polyout[1]);
        len = poly_mult(polys04, 15, polyout[1], len, polyout[0]);
        len = poly_mult(polys05, 15, polyout[0], len, polyout[1]);
        len = poly_mult(polys06, 15, polyout[1], len, polyout[0]);
        len = poly_mult(polys07, 15, polyout[0], len, polyout[1]);
        len = poly_mult(polys08, 15, polyout[1], len, polyout[0]);
        len = poly_mult(polys09, 15, polyout[0], len, polyout[1]);
        len = poly_mult(polys10, 15, polyout[1], len, polyout[0]);
        len = poly_mult(polys11, 15, polyout[0], len, polyout[1]);
        len = poly_mult(polys12, 15, polyout[1], len, polyout[0]);
        poly_pack(polyout[0], m_poly, 168);
    } else if (code == BCH_CODE_M12) {
        len = poly_mult(polym01, 16, polym02, 16, polyout[0]);
        len = poly_mult(polym03, 16, polyout[0], len, polyout[1]);
        len = poly_mult(polym04, 16, polyout[1], len, polyout[0]);
        len = poly_mult(polym05, 16, polyout[0], len, polyout[1]);
        len = poly_mult(polym06, 16, polyout[1], len, polyout[0]);
        len = poly_mult(polym07, 16, polyout[0], len, polyout[1]);
        len = poly_mult(polym08, 16, polyout[1], len, polyout[0]);
        len = poly_mult(polym09, 16, polyout[0], len, polyout[1]);
        len = poly_mult(polym10, 16, polyout[1], len, polyout[0]);
        len = poly_mult(polym11, 16, polyout[0], len, polyout[1]);
        len = poly_mult(polym12, 16, polyout[1], len, polyout[0]);
        poly_pack(polyout[0], m_poly, 180);
    } else {
        len = poly_mult(polyn01, 17, polyn02, 17, polyout[0]);
        len = poly_mult(polyn03, 17, polyout[0], len, polyout[1]);
        len = poly_mult(polyn04, 17, polyout[1], len, polyout[0]);
        len = poly_mult(polyn05, 17, polyout[0], len, polyout[1]);
        len = poly_mult(polyn06, 17, polyout[1], len, polyout[0]);
        len = poly_mult(polyn07, 17, polyout[0], len, polyout[1]);
        len = poly_mult(polyn08, 17, polyout[1], len, polyout[0]);
        if (code == BCH_CODE_N8) {
            poly_pack(polyout[0], m_poly, 128);
            return;
        }

        len = poly_mult(polyn09, 17, polyout[0], len, polyout[1]);
        len = poly_mult(polyn10, 17, polyout[1], len, polyout[0]);
        if (code == BCH_CODE_N10) {
            poly_pack(polyout[0], m_poly, 160);
            return;
        }

        len = poly_mult(polyn11, 17, polyout[0], len, polyout[1]);
        len = poly_mult(polyn12, 17, polyout[1], len, polyout[0]);
        // if (code == BCH_CODE_N12)
        poly_pack(polyout[0], m_poly, 192);
    }
}

void bch_code::reset()
{
    memset(shift, 0, sizeof(unsigned int) * 6);
    // for (unsigned int t = 0; t < 6; t++) {
    //     shift[t] = 0;
    // }
    // shift[0] = 0;
    // shift[1] = 0;
    // shift[2] = 0;
    // shift[3] = 0;
    // shift[4] = 0;
    // shift[5] = 0;
}

void bch_code::reg_shift_6()
{
    shift[5] = (shift[5] >> 1) | (shift[4] << 31);
    shift[4] = (shift[4] >> 1) | (shift[3] << 31);
    shift[3] = (shift[3] >> 1) | (shift[2] << 31);
    shift[2] = (shift[2] >> 1) | (shift[1] << 31);
    shift[1] = (shift[1] >> 1) | (shift[0] << 31);
    shift[0] = (shift[0] >> 1);
}

void bch_code::reg_shift_5()
{
    shift[4] = (shift[4] >> 1) | (shift[3] << 31);
    shift[3] = (shift[3] >> 1) | (shift[2] << 31);
    shift[2] = (shift[2] >> 1) | (shift[1] << 31);
    shift[1] = (shift[1] >> 1) | (shift[0] << 31);
    shift[0] = (shift[0] >> 1);
}

void bch_code::reg_shift_4()
{
    shift[3] = (shift[3] >> 1) | (shift[2] << 31);
    shift[2] = (shift[2] >> 1) | (shift[1] << 31);
    shift[1] = (shift[1] >> 1) | (shift[0] << 31);
    shift[0] = (shift[0] >> 1);
}

void bch_code::encode(const unsigned char* in, unsigned char* out)
{
    reset();
    unsigned b;
    switch (code) {
    case BCH_CODE_N12:
        for (unsigned j = 0; j < kbch; j++) {
            out[j] = in[j];
            b = out[j] ^ (shift[5] & 1);
            reg_shift_6();
            if (b) {
                shift[0] ^= m_poly[0];
                shift[1] ^= m_poly[1];
                shift[2] ^= m_poly[2];
                shift[3] ^= m_poly[3];
                shift[4] ^= m_poly[4];
                shift[5] ^= m_poly[5];
            }
        }
        for (int n = kbch; n < kbch + 192; n++) {
            out[n] = shift[5] & 1;
            reg_shift_6();
        }
        break;
    case BCH_CODE_N10:
        for (unsigned j = 0; j < kbch; j++) {
            out[j] = in[j];
            b = out[j] ^ (shift[4] & 1);
            reg_shift_5();
            if (b) {
                shift[0] ^= m_poly[0];
                shift[1] ^= m_poly[1];
                shift[2] ^= m_poly[2];
                shift[3] ^= m_poly[3];
                shift[4] ^= m_poly[4];
            }
        }
        for (int n = kbch; n < kbch + 160; n++) {
            out[n] = shift[4] & 1;
            reg_shift_6();
        }
        break;
    case BCH_CODE_N8:
        for (unsigned j = 0; j < kbch; j++) {
            out[j] = in[j];
            b = out[j] ^ (shift[3] & 1);
            reg_shift_5();
            if (b) {
                shift[0] ^= m_poly[0];
                shift[1] ^= m_poly[1];
                shift[2] ^= m_poly[2];
                shift[3] ^= m_poly[3];
            }
        }
        for (int n = kbch; n < kbch + 128; n++) {
            out[n] = shift[3] & 1;
            reg_shift_4();
        }
        break;
    case BCH_CODE_S12:
        for (unsigned j = 0; j < kbch; j++) {
            out[j] = in[j];
            b = out[j] ^ ((shift[5] & 0x01000000) ? 1 : 0);
            reg_shift_6();
            if (b) {
                shift[0] ^= m_poly[0];
                shift[1] ^= m_poly[1];
                shift[2] ^= m_poly[2];
                shift[3] ^= m_poly[3];
                shift[4] ^= m_poly[4];
                shift[5] ^= m_poly[5];
            }
        }
        for (int n = kbch; n < kbch + 168; n++) {
            out[n] = (shift[5] & 0x01000000) ? 1 : 0;
            reg_shift_6();
        }
        break;
    case BCH_CODE_M12:
        for (unsigned j = 0; j < kbch; j++) {
            out[j] = in[j];
            b = out[j] ^ ((shift[5] & 0x00001000) ? 1 : 0);
            reg_shift_6();
            if (b) {
                shift[0] ^= m_poly[0];
                shift[1] ^= m_poly[1];
                shift[2] ^= m_poly[2];
                shift[3] ^= m_poly[3];
                shift[4] ^= m_poly[4];
                shift[5] ^= m_poly[5];
            }
        }
        for (int n = kbch; n < kbch + 180; n++) {
            out[n] = (shift[5] & 0x00001000) ? 1 : 0;
            reg_shift_6();
        }
        break;
    }
}

#define BCH_N12(KBCH, NBCH)    \
    {                          \
        auto b = bch_code_N12; \
        b.kbch = KBCH;         \
        b.nbch = NBCH;         \
        return b;              \
    }

#define BCH_N10(KBCH, NBCH)    \
    {                          \
        auto b = bch_code_N10; \
        b.kbch = KBCH;         \
        b.nbch = NBCH;         \
        return b;              \
    }

#define BCH_N8(KBCH, NBCH)    \
    {                         \
        auto b = bch_code_N8; \
        b.kbch = KBCH;        \
        b.nbch = NBCH;        \
        return b;             \
    }

#define BCH_S12(KBCH, NBCH)    \
    {                          \
        auto b = bch_code_S12; \
        b.kbch = KBCH;         \
        b.nbch = NBCH;         \
        return b;              \
    }

#define BCH_M12(KBCH, NBCH)    \
    {                          \
        auto b = bch_code_M12; \
        b.kbch = KBCH;         \
        b.nbch = NBCH;         \
        return b;              \
    }

bch_code bch_code::select(dvbs2_modcod_t modcod, dvbs2_vlsnr_header_t vlsnr_header)
{
    if (modcod == MC_VLSNR_SET1 || modcod == MC_VLSNR_SET2) {
        return select_vlsnr(vlsnr_header);
    }
    auto rate = modcod_rate(modcod);
    if ((modcod < 64 && (bool)(modcod & 1)) || (modcod >= MC_QPSK_11_45_S)) {
        return select_short(rate);
    } else {
        return select_normal(rate);
    }
}

bch_code bch_code::select_normal(dvbs2_code_rate_t rate)
{
    switch (rate) {
    case C1_4:
        BCH_N12(16008, 16200)
    case C1_3:
        BCH_N12(21408, 21600)
    case C2_5:
        BCH_N12(25728, 25920)
    case C1_2:
        BCH_N12(32208, 32400)
    case C3_5:
        BCH_N12(38688, 38880)
    case C2_3:
        BCH_N10(43040, 43200)
    case C3_4:
        BCH_N12(48408, 48600)
    case C4_5:
        BCH_N12(51648, 51840)
    case C5_6:
        BCH_N10(53840, 54000)
    case C8_9:
        BCH_N8(57472, 57600)
    case C9_10:
        BCH_N8(58192, 58320)
    case C13_45:
        BCH_N12(18528, 18720)
    case C9_20:
        BCH_N12(28968, 29160)
    case C90_180:
        BCH_N12(32208, 32400)
    case C96_180:
        BCH_N12(34368, 34560)
    case C11_20:
        BCH_N12(35448, 35640)
    case C100_180:
        BCH_N12(35808, 36000)
    case C104_180:
        BCH_N12(37248, 37440)
    case C26_45:
        BCH_N12(37248, 37440)
    case C18_30:
        BCH_N12(38688, 38880)
    case C28_45:
        BCH_N12(40128, 40320)
    case C23_36:
        BCH_N12(41208, 41400)
    case C116_180:
        BCH_N12(41568, 41760)
    case C20_30:
        BCH_N12(43008, 43200)
    case C124_180:
        BCH_N12(44448, 44640)
    case C25_36:
        BCH_N12(44808, 45000)
    case C128_180:
        BCH_N12(45888, 46080)
    case C13_18:
        BCH_N12(46608, 46800)
    case C132_180:
        BCH_N12(47328, 47520)
    case C22_30:
        BCH_N12(47328, 47520)
    case C135_180:
        BCH_N12(48408, 48600)
    case C140_180:
        BCH_N12(50208, 50400)
    case C7_9:
        BCH_N12(50208, 50400)
    case C154_180:
        BCH_N12(55248, 55440)
    default:
        return bch_code_invalid;
    }
}

bch_code bch_code::select_short(dvbs2_code_rate_t rate)
{
    switch (rate) {
    case C1_4:
        BCH_S12(3072, 3240)
    case C1_3:
        BCH_S12(5232, 5400)
    case C2_5:
        BCH_S12(6312, 6480)
    case C1_2:
        BCH_S12(7032, 7200)
    case C3_5:
        BCH_S12(9552, 9720)
    case C2_3:
        BCH_S12(10632, 10800)
    case C3_4:
        BCH_S12(11712, 11880)
    case C4_5:
        BCH_S12(12432, 12600)
    case C5_6:
        BCH_S12(13152, 13320)
    case C8_9:
        BCH_S12(14232, 14400)
    case C11_45:
        BCH_S12(3792, 3960)
    case C4_15:
        BCH_S12(4152, 4320)
    case C14_45:
        BCH_S12(4872, 5040)
    case C7_15:
        BCH_S12(7392, 7560)
    case C8_15:
        BCH_S12(8472, 8640)
    case C26_45:
        BCH_S12(9192, 9360)
    case C32_45:
        BCH_S12(11352, 11520)
    default:
        return bch_code_invalid;
    }
}

bch_code bch_code::select_vlsnr(dvbs2_vlsnr_header_t vlsnr_header)
{
    switch (vlsnr_header) {
    case VLSNR_N_QPSK_2_9:
        BCH_N12(14208, 14400)
    case VLSNR_M_BPSK_1_5:
        BCH_M12(5660, 5840)
    case VLSNR_M_BPSK_11_45:
        BCH_M12(7740, 7920)
    case VLSNR_M_BPSK_1_3:
        BCH_M12(10620, 10800)
    case VLSNR_S_BPSK_SF2_1_5:
        BCH_S12(2512, 2680)
    case VLSNR_S_BPSK_SF2_11_45:
        BCH_S12(3792, 3960)
    case VLSNR_S_BPSK_1_5:
        BCH_S12(3072, 3240)
    case VLSNR_S_BPSK_4_15:
        BCH_S12(4152, 4320)
    case VLSNR_S_BPSK_1_3:
        BCH_S12(5232, 5400)
    default:
        return bch_code_invalid;
    }
}

const bch_code bch_code::bch_code_invalid = { 0xFF };
const bch_code bch_code::bch_code_N8 = { BCH_CODE_N8 };
const bch_code bch_code::bch_code_N10 = { BCH_CODE_N10 };
const bch_code bch_code::bch_code_N12 = { BCH_CODE_N12 };
const bch_code bch_code::bch_code_S12 = { BCH_CODE_S12 };
const bch_code bch_code::bch_code_M12 = { BCH_CODE_M12 };

} // namespace dvbs2acm
} // namespace gr