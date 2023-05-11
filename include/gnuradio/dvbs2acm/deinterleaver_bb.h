/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_DEINTERLEAVER_BB_H
#define INCLUDED_DVBS2ACM_DEINTERLEAVER_BB_H

#include <gnuradio/block.h>
#include <gnuradio/dvbs2acm/api.h>

namespace gr {
namespace dvbs2acm {

/*!
 * \brief Deinterleaves received DVB-S2 frames.
 * \ingroup dvbs2acm
 *
 */
class DVBS2ACM_API deinterleaver_bb : virtual public gr::block
{
public:
    typedef std::shared_ptr<deinterleaver_bb> sptr;

    /*!
     * \brief Create a DVB-S2 Deinterleaver.
     */
    static sptr make();
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_DEINTERLEAVER_BB_H */
