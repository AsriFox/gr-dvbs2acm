/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 * Copyright 2018,2019 Ahmet Inan, Ron Economos.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DVBS2ACM_LDPC_DECODER_HYBRID_CB_IMPL_H
#define INCLUDED_DVBS2ACM_LDPC_DECODER_HYBRID_CB_IMPL_H

#include "ldpc_decoder/ldpc.hh"
#include "modulation.hh"
#include <gnuradio/dvbs2acm/dvbs2_config.h>
#include <gnuradio/dvbs2acm/ldpc_decoder_hybrid_cb.h>

namespace gr {
namespace dvbs2acm {

class ldpc_decoder_hybrid_cb_impl : public ldpc_decoder_hybrid_cb
{
private:
    const int d_debug_level;
    bool info_mode;
    int d_max_trials;

    float snr;
    float total_snr;
    float precision;
    dvbs2_constellation_t constellation;
    std::unique_ptr<Modulation<gr_complex, int8_t>> mod;

    unsigned int frame_size;
    unsigned int mod_order;
    unsigned int rowaddr[5];
    void get_rows(dvbs2_framesize_t, dvbs2_code_rate_t, dvbs2_constellation_t);

    unsigned int frame;
    unsigned int chunk;
    unsigned int total_trials;

    dvbs2_framesize_t framesize;
    dvbs2_code_rate_t rate;
    std::unique_ptr<LDPCInterface> ldpc;

    int d_simd_size;
    std::vector<int8_t> tempu;
    std::vector<int8_t> tempv;
    std::vector<int8_t> soft;
    void* aligned_buffer;
    int (*decode)(void*, int8_t*, int);
    void (*init)(LDPCInterface*);

public:
    ldpc_decoder_hybrid_cb_impl(bool infomode, int max_trials, int debug_level);
    ~ldpc_decoder_hybrid_cb_impl();

    // Where all the action really happens
    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;

    float get_snr() override { return snr; }

    unsigned int get_average_trials() override { return total_trials / chunk; }
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_DVBS2ACM_LDPC_DECODER_HYBRID_CB_IMPL_H */
