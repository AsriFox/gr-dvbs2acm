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

#include "bb_header.hh"
#include "bbheader_bb_impl.h"
#include "bch_code.h"
#include "modcod.hh"
#include <gnuradio/io_signature.h>
#include <pmt/pmt.h>

namespace gr {
namespace dvbs2acm {
using input_type = unsigned char;
using output_type = unsigned char;

bbheader_bb::sptr
bbheader_bb::make(int modcod, bool pilots, dvbs2_rolloff_factor_t rolloff, int goldcode)
{
    return gnuradio::make_block_sptr<bbheader_bb_impl>(modcod, pilots, rolloff, goldcode);
}


/*
 * The private constructor
 */
bbheader_bb_impl::bbheader_bb_impl(int modcod,
                                   bool pilots,
                                   dvbs2_rolloff_factor_t rolloff,
                                   int goldcode)
    : gr::block("bbheader_bb",
                gr::io_signature::make(1, 1, sizeof(input_type)),
                gr::io_signature::make(1, 1, sizeof(output_type)))
{
    if (modcod < 0 || modcod > 128) {
        GR_LOG_WARN(d_logger, "Provided MODCOD value is out of range.");
        GR_LOG_WARN(d_logger, "MODCOD set to DUMMY.");
        this->modcod = 0;
    } else if (modcod < 2 || (modcod > 57 && modcod < MC_QPSK_13_45) || modcod > MC_32APSK_32_45_S) {
        GR_LOG_WARN(d_logger, "Provided MODCOD value is reserved.");
        GR_LOG_WARN(d_logger, "MODCOD set to DUMMY.");
        if (modcod < 64) {             // DVB-S2
            this->modcod = modcod & 1; // Select framesize
        } else {                       // DVB-S2X
            this->modcod = 0;
        }
    }
    this->modcod = (modcod << 1) | pilots;

    if (goldcode < 0 || goldcode > 262141) {
        GR_LOG_WARN(d_logger, "Gold Code must be between 0 and 262141 inclusive.");
        GR_LOG_WARN(d_logger, "Gold Code set to 0.");
        goldcode = 0;
    }
    root_code = gold_to_root(goldcode);

    header.ts_gs = TS_GS_GENERIC_CONTINUOUS; // TODO: GSE
    // f->ts_gs = TS_GS_GENERIC_PACKETIZED;
    header.sis_mis = SIS_MIS_SINGLE;
    header.ccm_acm = ACM;
    header.issyi = ISSYI_NOT_ACTIVE;
    header.upl = 0;  // TODO: GSE
    header.sync = 0; // TODO: GSE
    if (rolloff & 0x4 || modcod >= MC_QPSK_13_45) {
        dvbs2x = true;
    }
    header.ro = rolloff & 0x3;

    build_crc8_table(crc_tab.data());
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

        auto framesize = (modcod & 2) ? FECFRAME_SHORT : FECFRAME_NORMAL;
        auto pilots = (modcod & 1);
        auto constellation = modcod_constellation(modcod);
        auto code_rate = modcod_rate(modcod);

        const uint64_t tagmodcod = (uint64_t(root_code) << 32) | (uint64_t(pilots) << 24) |
                                   (uint64_t(constellation) << 16) | (uint64_t(code_rate) << 8) |
                                   (uint64_t(framesize) << 1) | uint64_t(0);
        pmt::pmt_t key = pmt::string_to_symbol("modcod");
        pmt::pmt_t value = pmt::from_uint64(tagmodcod);
        this->add_item_tag(0, tagoffset, key, value);
        auto kbch = bch_code::select(framesize, code_rate).kbch;
        header.dfl = kbch - BB_HEADER_LENGTH_BITS;
        if (framesize != FECFRAME_MEDIUM) {
            header.add_to_frame(&out[offset], count, nibble, dvbs2x && alternate);
            if (dvbs2x) {
                alternate = !alternate;
            }
            offset += BB_HEADER_LENGTH_BITS;
            for (int j = 0; j < (int)(header.dfl / 8); j++) {
                if (header.sync == 0x47) {
                    // MPEG-2 Transport Stream
                    if (count == 0) {
                        if (*in != header.sync) {
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
                }
                // else Generic Continous Stream
                // TODO: GSE
                consumed++;
                offset += unpack_bits_8(b, &out[offset]);
            }
        } else {
            header.add_to_frame(&out[offset], count, nibble, dvbs2x && alternate);
            if (dvbs2x) {
                alternate = !alternate;
            }
            offset += BB_HEADER_LENGTH_BITS;
            for (int j = 0; j < (int)(header.dfl / 4); j++) {
                // TODO: Generic Streams
                if (nibble) {
                    if (count == 0) {
                        if (header.sync != 0 && *in != header.sync) {
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
    consume_each(consumed);
    return WORK_CALLED_PRODUCE;
}

} /* namespace dvbs2acm */
} /* namespace gr */
