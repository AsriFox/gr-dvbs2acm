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

#include <gnuradio/block.h>
#include <gnuradio/dvbs2acm/api.h>

namespace gr {
namespace dvbs2acm {

/*!
 * \brief Decodes a LDPC (Low-Density Parity-Check) FEC in received DVB-S2 frames.
 * \ingroup dvbs2acm
 *
 * Input: Soft-valued FEC-encoded baseband frames with appended LPDC (LDPCFEC).
 * Output: BCH-encoded baseband frames (BCHFEC).
 */
class DVBS2ACM_API ldpc_decoder_bb : virtual public gr::block
{
public:
    typedef std::shared_ptr<ldpc_decoder_bb> sptr;

    /*!
     * \brief Create a DVB-S2 demodulator / deinterleaver / LDPC decoder.
     *
     * \param max_trials Maximum count of LDPC decoding trials before passing the codeword.
     * \param debug_level Level of debug messages to display.
     */
    static sptr make(int max_trials = 25, int debug_level = 0);

    /*!
     * \brief Get the average number of LDPC decoding iterations per frame.
     * \return unsigned int Average decoding interations.
     */
    virtual unsigned int get_average_trials() = 0;
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_LDPC_DECODER_BB_H */
