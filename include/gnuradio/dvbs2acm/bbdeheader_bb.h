/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2018 Ron Economos.
 *
 * This file is part of gr-dvbs2rx.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_BBDEHEADER_BB_H
#define INCLUDED_DVBS2ACM_BBDEHEADER_BB_H

#include <gnuradio/block.h>
#include <gnuradio/dvbs2acm/api.h>

namespace gr {
namespace dvbs2acm {

/*!
 * \brief Processes decoded FEC baseband frames, reading
 * the 10-byte baseband header and validating it.
 * \ingroup dvbs2acm
 * \details
 * Input: Variable length FEC baseband frames (BBFRAME).
 * Output: Generic Stream packets.
 */
class DVBS2ACM_API bbdeheader_bb : virtual public gr::block
{
public:
    typedef std::shared_ptr<bbdeheader_bb> sptr;

    /*!
     * \brief Create a baseband header reader.
     *
     * \param debug_level Max level of debug messages to display (0 to 4 inclusive).
     */
    static sptr make(int debug_level = 0);

    /*!
     * \brief Get count of packets extracted from BBFRAMEs.
     * \return uint64_t packet count.
     */
    virtual uint64_t get_packet_count() = 0;

    /*!
     * \brief Get count of corrupt packets extracted from BBFRAMEs.
     * \return uint64_t Corrupt packet count.
     */
    virtual uint64_t get_error_count() = 0;
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_BBDEHEADER_BB_H */
