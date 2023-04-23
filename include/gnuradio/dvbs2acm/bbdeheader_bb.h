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
 * \brief <+description of block+>
 * \ingroup dvbs2acm
 *
 */
class DVBS2ACM_API bbdeheader_bb : virtual public gr::block
{
public:
    typedef std::shared_ptr<bbdeheader_bb> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of dvbs2acm::bbdeheader_bb.
     *
     * To avoid accidental use of raw pointers, dvbs2acm::bbdeheader_bb's
     * constructor is in a private implementation
     * class. dvbs2acm::bbdeheader_bb::make is the public interface for
     * creating new instances.
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
