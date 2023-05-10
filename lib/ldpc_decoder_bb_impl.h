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
#include <vector>

namespace gr {
namespace dvbs2acm {

class ldpc_decoder_bb_impl : public ldpc_decoder_bb
{
private:
    const int d_debug_level;
    unsigned int chunk;
    int d_max_trials;
    std::unique_ptr<LDPCInterface> ldpc;
    int d_simd_size;
    void* aligned_buffer;

    dvbs2_framesize_t framesize;
    dvbs2_code_rate_t rate;
    int (*decode)(void*, int8_t*, int);
    void (*init)(LDPCInterface*);

protected:
    void parse_length_tags(const std::vector<std::vector<tag_t>>& tags,
                           gr_vector_int& n_input_items_reqd) override;
    int calculate_output_stream_length(const gr_vector_int& ninput_items) override;

public:
    ldpc_decoder_bb_impl(int max_trials, int debug_level);
    ~ldpc_decoder_bb_impl();

    int work(int noutput_items,
             gr_vector_int& ninput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_LDPC_DECODER_BB_IMPL_H */
