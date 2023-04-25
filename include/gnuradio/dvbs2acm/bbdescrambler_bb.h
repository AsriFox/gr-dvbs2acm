/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2018 Ron Economos.
 *
 * This file is part of gr-dvbs2rx.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_BBDESCRAMBLER_BB_H
#define INCLUDED_DVBS2ACM_BBDESCRAMBLER_BB_H

#include <gnuradio/dvbs2acm/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace dvbs2acm {

/*!
 * \brief Descrambles FEC baseband frames with a PRBS decoder.
 * \ingroup dvbs2acm
 *
 * \details
 * Input: Variable length FEC baseband frames (BBFRAME).
 * Output: Descrambled variable length FEC baseband frames (BBFRAME).
 */
class DVBS2ACM_API bbdescrambler_bb : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<bbdescrambler_bb> sptr;

    /*!
     * \brief Create a BBFRAME descrambler.
     */
    static sptr make();
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_BBDESCRAMBLER_BB_H */
