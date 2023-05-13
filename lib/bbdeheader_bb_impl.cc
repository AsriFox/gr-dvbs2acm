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
#include <gnuradio/io_signature.h>
#include <gnuradio/logger.h>

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
      kbch(FRAME_SIZE_NORMAL),
      count(0),
      synched(false),
      spanning(false),
      index(0),
      d_packet_cnt(0),
      d_error_cnt(0)
{
    build_crc8_table(crc_tab.data());
    set_output_multiple((kbch - BB_HEADER_LENGTH_BITS) / 8 * 2);
}

/*
 * Our virtual destructor.
 */
bbdeheader_bb_impl::~bbdeheader_bb_impl() {}

void bbdeheader_bb_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    auto n_bbframes = noutput_items / ((kbch - BB_HEADER_LENGTH_BITS) / 8);
    ninput_items_required[0] = n_bbframes * kbch;
}

int bbdeheader_bb_impl::general_work(int noutput_items,
                                     gr_vector_int& ninput_items,
                                     gr_vector_const_void_star& input_items,
                                     gr_vector_void_star& output_items)
{
    auto in = static_cast<const input_type*>(input_items[0]);
    auto out = static_cast<output_type*>(output_items[0]);
    // unsigned char* tei = out;
    unsigned int errors = 0;
    unsigned int consumed_total = 0;
    unsigned int produced_total = 0;
    dvbs2_modcod_t modcod = MC_DUMMY;
    dvbs2_vlsnr_header_t vlsnr_header = VLSNR_DUMMY;
    unsigned char tmp;

    std::vector<tag_t> tags;
    const uint64_t nread = this->nitems_read(0);

    // Read all tags on the input buffer
    this->get_tags_in_range(tags, 0, nread, nread + noutput_items, pmt::string_to_symbol("pls"));

    for (tag_t tag : tags) {
        if (tag.key == pmt::intern("pls") && tag.value->is_dict()) {
            auto dict = tag.value;
            if (pmt::dict_has_key(dict, pmt::intern("modcod")) &&
                pmt::dict_has_key(dict, pmt::intern("vlsnr_header"))) {
                auto not_found = pmt::get_PMT_NIL();

                auto modcod_r = pmt::dict_ref(dict, pmt::intern("modcod"), not_found);
                if (modcod_r == not_found) {
                    continue;
                }
                modcod = (dvbs2_modcod_t)pmt::to_long(modcod_r);

                auto vlsnr_header_r = pmt::dict_ref(dict, pmt::intern("vlsnr_header"), not_found);
                if (vlsnr_header_r == not_found) {
                    continue;
                }
                vlsnr_header = (dvbs2_vlsnr_header_t)pmt::to_long(vlsnr_header_r);
            }
        }
        auto kbch = bch_code::select(modcod, vlsnr_header).kbch;
        if (this->kbch != kbch) {
            this->kbch = kbch;
            set_output_multiple((kbch - BB_HEADER_LENGTH_BITS) / 8 * 2);
        }
        auto max_dfl = kbch - BB_HEADER_LENGTH_BITS;

        if (produced_total + max_dfl / 8 > (unsigned)noutput_items) {
            break;
        }
        consumed_total += kbch;

        // Check the BBHEADER integrity
        // enable_crc_check = true;
        if (check_crc8_bits(in, BB_HEADER_LENGTH_BITS) != 0) {
            synched = false;
            GR_LOG_DEBUG_LEVEL(1, "Baseband header crc failed.");
            in += kbch;
            continue;
        }

        header.parse(in);
        df_remaining = header.dfl;

        // Validate the UPL, DFL and the SYNCD fields of the BBHEADER
        if (header.dfl > (int)max_dfl) {
            synched = false;
            d_logger->warn("Baseband header invalid (dfl > kbch - 80).");
            in += max_dfl;
            continue;
        }

        if (header.dfl % 8 != 0) {
            synched = false;
            d_logger->warn("Baseband header invalid (dfl not a multiple of 8).");
            in += max_dfl;
            continue;
        }

        if (header.syncd > header.dfl) {
            synched = false;
            d_logger->warn("Baseband header invalid (syncd > dfl).");
            in += max_dfl;
            continue;
        }

        // if (header.upl != (TRANSPORT_PACKET_LENGTH * 8)) {
        //     synched = FALSE;
        //     d_logger->warn("Baseband header unsupported (upl != 188 bytes).");
        //     in += max_dfl;
        //     continue;
        // }

        if (header.syncd % 8 != 0) {
            synched = false;
            d_logger->warn("Baseband header unsupported (syncd not byte-aligned).");
            in += max_dfl;
            continue;
        }

        // Skip the initial SYNCD bits of the DATAFIELD if re-synchronizing
        if (!synched) {
            GR_LOG_DEBUG_LEVEL(1, "Baseband header resynchronizing.");
            in += header.syncd + 8;
            df_remaining -= header.syncd + 8;
            count = 0;
            synched = true;
            index = 0;
            spanning = false;
            distance = header.syncd;
        }

        GR_LOG_DEBUG_LEVEL(3,
                           "MATYPE: TS/GS={:b}; SIS/MIS={}; CCM/ACM={}; ISSYI={}; "
                           "NPD={}; RO={:b}; ISI={}",
                           header.ts_gs,
                           header.sis_mis,
                           header.ccm_acm,
                           header.issyi,
                           header.npd,
                           header.ro,
                           header.isi);

        // TODO: MPEG-2 TS processing
        // while (df_remaining) {
        // if (count == 0) {
        //     crc = 0;
        //     if (index == TRANSPORT_PACKET_LENGTH) {
        //         for (int j = 0; j < TRANSPORT_PACKET_LENGTH; j++) {
        //             *out++ = packet[j];
        //             produced++;
        //         }
        //         index = 0;
        //         spanning = FALSE;
        //     }
        //     if (df_remaining < (TRANSPORT_PACKET_LENGTH - 1) * 8) {
        //         index = 0;
        //         packet[index++] = 0x47;
        //         spanning = TRUE;
        //     } else {
        //         *out++ = 0x47;
        //         produced++;
        //         tei = out;
        //     }
        //     count++;
        //     if (check == TRUE) {
        //         if (distance != (unsigned int)header.syncd) {
        //             synched = FALSE;
        //         }
        //         check = FALSE;
        //     }
        // } else if (count == TRANSPORT_PACKET_LENGTH) {
        //     if (enable_crc_check) {
        //         tmp = 0;
        //         for (int n = 7; n >= 0; n--) {
        //             tmp |= *in++ << n;
        //         }
        //         if (tmp != crc) {
        //             errors++;
        //             if (spanning) {
        //                 packet[1] |= TRANSPORT_ERROR_INDICATOR;
        //             } else {
        //                 *tei |= TRANSPORT_ERROR_INDICATOR;
        //             }
        //             d_error_cnt++;
        //         }
        //         df_remaining -= 8;
        //         d_packet_cnt++;
        //     }
        //     count = 0;
        //     if (df_remaining == 0) {
        //         distance = (TRANSPORT_PACKET_LENGTH - 1) * 8;
        //     }
        // }
        // if (df_remaining >= 8 && count > 0) {
        //     tmp = 0;
        //     for (int n = 7; n >= 0; n--) {
        //         tmp |= *in++ << n;
        //         distance++;
        //     }
        //     crc = crc_tab[tmp ^ crc];
        //     if (spanning == TRUE) {
        //         packet[index++] = tmp;
        //     } else {
        //         *out++ = tmp;
        //         produced++;
        //     }
        //     count++;
        //     df_remaining -= 8;
        //     if (df_remaining == 0) {
        //         distance = 0;
        //     }
        // }
        // }

        // Process the DATAFIELD
        while (df_remaining >= 8) {
            // Pack bits
            tmp = 0;
            for (int n = 7; n >= 0; n--) {
                tmp |= *in++ << n;
            }
            *out++ = tmp;
            produced_total++;
            df_remaining -= 8;
        }
        in += max_dfl - header.dfl; // Skip the DATAFIELD padding, if any
    }

    if (errors != 0) {
        GR_LOG_DEBUG_LEVEL(1,
                           "TS packet crc errors = {:d} (PER = {:g})",
                           errors,
                           ((double)d_error_cnt / d_packet_cnt));
    }

    consume_each(consumed_total);
    return produced_total;
}

} /* namespace dvbs2acm */
} /* namespace gr */
