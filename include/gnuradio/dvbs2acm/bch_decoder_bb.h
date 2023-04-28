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
 * \brief Decodes a BCH (Bose, Chaudhuri, Hocquenghem) FEC.
 * \ingroup dvbs2acm
 *
 * \details
 * Input: Variable length FEC baseband frames with appended BCH (BCHFEC).
 * Output: Variable length FEC baseband frames (BBFRAME).
 */
class DVBS2ACM_API bch_decoder_bb : virtual public gr::block
{
public:
    typedef std::shared_ptr<bch_decoder_bb> sptr;

    /*!
     * \brief Create a baseband frame BCH decoder.
     *
     * \param outputmode Use Message block output mode instead of Codeword.
     */
    static sptr make(bool outputmode = false, int debug_level = 0);

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
