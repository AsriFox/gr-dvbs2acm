/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2014,2016,2017,2020 Ron Economos.
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
     * \param modcod DVB-S2 FEC code rate and constellation (MODCOD).
     * \param pilots Pilot symbols (on/off).
     * \param rolloff DVB-S2 root-raised-cosine filter roll-off factor.
     * \param goldcode PL scrambler Gold code (0 to 262141 inclusive).
     * \param compat_mode Enable compatibility with drmpeg/gr-dvbs2 blocks.
     */
    static sptr
    make(int modcod, bool pilots, dvbs2_rolloff_factor_t rolloff, int goldcode, bool compat_mode);

    virtual void set_modcod(int modcod) = 0;
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_BBHEADER_BB_H */
