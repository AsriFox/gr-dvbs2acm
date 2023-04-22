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

#include "bch_bb_impl.h"
#include "ldpc_standard.h"
#include <gnuradio/io_signature.h>
#include <pmt/pmt.h>
#include <vector>

namespace gr {
namespace dvbs2acm {
using input_type = unsigned char;
using output_type = unsigned char;
bch_bb::sptr bch_bb::make() { return gnuradio::make_block_sptr<bch_bb_impl>(); }


/*
 * The private constructor
 */
bch_bb_impl::bch_bb_impl()
    : gr::block("bch_bb",
                gr::io_signature::make(1, 1, sizeof(input_type)),
                gr::io_signature::make(1, 1, sizeof(output_type)))
{
}

/*
 * Our virtual destructor.
 */
bch_bb_impl::~bch_bb_impl() {}

void bch_bb_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    ninput_items_required[0] = noutput_items;
}

/*
 * Polynomial calculation routines
 * multiply polynomials
 */
int bch_bb_impl::poly_mult(const int* ina, int lena, const int* inb, int lenb, int* out)
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
void bch_bb_impl::poly_pack(const int* pin, unsigned int* pout, int len)
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

void bch_bb_impl::poly_reverse(int* pin, int* pout, int len)
{
    int c;
    c = len - 1;

    for (int i = 0; i < len; i++) {
        pout[c--] = pin[i];
    }
}

/*
 *Shift a 128 bit register
 */
inline void bch_bb_impl::reg_4_shift(unsigned int* sr)
{
    sr[3] = (sr[3] >> 1) | (sr[2] << 31);
    sr[2] = (sr[2] >> 1) | (sr[1] << 31);
    sr[1] = (sr[1] >> 1) | (sr[0] << 31);
    sr[0] = (sr[0] >> 1);
}

/*
 * Shift 160 bits
 */
inline void bch_bb_impl::reg_5_shift(unsigned int* sr)
{
    sr[4] = (sr[4] >> 1) | (sr[3] << 31);
    sr[3] = (sr[3] >> 1) | (sr[2] << 31);
    sr[2] = (sr[2] >> 1) | (sr[1] << 31);
    sr[1] = (sr[1] >> 1) | (sr[0] << 31);
    sr[0] = (sr[0] >> 1);
}

/*
 * Shift 192 bits
 */
inline void bch_bb_impl::reg_6_shift(unsigned int* sr)
{
    sr[5] = (sr[5] >> 1) | (sr[4] << 31);
    sr[4] = (sr[4] >> 1) | (sr[3] << 31);
    sr[3] = (sr[3] >> 1) | (sr[2] << 31);
    sr[2] = (sr[2] >> 1) | (sr[1] << 31);
    sr[1] = (sr[1] >> 1) | (sr[0] << 31);
    sr[0] = (sr[0] >> 1);
}

void bch_bb_impl::bch_poly_build_tables(void)
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

    len = poly_mult(polyn01, 17, polyn02, 17, polyout[0]);
    len = poly_mult(polyn03, 17, polyout[0], len, polyout[1]);
    len = poly_mult(polyn04, 17, polyout[1], len, polyout[0]);
    len = poly_mult(polyn05, 17, polyout[0], len, polyout[1]);
    len = poly_mult(polyn06, 17, polyout[1], len, polyout[0]);
    len = poly_mult(polyn07, 17, polyout[0], len, polyout[1]);
    len = poly_mult(polyn08, 17, polyout[1], len, polyout[0]);
    poly_pack(polyout[0], m_poly_n_8, 128);

    len = poly_mult(polyn09, 17, polyout[0], len, polyout[1]);
    len = poly_mult(polyn10, 17, polyout[1], len, polyout[0]);
    poly_pack(polyout[0], m_poly_n_10, 160);

    len = poly_mult(polyn11, 17, polyout[0], len, polyout[1]);
    len = poly_mult(polyn12, 17, polyout[1], len, polyout[0]);
    poly_pack(polyout[0], m_poly_n_12, 192);

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
    poly_pack(polyout[0], m_poly_s_12, 168);

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
    poly_pack(polyout[0], m_poly_m_12, 180);
}


