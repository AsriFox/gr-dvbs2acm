/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2018 Ahmet Inan, Ron Economos.
 *
 * This file is part of gr-dvbs2rx.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "bch_decoder_bb_impl.h"
#include "debug_level.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/logger.h>
#include <functional>
#include <vector>

namespace gr {
namespace dvbs2acm {

bch_decoder_bb::sptr bch_decoder_bb::make(int debug_level)
{
    return gnuradio::make_block_sptr<bch_decoder_bb_impl>(debug_level);
}


/*
 * The private constructor
 */
bch_decoder_bb_impl::bch_decoder_bb_impl(int debug_level)
    : FRAMESTREAM_BLOCK_DEF("bch_decoder_bb"),
      d_debug_level(debug_level),
      d_frame_cnt(0),
      d_frame_error_cnt(0),
      params(bch_code::bch_code_invalid),
      instance_n(new GF_NORMAL()),
      instance_m(new GF_MEDIUM()),
      instance_s(new GF_SHORT()),
      decode_n_12(new BCH_NORMAL_12()),
      decode_n_10(new BCH_NORMAL_10()),
      decode_n_8(new BCH_NORMAL_8()),
      decode_m_12(new BCH_MEDIUM_12()),
      decode_s_12(new BCH_SHORT_12())
{
    code.fill(0);
    parity.fill(0);
    set_tag_propagation_policy(TPP_CUSTOM);
    set_output_multiple(FRAME_SIZE_NORMAL);
}

/*
 * Our virtual destructor.
 */
bch_decoder_bb_impl::~bch_decoder_bb_impl() {}

void bch_decoder_bb_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    if (params.kbch != 0) {
        ninput_items_required[0] = (noutput_items / params.kbch) * params.nbch;
    } else {
        ninput_items_required[0] = noutput_items;
    }
}

int bch_decoder_bb_impl::FRAMESTREAM_GENERAL_WORK()
{
    FRAMESTREAM_GENERAL_WORK_BEGIN

    for (tag_t tag : tags) {
        FRAMESTREAM_HANDLE_TAG
        auto params = bch_code::select(modcod, vlsnr_header);
        if (this->params != params) {
            this->params = params;
            set_relative_rate(params.kbch, params.nbch);
            set_output_multiple(params.kbch);
        }
        if (this->params.kbch + produced_total > (unsigned int)noutput_items) {
            break;
        }
        FRAMESTREAM_PROPAGATE_TAG
        FRAMESTREAM_PRODUCE_WORK
    }
    FRAMESTREAM_GENERAL_WORK_END
}

void bch_decoder_bb_impl::FRAMESTREAM_SPECIFIC_WORK()
{
    auto kbch = params.kbch;
    auto nbch = params.nbch;
    int corrections = 0;
    for (unsigned int j = 0; j < kbch; j++) {
        CODE::set_be_bit(code.data(), j, *in++);
    }
    for (unsigned int j = 0; j < nbch - kbch; j++) {
        CODE::set_be_bit(parity.data(), j, *in++);
    }
    switch (params.code) {
    case BCH_CODE_N12:
        corrections = decode_n_12->decode(code.data(), parity.data(), 0, 0, kbch);
        break;
    case BCH_CODE_N10:
        corrections = decode_n_10->decode(code.data(), parity.data(), 0, 0, kbch);
        break;
    case BCH_CODE_N8:
        corrections = decode_n_8->decode(code.data(), parity.data(), 0, 0, kbch);
        break;
    case BCH_CODE_S12:
        corrections = decode_s_12->decode(code.data(), parity.data(), 0, 0, kbch);
        break;
    case BCH_CODE_M12:
        corrections = decode_m_12->decode(code.data(), parity.data(), 0, 0, kbch);
        break;
    }
    if (corrections > 0) {
        d_logger->debug("frame = {:d}, BCH decoder corrections = {:d}", d_frame_cnt, corrections);
    } else if (corrections == -1) {
        d_frame_error_cnt++;
        d_logger->debug("frame = {:d}, BCH decoder too many bit errors (FER = {:g})",
                        d_frame_cnt,
                        ((double)d_frame_error_cnt / (d_frame_cnt + 1)));
    }
    d_frame_cnt++;
    for (unsigned int j = 0; j < kbch; j++) {
        *out++ = CODE::get_be_bit(code.data(), j);
    }
    consumed = nbch;
    produced = kbch;
}

} /* namespace dvbs2acm */
} /* namespace gr */
