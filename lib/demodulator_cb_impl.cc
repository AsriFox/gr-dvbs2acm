/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "demodulator_cb_impl.h"
#include "modcod.hh"
#include <gnuradio/io_signature.h>
#include <vector>

namespace gr {
namespace dvbs2acm {

using input_type = gr_complex;
using output_type = int8_t;

demodulator_cb::sptr demodulator_cb::make(float precision)
{
    return gnuradio::make_block_sptr<demodulator_cb_impl>(precision);
}


/*
 * The private constructor
 */
demodulator_cb_impl::demodulator_cb_impl(float precision)
    : gr::block("demodulator_cb",
                gr::io_signature::make(1, 1, sizeof(input_type)),
                gr::io_signature::make(1, 1, sizeof(output_type)))
{
    set_precision(precision);
    set_output_multiple(FRAME_SIZE_NORMAL);
    set_tag_propagation_policy(TPP_CUSTOM);
}

/*
 * Our virtual destructor.
 */
demodulator_cb_impl::~demodulator_cb_impl() {}

void demodulator_cb_impl::set_precision(float precision) { this->precision = precision; }

void demodulator_cb_impl::set_constellation(dvbs2_constellation_t constellation)
{
    if (this->constellation == constellation) {
        return;
    }
    this->constellation = constellation;
    switch (constellation) {
    case MOD_BPSK:
    case MOD_BPSK_SF2:
        mod.reset(new PhaseShiftKeying<2, gr_complex, int8_t>());
        break;
    case MOD_QPSK:
        mod.reset(new PhaseShiftKeying<4, gr_complex, int8_t>());
        break;
    case MOD_8PSK:
        mod.reset(new PhaseShiftKeying<8, gr_complex, int8_t>());
        break;
    default:
        // TODO
        mod.reset(nullptr);
        break;
    }
    if (mod) {
        set_relative_rate(1, mod->bits());
    }
}

void demodulator_cb_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    if (mod) {
        ninput_items_required[0] = noutput_items / mod->bits();
    } else {
        ninput_items_required[0] = noutput_items;
    }
}

#define FACTOR 2

int demodulator_cb_impl::general_work(int noutput_items,
                                      gr_vector_int& ninput_items,
                                      gr_vector_const_void_star& input_items,
                                      gr_vector_void_star& output_items)
{
    auto in = static_cast<const input_type*>(input_items[0]);
    auto out = static_cast<output_type*>(output_items[0]);

    dvbs2_modcod_t modcod = MC_DUMMY;
    dvbs2_vlsnr_header_t vlsnr_header;
    std::vector<tag_t> tags;
    get_tags_in_window(tags, 0, 0, ninput_items[0]);
    for (tag_t tag : tags) {
        if (tag.key == pmt::intern("pls") && tag.value->is_dict()) {
            auto dict = tag.value;
            if (pmt::dict_has_key(dict, pmt::intern("modcod")) &&
                pmt::dict_has_key(dict, pmt::intern("vlsnr_header"))) {
                auto not_found = pmt::get_PMT_NIL();

                auto modcod_r = pmt::dict_ref(dict, pmt::intern("modcod"), not_found);
                if (modcod_r == not_found) {
                    continue;
                }
                modcod = (dvbs2_modcod_t)pmt::to_long(modcod_r);

                auto vlsnr_header_r = pmt::dict_ref(dict, pmt::intern("vlsnr_header"), not_found);
                if (vlsnr_header_r == not_found) {
                    continue;
                }
                vlsnr_header = (dvbs2_vlsnr_header_t)pmt::to_long(vlsnr_header_r);
            }
        }
        dvbs2_framesize_t framesize;
        if (modcod == MC_VLSNR_SET1 || modcod == MC_VLSNR_SET2) {
            set_constellation(vlsnr_constellation(vlsnr_header));
            framesize = vlsnr_framesize(vlsnr_header);
        } else {
            set_constellation(modcod_constellation(modcod));
            framesize = modcod_framesize(modcod);
        }
        switch (framesize) {
        case FECFRAME_NORMAL:
            frame_size = FRAME_SIZE_NORMAL;
        case FECFRAME_SHORT:
            frame_size = FRAME_SIZE_SHORT;
        case FECFRAME_MEDIUM:
            frame_size = FRAME_SIZE_MEDIUM;
        }

        int consumed, produced;
        demodulate(in, out, consumed, produced);

        const auto tagoffset = nitems_written(0);
        add_item_tag(0, tagoffset, tag.key, tag.value);

        consume_each(consumed);
        produce(0, produced);
    }
    return WORK_CALLED_PRODUCE;
}

void demodulator_cb_impl::demodulate(const gr_complex* in, int8_t* out, int& consumed, int& produced)
{
    int mod_bits = mod->bits();
    int symbols = frame_size / mod_bits;
    int8_t tmp[mod_bits];

    if (std::isinf(precision)) {
        // Determine target precision for demodulator based on noise level
        float sp = 0, np = 0;
        std::complex<float> s, e;
        for (int j = 0; j < symbols; j++) {
            mod->hard(tmp, in[j]);
            s = mod->map(tmp);
            e = in[j] - s;
            sp += std::norm(s);
            np += std::norm(e);
        }
        if (!(np > 0)) {
            np = 1e-12;
        }
        float snr = 10 * std::log10(sp / np);
        float sigma = std::sqrt(np / (2 * sp));
        set_precision(FACTOR / (sigma * sigma));
        d_logger->debug("Detected SNR: {:.2f}", snr);
    }

    for (int j = 0; j < symbols; j++) {
        mod->soft(tmp, in[j], precision);
        for (int n = 0; n < mod_bits; n++) {
            *out++ = tmp[n];
        }
    }
    consumed = symbols;
    produced = frame_size;
}

} /* namespace dvbs2acm */
} /* namespace gr */
