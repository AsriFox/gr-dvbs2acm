#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2023 AsriFox.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from gnuradio import gr, gr_unittest
from gnuradio import analog, blocks
from gnuradio.dvbs2acm import tx_hier, RO_0_15, RO_0_20

class qa_tx_hier(gr_unittest.TestCase):

    def _setUp(self, modcod=2, pilots=True, rolloff=RO_0_20, goldcode=0, sym_rate=64800, sps=2.0):
        self.tb = gr.top_block()

        self.source = analog.random_uniform_source_b(0, 256, 0)
        self.tx_hier = tx_hier(modcod, pilots, rolloff, sym_rate, goldcode)
        self.sink = blocks.probe_rate(gr.sizeof_gr_complex*1, 1000.0, 0.15)
        # self.sink_print = blocks.message_debug(True)

        self.tb.connect(self.source, self.tx_hier, self.sink)

    def setUp(self):
        self._setUp()

    def tearDown(self):
        self.tb = None

    def test_instance(self):
        instance = tx_hier(
            modcod=32,
            pilots=False,
            rolloff=RO_0_15,
            sym_rate=64800*16,
            goldcode=256,
        )

    # def test_001_descriptive_test_name(self):
    #     # set up fg
    #     self.tb.run()
    #     # check data


if __name__ == '__main__':
    gr_unittest.run(qa_tx_hier)
