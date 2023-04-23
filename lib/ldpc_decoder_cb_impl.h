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

namespace gr {
namespace dvbs2acm {

class ldpc_decoder_cb_impl : public ldpc_decoder_cb
{
private:
    const int d_debug_level;
    unsigned int output_mode;
    unsigned int info_mode;
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
    LDPCInterface* ldpc;
    Modulation<gr_complex, int8_t>* mod;
    int d_simd_size;
    int8_t* soft;
    int8_t* dint;
    int8_t* tempu;
    int8_t* tempv;
    void* aligned_buffer;
    int (*decode)(void*, int8_t*, int);

    int interleave_lookup_table[FRAME_SIZE_NORMAL];
    int deinterleave_lookup_table[FRAME_SIZE_NORMAL];

    void generate_interleave_lookup();
    void generate_deinterleave_lookup();
    inline void interleave_parity_bits(int* tempu, const int*& in);
    inline void twist_interleave_columns(int* tempv, int* tempu, int rows, int mod, const int* twist);
    inline void twist_deinterleave_columns(int* tempv, int* tempu, int rows, int mod, const int* twist);

    const static int twist16n[8];
    const static int twist64n[12];
    const static int twist256n[16];

    const static int twist16s[8];
    const static int twist64s[12];
    const static int twist256s[8];

    const static int mux16[8];
    const static int mux64[12];
    const static int mux256[16];

    const static int mux16_35[8];
    const static int mux16_13[8];
    const static int mux16_25[8];
    const static int mux64_35[12];
    const static int mux64_13[12];
    const static int mux64_25[12];
    const static int mux256_35[16];
    const static int mux256_23[16];

    const static int mux256s[8];
    const static int mux256s_13[8];
    const static int mux256s_25[8];

public:
    ldpc_decoder_cb_impl(gr::dvbs2::dvbs2_outputmode_t outputmode,
                         gr::dvbs2::dvbs2_infomode_t infomode,
                         int max_trials,
                         int debug_level);
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
