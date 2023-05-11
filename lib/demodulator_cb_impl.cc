/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "debug_level.h"
#include "demodulator_cb_impl.h"
#include "modcod.hh"
#include <gnuradio/io_signature.h>
#include <cmath>
#include <complex>
#include <stdexcept>

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
    : gr::tagged_stream_block("demodulator_cb",
                              gr::io_signature::make(1, 1, sizeof(input_type)),
                              gr::io_signature::make(1, 1, sizeof(output_type)),
                              "frame_size"),
      precision(precision)
{
    const pmt::pmt_t port_id = pmt::mp("precision");
    message_port_register_in(port_id);
    set_msg_handler(port_id, [this](pmt::pmt_t msg) { this->handle_precision_msg(msg); });

    set_tag_propagation_policy(TPP_ALL_TO_ALL);
    set_output_multiple(FRAME_SIZE_SHORT);
}

/*
 * Our virtual destructor.
 */
demodulator_cb_impl::~demodulator_cb_impl() {}

void demodulator_cb_impl::set_constellation(dvbs2_constellation_t constellation)
{
    if (this->constellation == constellation) {
        return;
    }
    this->constellation = constellation;
    switch (constellation) {
    case MOD_BPSK:
    case MOD_BPSK_SF2:
        this->mod.reset(new ModulationBPSK<gr_complex, int8_t>());
        break;
    case MOD_QPSK:
        this->mod.reset(new ModulationQPSK<gr_complex, int8_t>());
        break;
    case MOD_8PSK:
        this->mod.reset(new Modulation8PSK<gr_complex, int8_t>());
        break;
    default:
        // TODO
        break;
    }
    d_logger->debug("Modulation width: {:d}", mod->bits());
}

void demodulator_cb_impl::set_precision(float precision) { this->precision = precision; }

void demodulator_cb_impl::handle_precision_msg(pmt::pmt_t msg)
{
    gr::thread::scoped_lock l(d_mutex);

    if (!pmt::is_real(msg)) {
        throw std::runtime_error("demodulator_cb: Precision must be a float");
    }
    set_precision(pmt::to_float(msg));
}

void demodulator_cb_impl::parse_length_tags(const std::vector<std::vector<tag_t>>& tags,
                                            gr_vector_int& n_input_items_reqd)
{
    dvbs2_modcod_t modcod = MC_DUMMY;
    dvbs2_vlsnr_header_t vlsnr_header;
    for (tag_t tag : tags[0]) {
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
    }
    if (modcod == MC_VLSNR_SET1 || modcod == MC_VLSNR_SET2) {
        set_constellation(vlsnr_constellation(vlsnr_header));
        framesize = vlsnr_framesize(vlsnr_header);
    } else {
        set_constellation(modcod_constellation(modcod));
        framesize = modcod_framesize(modcod);
    }

    switch (framesize) {
    case FECFRAME_NORMAL:
        n_input_items_reqd[0] = FRAME_SIZE_NORMAL / mod->bits();
        break;
    case FECFRAME_SHORT:
        n_input_items_reqd[0] = FRAME_SIZE_SHORT / mod->bits();
        break;
    case FECFRAME_MEDIUM:
        n_input_items_reqd[0] = FRAME_SIZE_MEDIUM / mod->bits();
        break;
    }
}

int demodulator_cb_impl::calculate_output_stream_length(const gr_vector_int& ninput_items)
{
    if (mod) {
        return ninput_items[0] * mod->bits();
    }

    switch (framesize) {
    case FECFRAME_NORMAL:
        return FRAME_SIZE_NORMAL;
    case FECFRAME_SHORT:
        return FRAME_SIZE_SHORT;
    case FECFRAME_MEDIUM:
        return FRAME_SIZE_MEDIUM;
    }
}

#define FACTOR 2

int demodulator_cb_impl::work(int noutput_items,
                              gr_vector_int& ninput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    if (!mod) {
        return 0;
    }

    auto in = static_cast<const input_type*>(input_items[0]);
    auto out = static_cast<output_type*>(output_items[0]);
    int mod_bits = mod->bits();
    int8_t tmp[mod_bits];

    if (std::isinf(precision)) {
        // Determine target precision for demodulator based on noise level
        float sp = 0, np = 0;
        std::complex<float> s, e;
        for (int j = 0; j < ninput_items[0]; j++) {
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
        precision = FACTOR / (sigma * sigma);
        d_logger->debug("Detected SNR: {:.2f}", snr);
    }

    for (int j = 0; j < ninput_items[0]; j++) {
        mod->soft(tmp, in[j], precision);
        for (int n = 0; n < mod_bits; n++) {
            *out++ = tmp[n];
        }
    }
    return noutput_items;
}

} /* namespace dvbs2acm */
} /* namespace gr */
