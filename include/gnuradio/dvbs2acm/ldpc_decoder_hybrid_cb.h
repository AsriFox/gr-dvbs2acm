/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2018,2019 Ahmet Inan, Ron Economos.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_LDPC_DECODER_HYBRID_CB_H
#define INCLUDED_DVBS2ACM_LDPC_DECODER_HYBRID_CB_H

#include <gnuradio/block.h>
#include <gnuradio/dvbs2acm/api.h>

namespace gr {
namespace dvbs2acm {

/*!
 * \brief Demodulates, deinterleaves and decodes LDPC of received DVB-S2 frames.
 * \ingroup dvbs2acm
 */
class DVBS2ACM_API ldpc_decoder_hybrid_cb : virtual public gr::block
{
public:
    typedef std::shared_ptr<ldpc_decoder_hybrid_cb> sptr;

    /*!
     * \brief Create a DVB-S2 LDPC Decoder Hybrid.
     */
    static sptr make(bool infomode = false, int max_trials = 25, int debug_level = 0);

    virtual float get_snr() = 0;

    virtual unsigned int get_average_trials() = 0;
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_LDPC_DECODER_HYBRID_CB_H */
