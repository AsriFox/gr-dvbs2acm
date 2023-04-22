/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_BBHEADER_BB_H
#define INCLUDED_DVBS2ACM_BBHEADER_BB_H

#include <gnuradio/block.h>
#include <gnuradio/dvbs2acm/api.h>
#include <gnuradio/dvbs2acm/dvbs2_config.h>

namespace gr {
namespace dvbs2acm {

/*!
 * \brief Formats Generic Stream packets into FEC baseband frames
 * and adds a 10-byte header.
 * \ingroup dvbs2acm
 * \details
 * Input: Generic Stream packets.
 * Output: Variable length FEC baseband frames (BBFRAME).
 *         The output frame length is based on the FEC rate.
 */
class DVBS2ACM_API bbheader_bb : virtual public gr::block
{
public:
    typedef std::shared_ptr<bbheader_bb> sptr;

    /*!
     * \brief Create a baseband header formatter.
     *
     * \param framesize FEC frame size (normal, medium or short).
     * \param code_rate FEC code rate.
     * \param constellation DVB-S2 constellation.
     * \param pilots Pilot symbols (on/off).
     * \param rolloff DVB-S2 root-raised-cosine filter roll-off factor.
     * \param goldcode PL scrambler Gold code (0 to 262141 inclusive).
     */
    static sptr make(gr::dvbs2::dvbs2_framesize_t framesize = gr::dvbs2::FECFRAME_NORMAL,
                     gr::dvbs2::dvbs2_code_rate_t code_rate = gr::dvbs2::C1_4,
                     gr::dvbs2::dvbs2_constellation_t constellation = gr::dvbs2::MOD_QPSK,
                     gr::dvbs2::dvbs2_pilots_t pilots = gr::dvbs2::PILOTS_ON,
                     gr::dvbs2::dvbs2_rolloff_factor_t rolloff = gr::dvbs2::RO_0_20,
                     int goldcode = 0);
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_BBHEADER_BB_H */
