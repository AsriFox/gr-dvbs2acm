/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2018,2021 Igor Freire, Ron Economos.
 *
 * This file is part of gr-dvbs2rx.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "bbdeheader_bb_impl.h"
#include "bch_code.h"
#include "debug_level.h"
#include <gnuradio/dvbs2acm/dvbs2_config.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/logger.h>

#define TRANSPORT_PACKET_LENGTH 188
#define TRANSPORT_ERROR_INDICATOR 0x80

namespace gr {
namespace dvbs2acm {

using input_type = unsigned char;
using output_type = unsigned char;

bbdeheader_bb::sptr bbdeheader_bb::make(int debug_level)
{
    return gnuradio::make_block_sptr<bbdeheader_bb_impl>(debug_level);
}


/*
 * The private constructor
 */
bbdeheader_bb_impl::bbdeheader_bb_impl(int debug_level)
    : gr::block("bbdeheader_bb",
                gr::io_signature::make(1, 1, sizeof(input_type)),
                gr::io_signature::make(1, 1, sizeof(output_type))),
      d_debug_level(debug_level),
      count(0),
      synched(false),
      spanning(false),
      index(0),
      d_packet_cnt(0),
      d_error_cnt(0)
{
    build_crc8_table();
}

/*
 * Our virtual destructor.
 */
bbdeheader_bb_impl::~bbdeheader_bb_impl() {}

void bbdeheader_bb_impl::build_crc8_table(void)
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
unsigned int bbdeheader_bb_impl::check_crc8_bits(const unsigned char* in, int length)
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
    return crc;
}

void bbdeheader_bb_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    ninput_items_required[0] = noutput_items * 8 + BB_HEADER_LENGTH_BITS;
    // TODO: output count
}

