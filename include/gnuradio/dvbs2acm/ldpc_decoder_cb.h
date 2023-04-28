/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2018 Ahmet Inan, Ron Economos.
 *
 * This file is part of gr-dvbs2rx.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_LDPC_DECODER_CB_H
#define INCLUDED_DVBS2ACM_LDPC_DECODER_CB_H

#include <gnuradio/block.h>
#include <gnuradio/dvbs2acm/api.h>
#include <gnuradio/dvbs2acm/dvbs2_config.h>

namespace gr {
namespace dvbs2acm {

/*!
 * \brief Demodulates DVB-S2 frames, deinterleaves them
 * and decodes a LDPC (Low-Density Parity-Check) FEC.
 * \ingroup dvbs2acm
 *
 * Input: QPSK or 8PSK modulated complex IQ values (XFECFRAME).
 * Output: Normal, medium or short FEC baseband frames with appended LPDC (LDPCFEC).
 */
class DVBS2ACM_API ldpc_decoder_cb : virtual public gr::block
{
public:
    typedef std::shared_ptr<ldpc_decoder_cb> sptr;

    /*!
     * \brief Create a DVB-S2 demodulator / deinterleaver / LDPC decoder.
     *
     * \param outputmode Use Message block output mode instead of Codeword.
     * \param infomode Output SNR information to console.
     */
    static sptr
    make(bool outputmode = false, bool infomode = false, int max_trials = 25, int debug_level = 0);

    /*!
     * \brief Get the measured SNR.
     * \return float Measured SNR.
     */
    virtual float get_snr() = 0;

    /*!
     * \brief Get the average number of LDPC decoding iterations per frame.
     * \return unsigned int Average decoding interations.
     */
    virtual unsigned int get_average_trials() = 0;
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_LDPC_DECODER_CB_H */
