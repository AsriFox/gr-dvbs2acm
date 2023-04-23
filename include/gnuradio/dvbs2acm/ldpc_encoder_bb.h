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

#ifndef INCLUDED_DVBS2ACM_LDPC_ENCODER_BB_H
#define INCLUDED_DVBS2ACM_LDPC_ENCODER_BB_H

#include <gnuradio/block.h>
#include <gnuradio/dvbs2acm/api.h>

namespace gr {
namespace dvbs2acm {

/*!
 * \brief Encodes a LDPC (Low-Density Parity-Check) FEC.
 * \ingroup dvbs2
 *
 * Input: Variable length FEC baseband frames with appended BCH (BCHFEC).
 * Output: Normal, medium or short FEC baseband frames with appended LPDC (LDPCFEC).
 */
class DVBS2ACM_API ldpc_encoder_bb : virtual public gr::block
{
public:
    typedef std::shared_ptr<ldpc_encoder_bb> sptr;

    /*!
     * \brief Create a baseband frame LDPC encoder.
     */
    static sptr make();
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_LDPC_ENCODER_BB_H */
