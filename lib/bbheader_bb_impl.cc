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

#include "bbheader_bb_impl.h"
#include "ldpc_standard.h"
#include <gnuradio/io_signature.h>
#include <pmt/pmt.h>

namespace gr {
namespace dvbs2acm {
using input_type = unsigned char;
using output_type = unsigned char;

bbheader_bb::sptr bbheader_bb::make(gr::dvbs2::dvbs2_framesize_t framesize,
                                    gr::dvbs2::dvbs2_code_rate_t code_rate,
                                    gr::dvbs2::dvbs2_constellation_t constellation,
                                    gr::dvbs2::dvbs2_pilots_t pilots,
                                    gr::dvbs2::dvbs2_rolloff_factor_t rolloff,
                                    int goldcode)
{
    return gnuradio::make_block_sptr<bbheader_bb_impl>(
        framesize, code_rate, constellation, pilots, rolloff, goldcode);
}


/*
 * The private constructor
 */
bbheader_bb_impl::bbheader_bb_impl(gr::dvbs2::dvbs2_framesize_t framesize,
                                   gr::dvbs2::dvbs2_code_rate_t code_rate,
                                   gr::dvbs2::dvbs2_constellation_t constellation,
                                   gr::dvbs2::dvbs2_pilots_t pilots,
                                   gr::dvbs2::dvbs2_rolloff_factor_t rolloff,
                                   int goldcode)
    : gr::block("bbheader_bb",
                gr::io_signature::make(1, 1, sizeof(input_type)),
                gr::io_signature::make(1, 1, sizeof(output_type))),
      framesize(framesize),
      code_rate(code_rate),
      constellation(constellation),
      pilots(pilots)
{
    if (goldcode < 0 || goldcode > 262141) {
        GR_LOG_WARN(d_logger, "Gold Code 1 must be between 0 and 262141 inclusive.");
        GR_LOG_WARN(d_logger, "Gold Code 1 set to 0.");
        goldcode = 0;
    }
    root_code = gold_to_root(goldcode);
    kbch = gr::dvbs2::ldpc_std_values(framesize, code_rate).kbch;

    BBHeader* f = &m_format[0].bb_header;
    f->ts_gs = TS_GS_GENERIC_CONTINUOUS; // TODO: GSE
    // f->ts_gs = TS_GS_GENERIC_PACKETIZED;
    f->sis_mis = SIS_MIS_SINGLE;
    f->ccm_acm = ACM;
    f->issyi = ISSYI_NOT_ACTIVE;
    f->upl = 0; // TODO: GSE
    f->dfl = kbch - 80;
    f->sync = 0; // TODO: GSE
    if (rolloff & 0x4) {
        dvbs2x = true;
    }
    f->ro = rolloff & 0x3;

    build_crc8_table();
    set_output_multiple(FRAME_SIZE_NORMAL);
}

/*
 * Our virtual destructor.
 */
bbheader_bb_impl::~bbheader_bb_impl() {}

void bbheader_bb_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    ninput_items_required[0] = (noutput_items - 80) / 8;
}

void bbheader_bb_impl::build_crc8_table(void)
{
    const int crc_poly_rev = 0xD5;
    int r, crc;
    for (int i = 0; i < 256; i++) {
        r = i;
        crc = 0;
        for (int j = 1 << 7; j >= 1; j >>= 1) {
            if (((r & j) ? 1 : 0) ^ ((crc & 0x80) ? 1 : 0)) {
                crc = (crc << 1) ^ crc_poly_rev;
            } else {
                crc <<= 1;
            }
        }
        crc_tab[i] = crc;
    }
}

/*
 * MSB is sent first
 *
 * The polynomial has been reversed
 */
int bbheader_bb_impl::add_crc8_bits(unsigned char* in, int length)
{
    const int crc_poly = 0xAB;
    int crc = 0;
    int b;
    int i = 0;
    for (int n = 0; n < length; n++) {
        b = in[i++] ^ (crc & 0x01);
        crc >>= 1;
        if (b) {
            crc ^= crc_poly;
        }
    }
    for (int n = 1; n <= 1 << 7; n <<= 1) {
        in[i++] = (crc & n) ? 1 : 0;
    }
    return 8; // Length of CRC-8
}

void bbheader_bb_impl::add_bbheader(unsigned char* out, int count, bool nibble)
{
    unsigned char* m_frame = out;
    BBHeader* h = &m_format[0].bb_header;

    m_frame[0] = h->ts_gs >> 1;
    m_frame[1] = h->ts_gs & 1;
    m_frame[2] = h->sis_mis;
    m_frame[3] = h->ccm_acm;
    m_frame[4] = h->issyi & 1;
    m_frame[5] = h->npd & 1;
    if (dvbs2x) {
        if (alternate) {
            alternate = false;
            m_frame[6] = 1;
            m_frame[7] = 1;
        } else {
            alternate = true;
            m_frame[6] = h->ro >> 1;
            m_frame[7] = h->ro & 1;
        }
    } else {
        m_frame[6] = h->ro >> 1;
        m_frame[7] = h->ro & 1;
    }

    int temp, m_frame_offset_bits = 8;
    if (h->sis_mis == SIS_MIS_MULTIPLE) {
        // TODO: NOT IMPLEMENTED
        // temp = isi;
        for (int n = 1 << 7; n >= 1; n >>= 1) {
            m_frame[m_frame_offset_bits++] = (temp & n) ? 1 : 0;
        }
    } else {
        for (int n = 1 << 7; n >= 1; n >>= 1) {
            m_frame[m_frame_offset_bits++] = 0;
        }
    }
    temp = h->upl;
    for (int n = 1 << 15; n >= 1; n >>= 1) {
        m_frame[m_frame_offset_bits++] = (temp & n) ? 1 : 0;
    }
    temp = h->dfl;
    for (int n = 1 << 15; n >= 1; n >>= 1) {
        m_frame[m_frame_offset_bits++] = (temp & n) ? 1 : 0;
    }
    temp = h->sync;
    for (int n = 1 << 7; n >= 1; n >>= 1) {
        m_frame[m_frame_offset_bits++] = (temp & n) ? 1 : 0;
    }
    temp = count;
    if (temp == 0) {
        // TODO: WTF?
        temp = count;
    } else {
        // TODO: GSE
        temp = (188 - count) * 8;
    }
    if (!nibble) {
        temp += 4;
    }
    for (int n = 1 << 15; n >= 1; n >>= 1) {
        m_frame[m_frame_offset_bits++] = (temp & n) ? 1 : 0;
    }
    // Append CRC to the end of BBHEADER
    int length = BB_HEADER_LENGTH_BITS;
    m_frame_offset_bits += add_crc8_bits(m_frame, length);
}

int bbheader_bb_impl::gold_to_root(int goldcode)
{
    int x = 1;
    for (int g = 0; g < goldcode; g++) {
        x = (((x ^ (x >> 7)) & 1) << 17) | (x >> 1);
    }
    return x;
}

int bbheader_bb_impl::general_work(int noutput_items,
                                   gr_vector_int& ninput_items,
                                   gr_vector_const_void_star& input_items,
                                   gr_vector_void_star& output_items)
{
    if (ninput_items[0] == 0) {
        consume_each(0);
        return WORK_CALLED_PRODUCE;
    }

    auto in = static_cast<const input_type*>(input_items[0]);
    auto out = static_cast<output_type*>(output_items[0]);
    int consumed = 0;
    int produced = 0;
    int offset = 0;
    unsigned char b;

    while (kbch + produced <= (unsigned int)noutput_items) {
        const uint64_t tagoffset = this->nitems_written(0);
        const uint64_t tagmodcod =
            (uint64_t(root_code) << 32) | (uint64_t(pilots) << 24) |
            (uint64_t(constellation) << 16) | (uint64_t(code_rate) << 8) |
            (uint64_t(framesize) << 1) | uint64_t(0);
        pmt::pmt_t key = pmt::string_to_symbol("modcod");
        pmt::pmt_t value = pmt::from_uint64(tagmodcod);
        this->add_item_tag(0, tagoffset, key, value);
        if (framesize != dvbs2::FECFRAME_MEDIUM) {
            add_bbheader(&out[offset], count, nibble);
            offset += 80;
            for (int j = 0; j < (int)((kbch - 80) / 8); j++) {
                if (count == 0) {
                    if (*in != 0x47) {
                        GR_LOG_WARN(d_logger, "Transport Stream sync error!");
                    }
                    in++;
                    b = crc;
                    crc = 0;
                } else {
                    b = *in++;
                    crc = crc_tab[b ^ crc];
                }
                count = (count + 1) % 188; // TODO: GSE
                consumed++;
                for (int n = 1 << 7; n >= 1; n >>= 1) {
                    out[offset++] = (b & n) ? 1 : 0;
                }
            }
        } else {
            add_bbheader(&out[offset], count, nibble);
            offset += 80;
            for (int j = 0; j < (int)((kbch - 80) / 4); j++) {
                if (nibble) {
                    if (count == 0) {
                        if (*in != 0x47) {
                            GR_LOG_WARN(d_logger, "Transport Stream sync error!");
                        }
                        in++;
                        b = crc;
                        crc = 0;
                    } else {
                        b = *in++;
                        crc = crc_tab[b ^ crc];
                    }
                    bsave = b;
                    count = (count + 1) % 188; // TODO: GSE
                    consumed++;
                    for (int n = 1 << 7; n >= 1; n >>= 1) {
                        out[offset++] = (b & n) ? 1 : 0;
                    }
                    nibble = false;
                } else {
                    for (int n = 1 << 3; n >= 1; n >>= 1) {
                        out[offset++] = (bsave & n) ? 1 : 0;
                    }
                    nibble = true;
                }
            }
        }
        produced += kbch;
        produce(0, kbch);
    }
    consume_each(produced);
    return WORK_CALLED_PRODUCE;
}

} /* namespace dvbs2acm */
} /* namespace gr */
