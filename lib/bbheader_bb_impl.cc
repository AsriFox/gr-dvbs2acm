/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2014,2016,2017,2020 Ron Economos.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "bb_header.hh"
#include "bbheader_bb_impl.h"
#include "bch_code.h"
#include "modcod.hh"
#include "gnuradio/dvbs2acm/dvbs2_config.h"
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
                gr::io_signature::make(1, 1, sizeof(output_type))),
      pilots(pilots)
{
    set_modcod(modcod);

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
    header.npd = NPD_NOT_ACTIVE;
    header.upl = 0;  // TODO: GSE
    header.sync = 0; // TODO: GSE
    if (rolloff & 0x4 || modcod >= MC_QPSK_13_45) {
        dvbs2x = true;
    }
    header.ro = rolloff & 0x3;

    build_crc8_table(crc_tab.data());

    const pmt::pmt_t port_id = pmt::mp("cmd");
    message_port_register_in(port_id);
    set_msg_handler(port_id, [this](pmt::pmt_t msg) { this->handle_cmd_msg(msg); });

    set_tag_propagation_policy(TPP_DONT);
}

/*
 * Our virtual destructor.
 */
bbheader_bb_impl::~bbheader_bb_impl() {}

void bbheader_bb_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    ninput_items_required[0] = (kbch - 80) / 8;
}

void bbheader_bb_impl::set_modcod(int modcod)
{
    if (modcod < 0 || modcod > 128) {
        GR_LOG_WARN(d_logger, "Provided MODCOD value is out of range.");
        GR_LOG_WARN(d_logger, "MODCOD set to DUMMY.");
        this->modcod = MC_DUMMY;
    } else if (modcod < 2 || (modcod > 57 && modcod < MC_QPSK_13_45) || modcod > MC_32APSK_32_45_S) {
        GR_LOG_WARN(d_logger, "Provided MODCOD value is reserved.");
        GR_LOG_WARN(d_logger, "MODCOD set to DUMMY.");
        if (modcod < 64) { // DVB-S2
            this->modcod = (modcod & 1) ? MC_DUMMY_S : MC_DUMMY;
        } else { // DVB-S2X
            this->modcod = MC_DUMMY;
        }
    } else {
        this->modcod = (dvbs2_modcod_t)modcod;
    }
    this->kbch = bch_code::select(this->modcod, this->vlsnr_header).kbch;
    header.dfl = kbch - BB_HEADER_LENGTH_BITS;
}

void bbheader_bb_impl::handle_cmd_msg(pmt::pmt_t msg)
{
    gr::thread::scoped_lock l(d_mutex);

    if (!pmt::is_uint64(msg)) {
        throw std::runtime_error("bbheader_bb: ACM command message must be an integer");
    }
    auto modcod = (int)(pmt::to_uint64(msg) & 0xff);
    set_modcod(modcod);
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
    if (ninput_items[0] < header.dfl / 8) {
        consume_each(0);
        return 0;
    }
    gr::thread::scoped_lock l(d_mutex);

    auto in = static_cast<const input_type*>(input_items[0]);
    auto out = static_cast<output_type*>(output_items[0]);

    auto constellation = modcod_constellation(modcod);
    auto code_rate = modcod_rate(modcod);
    auto framesize = modcod_framesize(modcod);
    if (modcod == MC_VLSNR_SET1 || modcod == MC_VLSNR_SET2) {
        constellation = vlsnr_constellation(vlsnr_header);
        code_rate = vlsnr_rate(vlsnr_header);
        framesize = vlsnr_framesize(vlsnr_header);
    }

    const auto tagoffset = nitems_written(0);
    const uint64_t tagmodcod = (uint64_t(root_code) << 32) | (uint64_t(pilots) << 24) |
                               (uint64_t(constellation) << 16) | (uint64_t(code_rate) << 8) |
                               (uint64_t(framesize) << 1) | uint64_t(0);
    add_item_tag(0, tagoffset, pmt::string_to_symbol("modcod"), pmt::from_uint64(tagmodcod));

    input_type b;
    if (framesize != FECFRAME_MEDIUM) {
        header.add_to_frame(out, count, nibble, dvbs2x && alternate);
        if (dvbs2x) {
            alternate = !alternate;
        }
        auto offset = BB_HEADER_LENGTH_BITS;
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
            } else {
                // Generic Continous Stream
                b = *in++;
            }
            // TODO: GSE
            offset += unpack_bits_8(b, &out[offset]);
        }
    } else {
        header.add_to_frame(out, count, nibble, dvbs2x && alternate);
        if (dvbs2x) {
            alternate = !alternate;
        }
        auto offset = BB_HEADER_LENGTH_BITS;
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

    consume_each(header.dfl / 8);
    return kbch;
}

} /* namespace dvbs2acm */
} /* namespace gr */
