/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2018 Ahmet Inan, Ron Economos.
 *
 * This file is part of gr-dvbs2rx.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_BCH_DECODER_BB_IMPL_H
#define INCLUDED_DVBS2ACM_BCH_DECODER_BB_IMPL_H

#include "bch_code.h"
#include "bch_decoder.hh"
#include "frame_stream.hh"
#include "galois_field.hh"
#include <gnuradio/dvbs2acm/bch_decoder_bb.h>
#include <array>

namespace gr {
namespace dvbs2acm {

using input_type = unsigned char;
using output_type = unsigned char;

class bch_decoder_bb_impl : public bch_decoder_bb
{
private:
    const int d_debug_level;
    uint64_t d_frame_cnt;
    uint64_t d_frame_error_cnt;
    bch_code params;

    typedef CODE::GaloisField<16, 0b10000000000101101, uint16_t> GF_NORMAL;
    typedef CODE::GaloisField<15, 0b1000000000101101, uint16_t> GF_MEDIUM;
    typedef CODE::GaloisField<14, 0b100000000101011, uint16_t> GF_SHORT;
    typedef CODE::BoseChaudhuriHocquenghemDecoder<24, 1, 65343, GF_NORMAL> BCH_NORMAL_12;
    typedef CODE::BoseChaudhuriHocquenghemDecoder<20, 1, 65375, GF_NORMAL> BCH_NORMAL_10;
    typedef CODE::BoseChaudhuriHocquenghemDecoder<16, 1, 65407, GF_NORMAL> BCH_NORMAL_8;
    typedef CODE::BoseChaudhuriHocquenghemDecoder<24, 1, 32587, GF_MEDIUM> BCH_MEDIUM_12;
    typedef CODE::BoseChaudhuriHocquenghemDecoder<24, 1, 16215, GF_SHORT> BCH_SHORT_12;

    std::unique_ptr<GF_NORMAL> instance_n;
    std::unique_ptr<GF_MEDIUM> instance_m;
    std::unique_ptr<GF_SHORT> instance_s;
    std::unique_ptr<BCH_NORMAL_12> decode_n_12;
    std::unique_ptr<BCH_NORMAL_10> decode_n_10;
    std::unique_ptr<BCH_NORMAL_8> decode_n_8;
    std::unique_ptr<BCH_MEDIUM_12> decode_m_12;
    std::unique_ptr<BCH_SHORT_12> decode_s_12;

    std::array<uint8_t, 8192> code;
    std::array<uint8_t, 24> parity;

public:
    bch_decoder_bb_impl(int debug_level);
    ~bch_decoder_bb_impl();

    // Where all the action really happens
    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int FRAMESTREAM_GENERAL_WORK();

    void FRAMESTREAM_SPECIFIC_WORK();

    uint64_t get_frame_count() { return d_frame_cnt; }
    uint64_t get_error_count() { return d_frame_error_cnt; }
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_BCH_DECODER_BB_IMPL_H */
