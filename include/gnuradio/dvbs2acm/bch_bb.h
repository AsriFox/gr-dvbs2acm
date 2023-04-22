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

#ifndef INCLUDED_DVBS2ACM_BCH_BB_H
#define INCLUDED_DVBS2ACM_BCH_BB_H

#include <gnuradio/block.h>
#include <gnuradio/dvbs2acm/api.h>
#include <gnuradio/dvbs2acm/dvbs2_config.h>

namespace gr {
namespace dvbs2acm {

/*!
 * \brief Encodes a BCH ((Bose, Chaudhuri, Hocquenghem) FEC.
 * \ingroup dvbs2
 *
 * \details
 * Input: Variable length FEC baseband frames (BBFRAME).
 * Output: Variable length FEC baseband frames with appended BCH (BCHFEC).
 */
class DVBS2ACM_API bch_bb : virtual public gr::block
{
public:
    typedef std::shared_ptr<bch_bb> sptr;

    /*!
     * \brief Create a baseband frame BCH encoder.
     */
    static sptr make();
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_BCH_BB_H */
