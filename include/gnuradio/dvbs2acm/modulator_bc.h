/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2014,2016 Ron Economos.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_MODULATOR_BC_H
#define INCLUDED_DVBS2ACM_MODULATOR_BC_H

#include <gnuradio/dvbs2acm/api.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
namespace dvbs2acm {

/*!
 * \brief Modulates DVB-S2 frames.
 * \ingroup dvbs2acm
 *
 * Input: Bit interleaved baseband frames.
 * Output: QPSK, 8PSK, 16APSK or 32APSK modulated complex IQ values (XFECFRAME).
 */
class DVBS2ACM_API modulator_bc : virtual public gr::tagged_stream_block
{
public:
    typedef std::shared_ptr<modulator_bc> sptr;

    /*!
     * \brief Create a DVB-S2 constellation modulator.
     */
    static sptr make();
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_MODULATOR_BC_H */
