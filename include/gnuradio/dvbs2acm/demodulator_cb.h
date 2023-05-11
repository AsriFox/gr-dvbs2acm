/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_DEMODULATOR_CB_H
#define INCLUDED_DVBS2ACM_DEMODULATOR_CB_H

#include <gnuradio/block.h>
#include <gnuradio/dvbs2acm/api.h>

namespace gr {
namespace dvbs2acm {

/*!
 * \brief Demodulates received DVB-S2 frames.
 * \ingroup dvbs2acm
 *
 */
class DVBS2ACM_API demodulator_cb : virtual public gr::block
{
public:
    typedef std::shared_ptr<demodulator_cb> sptr;

    /*!
     * \brief Create a DVB-S2 Demodulator.
     */
    static sptr make(float precision);

    virtual void set_precision(float precision) = 0;
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_DEMODULATOR_CB_H */
