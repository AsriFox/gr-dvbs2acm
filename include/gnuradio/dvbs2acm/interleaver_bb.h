/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2014,2016 Ron Economos.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_INTERLEAVER_BB_H
#define INCLUDED_DVBS2ACM_INTERLEAVER_BB_H

#include <gnuradio/dvbs2acm/api.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
namespace dvbs2acm {

/*!
 * \brief Bit interleaves DVB-S2 FEC baseband frames.
 * \ingroup dvbs2acm
 *
 * Input: Normal or short FEC baseband frames with appended LPDC (LDPCFEC).
 * Output: Bit interleaved baseband frames.
 */
class DVBS2ACM_API interleaver_bb : virtual public gr::tagged_stream_block
{
public:
    typedef std::shared_ptr<interleaver_bb> sptr;

    /*!
     * \brief Create a DVB-S2 bit interleaver.
     */
    static sptr make();
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_INTERLEAVER_BB_H */
