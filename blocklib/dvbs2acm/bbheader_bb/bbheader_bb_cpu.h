/* -*- c++ -*- */
/*
 * Copyright 2023 Block Author
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/dvbs2acm/bbheader_bb.h>

namespace gr {
namespace dvbs2acm {

class bbheader_bb_cpu : public virtual bbheader_bb
{
public:
    bbheader_bb_cpu(block_args args);
    work_return_t work(work_io& wio) override;

private:
    // private variables here
};

} // namespace dvbs2acm
} // namespace gr