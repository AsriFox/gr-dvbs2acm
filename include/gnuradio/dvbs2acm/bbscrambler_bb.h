/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2014,2016 Ron Economos.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_BBSCRAMBLER_BB_H
#define INCLUDED_DVBS2ACM_BBSCRAMBLER_BB_H

#include <gnuradio/dvbs2acm/api.h>
#include <gnuradio/dvbs2acm/dvbs2_config.h>
#include <gnuradio/tagged_stream_block.h>

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
    class DVBS2ACM_API bbscrambler_bb : virtual public gr::tagged_stream_block
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
