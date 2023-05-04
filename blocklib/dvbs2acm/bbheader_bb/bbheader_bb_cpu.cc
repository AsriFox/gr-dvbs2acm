/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox
 *
 * This file is part of gr-dvbs2acm
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "bbheader_bb_cpu.h"
#include "bbheader_bb_cpu_gen.h"

namespace gr {
namespace dvbs2acm {

bbheader_bb_cpu::bbheader_bb_cpu(block_args args) : INHERITED_CONSTRUCTORS {}

work_return_t bbheader_bb_cpu::work(work_io& wio)
{
    // Do <+signal processing+>
    // Block specific code goes here
    return work_return_t::OK;
}


} // namespace dvbs2acm
} // namespace gr