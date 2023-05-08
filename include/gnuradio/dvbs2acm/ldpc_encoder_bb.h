/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2014,2016 Ron Economos.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_LDPC_ENCODER_BB_H
#define INCLUDED_DVBS2ACM_LDPC_ENCODER_BB_H

#include <gnuradio/dvbs2acm/api.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
namespace dvbs2acm {

/*!
 * \brief Encodes a LDPC (Low-Density Parity-Check) FEC.
 * \ingroup dvbs2acm
 *
 * Input: Variable length FEC baseband frames with appended BCH (BCHFEC).
 * Output: Normal, medium or short FEC baseband frames with appended LPDC (LDPCFEC).
 */
class DVBS2ACM_API ldpc_encoder_bb : virtual public gr::tagged_stream_block
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
