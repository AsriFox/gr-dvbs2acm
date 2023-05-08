/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2014,2016,2017 Ron Economos.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_PHYSICAL_CC_H
#define INCLUDED_DVBS2ACM_PHYSICAL_CC_H

#include <gnuradio/dvbs2acm/api.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
namespace dvbs2acm {

/*!
 * \brief Signals DVB-S2 physical layer frames.
 * \ingroup dvbs2acm
 *
 * Input: QPSK, 8PSK, 16APSK or 32APSK modulated complex IQ values (XFECFRAME).
 * Output: DVB-S2 PLFRAME.
 */
class DVBS2ACM_API physical_cc : virtual public gr::tagged_stream_block
{
public:
    typedef std::shared_ptr<physical_cc> sptr;

    /*!
     * \brief Create a DVB-S2 physical layer framer.
     *
     * \param dummyframes Enable dummy frames.
     */
    static sptr make(bool dummyframes = false);
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_PHYSICAL_CC_H */
