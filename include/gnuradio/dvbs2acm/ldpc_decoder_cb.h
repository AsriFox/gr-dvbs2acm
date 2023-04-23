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
 * \brief <+description of block+>
 * \ingroup dvbs2acm
 *
 */
class DVBS2ACM_API ldpc_decoder_cb : virtual public gr::block
{
public:
    typedef std::shared_ptr<ldpc_decoder_cb> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of dvbs2acm::ldpc_decoder_cb.
     *
     * To avoid accidental use of raw pointers, dvbs2acm::ldpc_decoder_cb's
     * constructor is in a private implementation
     * class. dvbs2acm::ldpc_decoder_cb::make is the public interface for
     * creating new instances.
     */
    static sptr
    make(gr::dvbs2::dvbs2_outputmode_t outputmode, gr::dvbs2::dvbs2_infomode_t infomode, int max_trials, int debug_level = 0);

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