int bbdeheader_bb_impl::general_work(int noutput_items,
                                     gr_vector_int& ninput_items,
                                     gr_vector_const_void_star& input_items,
                                     gr_vector_void_star& output_items)
{
    auto in = static_cast<const input_type*>(input_items[0]);
    auto out = static_cast<output_type*>(output_items[0]);
    unsigned char* tei = out;
    unsigned int errors = 0;
    bool check, mode;
    unsigned int consumed = 0;
    unsigned int produced = 0;
    unsigned char tmp;
    BBHeader* h = &m_format[0].bb_header;

    std::vector<tag_t> tags;
    const uint64_t nread = this->nitems_read(0);

    // Read all tags on the input buffer
    this->get_tags_in_range(tags, 0, nread, nread + noutput_items, pmt::string_to_symbol("modcod"));

    for (tag_t tag : tags) {
        auto tagmodcod = pmt::to_uint64(tag.value);
        auto framesize = (dvbs2_framesize_t)((tagmodcod >> 1) & 0x7f);
        auto rate = (dvbs2_code_rate_t)((tagmodcod >> 8) & 0xff);
        auto kbch = bch_code::select(framesize, rate).kbch;
        auto max_dfl = kbch - BB_HEADER_LENGTH_BITS;

        if (produced + max_dfl > (unsigned)noutput_items) {
            break;
        }
        consumed += kbch;

        // Check the BBHEADER integrity
        check = check_crc8_bits(in, BB_HEADER_LENGTH_BITS) != 0;

        // if (dvb_standard == STANDARD_DVBS2) {
        //     mode = INPUTMODE_NORMAL;
        //     if (check == 0) {
        //         check = TRUE;
        //     } else {
        //         check = FALSE;
        //     }
        // } else {
        //     mode = INPUTMODE_NORMAL;
        //     if (check == 0) {
        //         check = TRUE;
        //     } else if (check == CRC_POLY) {
        //         check = TRUE;
        //         mode = INPUTMODE_HIEFF;
        //     } else {
        //         check = FALSE;
        //     }
        // }
        mode = true;
        check = !check;

        if (!check) {
            synched = false;
            GR_LOG_DEBUG_LEVEL(1, "Baseband header crc failed.");
            in += kbch;
            continue;
        }

        // Parse the BBHEADER
        h->ts_gs = *in++ << 1;
        h->ts_gs |= *in++;
        h->sis_mis = *in++;
        h->ccm_acm = *in++;
        h->issyi = *in++;
        h->npd = *in++;
        h->ro = *in++ << 1;
        h->ro |= *in++;
        h->isi = 0;
        if (h->sis_mis == 0) {
            for (int n = 7; n >= 0; n--) {
                h->isi |= *in++ << n;
            }
        } else {
            in += 8;
        }
        h->upl = 0;
        for (int n = 15; n >= 0; n--) {
            h->upl |= *in++ << n;
        }
        h->dfl = 0;
        for (int n = 15; n >= 0; n--) {
            h->dfl |= *in++ << n;
        }
        df_remaining = h->dfl;
        h->sync = 0;
        for (int n = 7; n >= 0; n--) {
            h->sync |= *in++ << n;
        }
        h->syncd = 0;
        for (int n = 15; n >= 0; n--) {
            h->syncd |= *in++ << n;
        }
        in += 8; // Skip the last byte (CRC-8), processed in the beginning.

        // Validate the UPL, DFL and the SYNCD fields of the BBHEADER
        if (h->dfl > (int)max_dfl) {
            synched = FALSE;
            d_logger->warn("Baseband header invalid (dfl > kbch - 80).");
            in += max_dfl;
            continue;
        }

        if (h->dfl % 8 != 0) {
            synched = FALSE;
            d_logger->warn("Baseband header invalid (dfl not a multiple of 8).");
            in += max_dfl;
            continue;
        }

        if (h->syncd > h->dfl) {
            synched = FALSE;
            d_logger->warn("Baseband header invalid (syncd > dfl).");
            in += max_dfl;
            continue;
        }

        if (h->upl != (TRANSPORT_PACKET_LENGTH * 8)) {
            synched = FALSE;
            d_logger->warn("Baseband header unsupported (upl != 188 bytes).");
            in += max_dfl;
            continue;
        }

        if (h->syncd % 8 != 0) {
            synched = FALSE;
            d_logger->warn("Baseband header unsupported (syncd not byte-aligned).");
            in += max_dfl;
            continue;
        }

        // Skip the initial SYNCD bits of the DATAFIELD if re-synchronizing
        if (synched == FALSE) {
            GR_LOG_DEBUG_LEVEL(1, "Baseband header resynchronizing.");
            if (mode) {
                in += h->syncd + 8;
                df_remaining -= h->syncd + 8;
            } else {
                in += h->syncd;
                df_remaining -= h->syncd;
            }
            count = 0;
            synched = TRUE;
            index = 0;
            spanning = FALSE;
            distance = h->syncd;
        }

        GR_LOG_DEBUG_LEVEL(3,
                           "MATYPE: TS/GS={:b}; SIS/MIS={}; CCM/ACM={}; ISSYI={}; "
                           "NPD={}; RO={:b}; ISI={}",
                           h->ts_gs,
                           h->sis_mis,
                           h->ccm_acm,
                           h->issyi,
                           h->npd,
                           h->ro,
                           h->isi);

        // Process the DATAFIELD
        if (mode) {
            while (df_remaining) {
                if (count == 0) {
                    crc = 0;
                    if (index == TRANSPORT_PACKET_LENGTH) {
                        for (int j = 0; j < TRANSPORT_PACKET_LENGTH; j++) {
                            *out++ = packet[j];
                            produced++;
                        }
                        index = 0;
                        spanning = FALSE;
                    }
                    if (df_remaining < (TRANSPORT_PACKET_LENGTH - 1) * 8) {
                        index = 0;
                        packet[index++] = 0x47;
                        spanning = TRUE;
                    } else {
                        *out++ = 0x47;
                        produced++;
                        tei = out;
                    }
                    count++;
                    if (check == TRUE) {
                        if (distance != (unsigned int)h->syncd) {
                            synched = FALSE;
                        }
                        check = FALSE;
                    }
                } else if (count == TRANSPORT_PACKET_LENGTH) {
                    tmp = 0;
                    for (int n = 7; n >= 0; n--) {
                        tmp |= *in++ << n;
                    }
                    if (tmp != crc) {
                        errors++;
                        if (spanning) {
                            packet[1] |= TRANSPORT_ERROR_INDICATOR;
                        } else {
                            *tei |= TRANSPORT_ERROR_INDICATOR;
                        }
                        d_error_cnt++;
                    }
                    count = 0;
                    d_packet_cnt++;
                    df_remaining -= 8;
                    if (df_remaining == 0) {
                        distance = (TRANSPORT_PACKET_LENGTH - 1) * 8;
                    }
                }
                if (df_remaining >= 8 && count > 0) {
                    tmp = 0;
                    for (int n = 7; n >= 0; n--) {
                        tmp |= *in++ << n;
                        distance++;
                    }
                    crc = crc_tab[tmp ^ crc];
                    if (spanning == TRUE) {
                        packet[index++] = tmp;
                    } else {
                        *out++ = tmp;
                        produced++;
                    }
                    count++;
                    df_remaining -= 8;
                    if (df_remaining == 0) {
                        distance = 0;
                    }
                }
            }
            in += max_dfl - h->dfl; // Skip the DATAFIELD padding, if any
        } else {
            while (df_remaining) {
                if (count == 0) {
                    if (index == TRANSPORT_PACKET_LENGTH) {
                        for (int j = 0; j < TRANSPORT_PACKET_LENGTH; j++) {
                            *out++ = packet[j];
                            produced++;
                        }
                        index = 0;
                        spanning = FALSE;
                    }
                    if (df_remaining < (TRANSPORT_PACKET_LENGTH - 1) * 8) {
                        index = 0;
                        packet[index++] = 0x47;
                        spanning = TRUE;
                    } else {
                        *out++ = 0x47;
                        produced++;
                    }
                    count++;
                    if (check == TRUE) {
                        if (distance != (unsigned int)h->syncd) {
                            synched = FALSE;
                        }
                        check = FALSE;
                    }
                } else if (count == TRANSPORT_PACKET_LENGTH) {
                    count = 0;
                    if (df_remaining == 0) {
                        distance = 0;
                    }
                }
                if (df_remaining >= 8 && count > 0) {
                    tmp = 0;
                    for (int n = 7; n >= 0; n--) {
                        tmp |= *in++ << n;
                        distance++;
                    }
                    if (spanning == TRUE) {
                        packet[index++] = tmp;
                    } else {
                        *out++ = tmp;
                        produced++;
                    }
                    count++;
                    df_remaining -= 8;
                    if (df_remaining == 0) {
                        distance = 0;
                    }
                }
            }
            in += max_dfl - h->dfl; // Skip the DATAFIELD padding, if any
        }
    }

    if (errors != 0) {
        GR_LOG_DEBUG_LEVEL(1,
                           "TS packet crc errors = {:d} (PER = {:g})",
                           errors,
                           ((double)d_error_cnt / d_packet_cnt));
    }

    // Tell runtime system how many input items we consumed on
    // each input stream.
    consume_each(consumed);

    // Tell runtime system how many output items we produced.
    return produced;
}

} /* namespace dvbs2acm */
} /* namespace gr */
