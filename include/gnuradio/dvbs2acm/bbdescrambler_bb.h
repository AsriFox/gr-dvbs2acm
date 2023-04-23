/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2018 Ron Economos.
 *
 * This file is part of gr-dvbs2rx.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_BBDESCRAMBLER_BB_H
#define INCLUDED_DVBS2ACM_BBDESCRAMBLER_BB_H

#include <gnuradio/dvbs2acm/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace dvbs2acm {

/*!
 * \brief <+description of block+>
 * \ingroup dvbs2acm
 *
 */
class DVBS2ACM_API bbdescrambler_bb : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<bbdescrambler_bb> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of dvbs2acm::bbdescrambler_bb.
     *
     * To avoid accidental use of raw pointers, dvbs2acm::bbdescrambler_bb's
     * constructor is in a private implementation
     * class. dvbs2acm::bbdescrambler_bb::make is the public interface for
     * creating new instances.
     */
    static sptr make();
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_BBDESCRAMBLER_BB_H */
