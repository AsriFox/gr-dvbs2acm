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

bbdeheader_bb::sptr bbdeheader_bb::make(int debug_level)
{
    return gnuradio::make_block_sptr<bbdeheader_bb_impl>(debug_level);
}


/*
 * The private constructor
 */
bbdeheader_bb_impl::bbdeheader_bb_impl(int debug_level)
    : FRAMESTREAM_BLOCK_DEF("bbdeheader_bb"),
      d_debug_level(debug_level),
      kbch(3072),
      count(0),
      synched(false),
      spanning(false),
      index(0),
      d_packet_cnt(0),
      d_error_cnt(0)
{
    build_crc8_table(crc_tab.data());
    set_tag_propagation_policy(TPP_DONT);
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

int bbdeheader_bb_impl::FRAMESTREAM_GENERAL_WORK()
{
    FRAMESTREAM_GENERAL_WORK_BEGIN
    for (tag_t tag : tags) {
        FRAMESTREAM_HANDLE_TAG
        FRAMESTREAM_PRODUCE_WORK
    }
    FRAMESTREAM_GENERAL_WORK_END
}

void bbdeheader_bb_impl::FRAMESTREAM_SPECIFIC_WORK()
{
    auto kbch = bch_code::select(modcod, vlsnr_header).kbch;
    if (this->kbch != kbch) {
        this->kbch = kbch;
        set_output_multiple((kbch - BB_HEADER_LENGTH_BITS) / 8);
    }
    auto max_dfl = kbch - BB_HEADER_LENGTH_BITS;
    consumed = kbch;

    // Check the BBHEADER integrity
    // enable_crc_check = true;
    if (check_crc8_bits(in, BB_HEADER_LENGTH_BITS) != 0) {
        synched = false;
        GR_LOG_DEBUG_LEVEL(1, "Baseband header crc failed.");
        in += kbch;
        return;
    }

    header.parse(in);
    df_remaining = header.dfl;

    // Validate the UPL, DFL and the SYNCD fields of the BBHEADER
    if (header.dfl > (int)max_dfl) {
        synched = false;
        d_logger->warn("Baseband header invalid (dfl > kbch - 80).");
        in += max_dfl;
        return;
    }

    if (header.dfl % 8 != 0) {
        synched = false;
        d_logger->warn("Baseband header invalid (dfl not a multiple of 8).");
        in += max_dfl;
        return;
    }

    if (header.syncd > header.dfl) {
        synched = false;
        d_logger->warn("Baseband header invalid (syncd > dfl).");
        in += max_dfl;
        return;
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
        return;
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

    // Process the DATAFIELD
    unsigned char tmp;
    produced = 0;
    while (df_remaining >= 8) {
        // Pack bits
        tmp = 0;
        for (int n = 7; n >= 0; n--) {
            tmp |= *in++ << n;
        }
        *out++ = tmp;
        produced++;
        df_remaining -= 8;
    }
    in += max_dfl - header.dfl; // Skip the DATAFIELD padding, if any
}

} /* namespace dvbs2acm */
} /* namespace gr */
