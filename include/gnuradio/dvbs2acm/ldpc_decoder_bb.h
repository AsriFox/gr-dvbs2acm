/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2018 Ahmet Inan, Ron Economos.
 *
 * This file is part of gr-dvbs2rx.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_LDPC_DECODER_BB_H
#define INCLUDED_DVBS2ACM_LDPC_DECODER_BB_H

#include <gnuradio/dvbs2acm/api.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
namespace dvbs2acm {

/*!
 * \brief Decodes a LDPC (Low-Density Parity-Check) FEC.
 * \ingroup dvbs2acm
 *
 * Input: Soft-valued FEC-encoded frames.
 * Output: BCH-encoded frames.
 */
class DVBS2ACM_API ldpc_decoder_bb : virtual public gr::tagged_stream_block
{
public:
    typedef std::shared_ptr<ldpc_decoder_bb> sptr;

    /*!
     * \brief Create a DVB-S2 LDPC decoder.
     */
    static sptr make(int max_trials = 25, int debug_level = 0);
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_LDPC_DECODER_BB_H */
