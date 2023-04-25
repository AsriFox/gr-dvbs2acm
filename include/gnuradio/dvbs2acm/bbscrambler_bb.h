/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2014,2016 Ron Economos.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_DVBS2ACM_BBSCRAMBLER_BB_H
#define INCLUDED_DVBS2ACM_BBSCRAMBLER_BB_H

#include <gnuradio/dvbs2acm/api.h>
#include <gnuradio/dvbs2acm/dvbs2_config.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace dvbs2acm {

/*!
 * \brief Scrambles FEC baseband frames with a PRBS encoder.
 * \ingroup dvbs2acm
 *
 * \details
 * Input: Variable length FEC baseband frames (BBFRAME).
 * Output: Scrambled variable length FEC baseband frames (BBFRAME).
 */
class DVBS2ACM_API bbscrambler_bb : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<bbscrambler_bb> sptr;

    /*!
     * \brief Create a BBFRAME scrambler.
     */
    static sptr make();
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_BBSCRAMBLER_BB_H */
