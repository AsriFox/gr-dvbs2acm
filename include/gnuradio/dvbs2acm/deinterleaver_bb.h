/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_DEINTERLEAVER_BB_H
#define INCLUDED_DVBS2ACM_DEINTERLEAVER_BB_H

#include <gnuradio/dvbs2acm/api.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
namespace dvbs2acm {

/*!
 * \brief Bit deinterleaves DVB-S2 FEC baseband frames.
 * \ingroup dvbs2acm
 *
 * Input: Bit-interleaved FEC-encoded frames.
 * Output: FEC-encoded baseband frames (LDPCFEC).
 */
class DVBS2ACM_API deinterleaver_bb : virtual public gr::tagged_stream_block
{
public:
    typedef std::shared_ptr<deinterleaver_bb> sptr;

    /*!
     * \brief Create a DVB-S2 bit deinterleaver.
     */
    static sptr make();
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_DEINTERLEAVER_BB_H */
