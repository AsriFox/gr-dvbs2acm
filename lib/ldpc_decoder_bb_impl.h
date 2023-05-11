/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_LDPC_DECODER_BB_IMPL_H
#define INCLUDED_DVBS2ACM_LDPC_DECODER_BB_IMPL_H

#include "ldpc_decoder/ldpc.hh"
#include <gnuradio/dvbs2acm/dvbs2_config.h>
#include <gnuradio/dvbs2acm/ldpc_decoder_bb.h>

namespace gr {
namespace dvbs2acm {

class ldpc_decoder_bb_impl : public ldpc_decoder_bb
{
private:
    const int d_debug_level;
    unsigned int chunk;
    unsigned int total_trials;
    int d_max_trials;

    dvbs2_framesize_t framesize;
    dvbs2_code_rate_t rate;
    std::unique_ptr<LDPCInterface> ldpc;

    int d_simd_size;
    void* aligned_buffer;
    int (*decode)(void*, int8_t*, int);
    void (*init)(LDPCInterface*);

public:
    ldpc_decoder_bb_impl(int max_trials, int debug_level);
    ~ldpc_decoder_bb_impl();

    // Where all the action really happens
    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;

    unsigned int get_average_trials() override { return total_trials / chunk; }
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_LDPC_DECODER_BB_IMPL_H */
