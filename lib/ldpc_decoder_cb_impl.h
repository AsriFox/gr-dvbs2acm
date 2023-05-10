/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_LDPC_DECODER_CB_IMPL_H
#define INCLUDED_DVBS2ACM_LDPC_DECODER_CB_IMPL_H

#include "ldpc_decoder/ldpc.hh"
#include "modulation.hh"
#include <gnuradio/dvbs2acm/ldpc_decoder_cb.h>
#include <vector>

namespace gr {
namespace dvbs2acm {

class ldpc_decoder_cb_impl : public ldpc_decoder_cb
{
private:
    const int d_debug_level;
    bool output_mode;
    bool info_mode;
    unsigned int frame;
    unsigned int chunk;
    unsigned int total_trials;
    int d_max_trials;
    float snr;
    float precision;
    float total_snr;
    unsigned int rowaddr0;
    unsigned int rowaddr1;
    unsigned int rowaddr2;
    std::unique_ptr<LDPCInterface> ldpc;
    std::unique_ptr<Modulation<gr_complex, int8_t>> mod;
    int d_simd_size;
    std::vector<int8_t> soft;
    std::vector<int8_t> dint;
    std::vector<int8_t> tempu;
    std::vector<int8_t> tempv;
    void* aligned_buffer;
    int (*decode)(void*, int8_t*, int);
    void (*init)(LDPCInterface*);

public:
    ldpc_decoder_cb_impl(bool outputmode, bool infomode, int max_trials, int debug_level);
    ~ldpc_decoder_cb_impl();

    // Where all the action really happens
    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);

    float get_snr() { return snr; }

    unsigned int get_average_trials() { return total_trials / chunk; }
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_LDPC_DECODER_CB_IMPL_H */
