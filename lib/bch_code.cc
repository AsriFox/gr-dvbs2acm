#include "bch_code.h"
#include <cstring>

using namespace gr::dvbs2;

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

bch_code::bch_code(unsigned int kbch, unsigned int nbch, unsigned int code) : kbch(kbch), nbch(nbch), code(code)
{
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

bch_code bch_code::select(dvbs2_framesize_t framesize, dvbs2_code_rate_t rate)
{
    if (framesize == FECFRAME_NORMAL) {
        switch (rate) {
        case C1_4:
            return bch_code_1_2N;
        case C1_3:
            return bch_code_1_3N;
        case C2_5:
            return bch_code_2_5N;
        case C1_2:
            return bch_code_1_2N;
        case C3_5:
            return bch_code_3_5N;
        case C2_3:
            return bch_code_2_3N;
        case C3_4:
            return bch_code_3_4N;
        case C4_5:
            return bch_code_4_5N;
        case C5_6:
            return bch_code_5_6N;
        case C8_9:
            return bch_code_8_9N;
        case C9_10:
            return bch_code_9_10N;
        default:
            return { 0, 0, 0 };
        }
    } else if (framesize == FECFRAME_SHORT) {
        switch (rate) {
        case C1_4:
            return bch_code_1_2S;
        case C1_3:
            return bch_code_1_3S;
        case C2_5:
            return bch_code_2_5S;
        case C1_2:
            return bch_code_1_2S;
        case C3_5:
            return bch_code_3_5S;
        case C2_3:
            return bch_code_2_3S;
        case C3_4:
            return bch_code_3_4S;
        case C4_5:
            return bch_code_4_5S;
        case C5_6:
            return bch_code_5_6S;
        case C8_9:
            return bch_code_8_9S;
        default:
            return { 0, 0, 0 };
        }
    }
    // TODO: DVB-S2X
    return { 0, 0, 0 };
}

const bch_code bch_code::bch_code_1_4N = { 16008, 16200, BCH_CODE_N12 };
const bch_code bch_code::bch_code_1_3N = { 21408, 21600, BCH_CODE_N12 };
const bch_code bch_code::bch_code_2_5N = { 25728, 25920, BCH_CODE_N12 };
const bch_code bch_code::bch_code_1_2N = { 32208, 32400, BCH_CODE_N12 };
const bch_code bch_code::bch_code_3_5N = { 38688, 38880, BCH_CODE_N12 };
const bch_code bch_code::bch_code_2_3N = { 43040, 43200, BCH_CODE_N10 };
const bch_code bch_code::bch_code_3_4N = { 48408, 48600, BCH_CODE_N12 };
const bch_code bch_code::bch_code_4_5N = { 51648, 51840, BCH_CODE_N12 };
const bch_code bch_code::bch_code_5_6N = { 53840, 54000, BCH_CODE_N10 };
const bch_code bch_code::bch_code_8_9N = { 57472, 57600, BCH_CODE_N8 };
const bch_code bch_code::bch_code_9_10N = { 58192, 58320, BCH_CODE_N8 };

const bch_code bch_code::bch_code_1_4S = { 3072, 3240, BCH_CODE_S12 };
const bch_code bch_code::bch_code_1_3S = { 5232, 5400, BCH_CODE_S12 };
const bch_code bch_code::bch_code_2_5S = { 6312, 6480, BCH_CODE_S12 };
const bch_code bch_code::bch_code_1_2S = { 7032, 7200, BCH_CODE_S12 };
const bch_code bch_code::bch_code_3_5S = { 9552, 9720, BCH_CODE_S12 };
const bch_code bch_code::bch_code_2_3S = { 10632, 10800, BCH_CODE_S12 };
const bch_code bch_code::bch_code_3_4S = { 11712, 11880, BCH_CODE_S12 };
const bch_code bch_code::bch_code_4_5S = { 12432, 12600, BCH_CODE_S12 };
const bch_code bch_code::bch_code_5_6S = { 13152, 13320, BCH_CODE_S12 };
const bch_code bch_code::bch_code_8_9S = { 14232, 14400, BCH_CODE_S12 };
