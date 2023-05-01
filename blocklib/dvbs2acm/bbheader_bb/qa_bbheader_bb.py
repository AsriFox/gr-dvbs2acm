#!/usr/bin/env python3
#
# Copyright 2023 Block Author
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from gnuradio import gr, gr_unittest, blocks, dvbs2acm

class test_bbheader_bb(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.flowgraph()

    def tearDown(self):
        self.tb = None

    def test_instantiate(self):
        # If this is a templated block, be sure to add the appropriate suffix
        op = dvbs2acm.bbheader_bb()

    def test_give_descriptive_name(self):
        # Set up a flowgraph, run, validate the results
        pass


if __name__ == '__main__':
    gr_unittest.run(test_bbheader_bb)

