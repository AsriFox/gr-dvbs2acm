/* -*- c++ -*- */
/*
 * Copyright 2022 Igor Freire.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_ROTATOR_CC_H
#define INCLUDED_DVBS2ACM_ROTATOR_CC_H

#include <gnuradio/dvbs2acm/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace dvbs2acm {

/*!
 * \brief Complex rotator
 * \ingroup dvbs2acm
 *
 * \details
 * Rotates an input complex sequence using a complex exponential in the form of
 * exp(1j * phase_inc * n), where "phase_inc" is a chosen phase increment in
 * radians and "n" is the sample index.
 *
 * Message Ports:
 *
 * - cmd (input):
 *    Receives a PMT dictionary with a command message to set a new phase
 *    increment on the rotator at a specified sample offset. The new increment
 *    must be provided as a PMT double on a key named "inc". The target sample
 *    offset on which to update the phase increment must be given as a PMT
 *    uint64 (with the absolute output item number) on a key named
 *    "offset". Unlike the "inc" key, the "offset" key is optional. When not
 *    provided, the rotator updates its phase increment immediately.
 */
class DVBS2ACM_API rotator_cc : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<rotator_cc> sptr;

    /*!
     * \brief Make a complex rotator block
     * \param phase_inc rotational velocity
     * \param tag_inc_updates Tag the sample where a phase increment update is
     *                        applied following the reception of a control
     *                        message received via the input message port.
     */
    static sptr make(double phase_inc = 0.0, bool tag_inc_updates = false);

    virtual void set_phase_inc(double phase_inc) = 0;
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_ROTATOR_CC_H */
