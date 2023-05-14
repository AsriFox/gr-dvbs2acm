/* -*- c++ -*- */
/*
 * Copyright 2023 AsriFox.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef DVBS2ACM_FRAME_STREAM_H
#define DVBS2ACM_FRAME_STREAM_H

#include <gnuradio/dvbs2acm/dvbs2_config.h>

#define FRAMESTREAM_BLOCK_DEF(NAME)                             \
    gr::block(NAME,                                             \
              gr::io_signature::make(1, 1, sizeof(input_type)), \
              gr::io_signature::make(1, 1, sizeof(output_type)))

#define FRAMESTREAM_GENERAL_WORK()                       \
    general_work(int noutput_items,                      \
                 gr_vector_int& ninput_items,            \
                 gr_vector_const_void_star& input_items, \
                 gr_vector_void_star& output_items)

#define FRAMESTREAM_SPECIFIC_WORK()         \
    work(dvbs2_modcod_t modcod,             \
         dvbs2_vlsnr_header_t vlsnr_header, \
         const input_type* in,              \
         output_type* out,                  \
         int& consumed,                     \
         int& produced)

#define FRAMESTREAM_GENERAL_WORK_BEGIN                        \
    auto in = static_cast<const input_type*>(input_items[0]); \
    auto out = static_cast<output_type*>(output_items[0]);    \
    unsigned consumed_total = 0;                              \
    unsigned produced_total = 0;                              \
    dvbs2_modcod_t modcod = MC_DUMMY;                         \
    dvbs2_vlsnr_header_t vlsnr_header = VLSNR_DUMMY;          \
    std::vector<tag_t> tags;                                  \
    const uint64_t nread = this->nitems_read(0);              \
    get_tags_in_range(tags, 0, nread, nread + ninput_items[0], pmt::intern("pls"));

#define FRAMESTREAM_HANDLE_TAG                                                                      \
    if (tag.value->is_dict()) {                                                                     \
        if (pmt::dict_has_key(tag.value, pmt::intern("modcod")) &&                                  \
            pmt::dict_has_key(tag.value, pmt::intern("vlsnr_header"))) {                            \
            auto not_found = pmt::get_PMT_NIL();                                                    \
            auto modcod_r = pmt::dict_ref(tag.value, pmt::intern("modcod"), not_found);             \
            if (modcod_r == not_found) {                                                            \
                continue;                                                                           \
            }                                                                                       \
            modcod = (dvbs2_modcod_t)pmt::to_long(modcod_r);                                        \
            auto vlsnr_header_r = pmt::dict_ref(tag.value, pmt::intern("vlsnr_header"), not_found); \
            if (vlsnr_header_r == not_found) {                                                      \
                continue;                                                                           \
            }                                                                                       \
            vlsnr_header = (dvbs2_vlsnr_header_t)pmt::to_long(vlsnr_header_r);                      \
        }                                                                                           \
    }

#define FRAMESTREAM_PROPAGATE_TAG        \
    auto abs_offset = nitems_written(0); \
    add_item_tag(0, abs_offset, tag.key, tag.value);

#define FRAMESTREAM_PRODUCE_WORK                             \
    int consumed, produced;                                  \
    work(modcod, vlsnr_header, in, out, consumed, produced); \
    consumed_total += consumed;                              \
    produced_total += produced;                              \
    produce(0, produced);

#define FRAMESTREAM_GENERAL_WORK_END \
    consume_each(consumed_total);    \
    return produced_total;

#define FRAMESTREAM_DO_GENERAL_WORK \
    FRAMESTREAM_GENERAL_WORK_BEGIN  \
    for (tag_t tag : tags) {        \
        FRAMESTREAM_HANDLE_TAG      \
        FRAMESTREAM_PROPAGATE_TAG   \
        FRAMESTREAM_PRODUCE_WORK    \
    }                               \
    FRAMESTREAM_GENERAL_WORK_END

#endif /* DVBS2ACM_FRAME_STREAM_H */