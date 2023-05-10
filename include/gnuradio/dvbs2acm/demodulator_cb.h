/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_DEMODULATOR_CB_H
#define INCLUDED_DVBS2ACM_DEMODULATOR_CB_H

#include <gnuradio/dvbs2acm/api.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
namespace dvbs2acm {

/*!
 * \brief Demodulates DVB-S2 frames.
 * \ingroup dvbs2acm
 *
 * \details
 * Input: QPSK, 8PSK, 16APSK or 32APSK modulated complex IQ values (XFECFRAME).
 * Output: Soft-valued bit-interleaved FEC-encoded frame.
 */
class DVBS2ACM_API demodulator_cb : virtual public gr::tagged_stream_block
{
public:
    typedef std::shared_ptr<demodulator_cb> sptr;

    /*!
     * \brief Create a DVB-S2 constellation demodulator.
     */
    static sptr make(float precision);

    virtual void set_precision(float precision) = 0;
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_DEMODULATOR_CB_H */