int bch_bb_impl::general_work(int noutput_items,
                              gr_vector_int& ninput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    auto in = static_cast<const input_type*>(input_items[0]);
    auto out = static_cast<output_type*>(output_items[0]);
    unsigned char b, temp;
    unsigned int shift[6];
    int consumed = 0;
    int produced = 0;
    int produced_per_iteration;

    std::vector<tag_t> tags;
    const uint64_t nread = this->nitems_read(0); // number of items read on port 0

    // Read all tags on the input buffer
    this->get_tags_in_range(
        tags, 0, nread, nread + noutput_items, pmt::string_to_symbol("modcod"));

    for (int i = 0; i < (int)tags.size(); i++) {
        auto tagmodcod = pmt::to_uint64(tags[i].value);
        auto params = gr::dvbs2::ldpc_std_values::from_tag(tagmodcod);
        if (params.nbch + produced > (unsigned int)noutput_items) {
            break;
        }
        produced_per_iteration = 0;
        const uint64_t tagoffset = this->nitems_written(0);
        pmt::pmt_t key = pmt::string_to_symbol("modcod");
        pmt::pmt_t value = pmt::from_uint64(tagmodcod);
        this->add_item_tag(0, tagoffset, key, value);
        auto kbch = params.kbch;
        switch (params.bch_code) {
        case BCH_CODE_N12:
            // Zero the shift register
            memset(shift, 0, sizeof(unsigned int) * 6);
            // MSB of the codeword first
            for (int j = 0; j < (int)kbch; j++) {
                temp = *in++;
                *out++ = temp;
                b = (temp ^ (shift[5] & 1));
                reg_6_shift(shift);
                if (b) {
                    shift[0] ^= m_poly_n_12[0];
                    shift[1] ^= m_poly_n_12[1];
                    shift[2] ^= m_poly_n_12[2];
                    shift[3] ^= m_poly_n_12[3];
                    shift[4] ^= m_poly_n_12[4];
                    shift[5] ^= m_poly_n_12[5];
                }
            }
            consumed += kbch;
            produced += kbch;
            produced_per_iteration += kbch;
            // Now add the parity bits to the output
            for (int n = 0; n < 192; n++) {
                *out++ = (shift[5] & 1);
                reg_6_shift(shift);
            }
            produced += 192;
            produced_per_iteration += 192;
            break;
        case BCH_CODE_N10:
            // Zero the shift register
            memset(shift, 0, sizeof(unsigned int) * 5);
            // MSB of the codeword first
            for (int j = 0; j < (int)kbch; j++) {
                temp = *in++;
                *out++ = temp;
                b = (temp ^ (shift[4] & 1));
                reg_5_shift(shift);
                if (b) {
                    shift[0] ^= m_poly_n_10[0];
                    shift[1] ^= m_poly_n_10[1];
                    shift[2] ^= m_poly_n_10[2];
                    shift[3] ^= m_poly_n_10[3];
                    shift[4] ^= m_poly_n_10[4];
                }
            }
            consumed += kbch;
            produced += kbch;
            produced_per_iteration += kbch;
            // Now add the parity bits to the output
            for (int n = 0; n < 160; n++) {
                *out++ = (shift[4] & 1);
                reg_5_shift(shift);
            }
            produced += 160;
            produced_per_iteration += 160;
            break;
        case BCH_CODE_N8:
            // Zero the shift register
            memset(shift, 0, sizeof(unsigned int) * 4);
            // MSB of the codeword first
            for (int j = 0; j < (int)kbch; j++) {
                temp = *in++;
                *out++ = temp;
                b = temp ^ (shift[3] & 1);
                reg_4_shift(shift);
                if (b) {
                    shift[0] ^= m_poly_n_8[0];
                    shift[1] ^= m_poly_n_8[1];
                    shift[2] ^= m_poly_n_8[2];
                    shift[3] ^= m_poly_n_8[3];
                }
            }
            consumed += kbch;
            produced += kbch;
            produced_per_iteration += kbch;
            // Now add the parity bits to the output
            for (int n = 0; n < 128; n++) {
                *out++ = shift[3] & 1;
                reg_4_shift(shift);
            }
            produced += 128;
            produced_per_iteration += 128;
            break;
        case BCH_CODE_S12:
            // Zero the shift register
            memset(shift, 0, sizeof(unsigned int) * 6);
            // MSB of the codeword first
            for (int j = 0; j < (int)kbch; j++) {
                temp = *in++;
                *out++ = temp;
                b = (temp ^ ((shift[5] & 0x01000000) ? 1 : 0));
                reg_6_shift(shift);
                if (b) {
                    shift[0] ^= m_poly_s_12[0];
                    shift[1] ^= m_poly_s_12[1];
                    shift[2] ^= m_poly_s_12[2];
                    shift[3] ^= m_poly_s_12[3];
                    shift[4] ^= m_poly_s_12[4];
                    shift[5] ^= m_poly_s_12[5];
                }
            }
            consumed += kbch;
            produced += kbch;
            produced_per_iteration += kbch;
            // Now add the parity bits to the output
            for (int n = 0; n < 168; n++) {
                *out++ = (shift[5] & 0x01000000) ? 1 : 0;
                reg_6_shift(shift);
            }
            produced += 168;
            produced_per_iteration += 168;
            break;
        case BCH_CODE_M12:
            // Zero the shift register
            memset(shift, 0, sizeof(unsigned int) * 6);
            // MSB of the codeword first
            for (int j = 0; j < (int)kbch; j++) {
                temp = *in++;
                *out++ = temp;
                b = (temp ^ ((shift[5] & 0x00001000) ? 1 : 0));
                reg_6_shift(shift);
                if (b) {
                    shift[0] ^= m_poly_m_12[0];
                    shift[1] ^= m_poly_m_12[1];
                    shift[2] ^= m_poly_m_12[2];
                    shift[3] ^= m_poly_m_12[3];
                    shift[4] ^= m_poly_m_12[4];
                    shift[5] ^= m_poly_m_12[5];
                }
            }
            consumed += kbch;
            produced += kbch;
            produced_per_iteration += kbch;
            // Now add the parity bits to the output
            for (int n = 0; n < 180; n++) {
                *out++ = (shift[5] & 0x00001000) ? 1 : 0;
                reg_6_shift(shift);
            }
            produced += 180;
            produced_per_iteration += 180;
            break;
        }
        produce(0, produced_per_iteration);
    }
    consume_each(consumed);
    return WORK_CALLED_PRODUCE;
}

} /* namespace dvbs2acm */
} /* namespace gr */
