/*
 * Copyright 2023 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually edited  */
/* The following lines can be configured to regenerate this file during cmake      */
/* If manual edits are made, the following tags should be modified accordingly.    */
/* BINDTOOL_GEN_AUTOMATIC(0)                                                       */
/* BINDTOOL_USE_PYGCCXML(0)                                                        */
/* BINDTOOL_HEADER_FILE(bch_encoder_bb.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(ac45817df714f9f41b1e8dfe7b022745)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/dvbs2acm/bch_encoder_bb.h>
// pydoc.h is automatically generated in the build directory
#include <bch_encoder_bb_pydoc.h>

void bind_bch_encoder_bb(py::module& m)
{

    using bch_encoder_bb    = ::gr::dvbs2acm::bch_encoder_bb;


    py::class_<bch_encoder_bb, gr::block, gr::basic_block,
        std::shared_ptr<bch_encoder_bb>>(m, "bch_encoder_bb", D(bch_encoder_bb))

        .def(py::init(&bch_encoder_bb::make),
           D(bch_encoder_bb,make)
        )
        



        ;




}








