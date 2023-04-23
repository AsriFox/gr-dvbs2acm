/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2018 Ahmet Inan, Ron Economos.
 *
 * This file is part of gr-dvbs2rx.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_BCH_DECODER_BB_H
#define INCLUDED_DVBS2ACM_BCH_DECODER_BB_H

#include <gnuradio/block.h>
#include <gnuradio/dvbs2acm/api.h>
#include <gnuradio/dvbs2acm/dvbs2_config.h>

namespace gr {
namespace dvbs2acm {

/*!
 * \brief <+description of block+>
 * \ingroup dvbs2acm
 *
 */
class DVBS2ACM_API bch_decoder_bb : virtual public gr::block
{
public:
    typedef std::shared_ptr<bch_decoder_bb> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of dvbs2acm::bch_decoder_bb.
     *
     * To avoid accidental use of raw pointers, dvbs2acm::bch_decoder_bb's
     * constructor is in a private implementation
     * class. dvbs2acm::bch_decoder_bb::make is the public interface for
     * creating new instances.
     */
    static sptr make(gr::dvbs2::dvbs2_outputmode_t outputmode, int debug_level = 0);

    /*!
     * \brief Get count of processed FECFRAMEs.
     * \return uint64_t FECFRAME count.
     */
    virtual uint64_t get_frame_count() = 0;

    /*!
     * \brief Get count of FECFRAMEs output with residual uncorrected errors.
     * \return uint64_t FECFRAME error count.
     */
    virtual uint64_t get_error_count() = 0;
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_BCH_DECODER_BB_H */
