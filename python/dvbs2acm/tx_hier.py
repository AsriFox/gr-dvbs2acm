#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2023 AsriFox.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from gnuradio import blocks
from gnuradio import dvbs2acm
from gnuradio import filter
from gnuradio.filter import firdes
from gnuradio import gr
# from gnuradio.fft import window
# import sys
# import signal

class tx_hier(gr.hier_block2):
    """
    DVB-S2 ACM Transmitter stack.
    Prototype flowgraph: gr-dvbs2acm/examples/dvbs2acm_tx_hier.py
    """
    def __init__(self, modcod=2, pilots=True, rolloff=dvbs2acm.RO_0_20, goldcode=0, sym_rate=64800, sps=2.0):
        gr.hier_block2.__init__(self, 
            "DVB-S2 ACM Transmitter",
            gr.io_signature(1, 1, gr.sizeof_char*1),
            gr.io_signature(1, 1, gr.sizeof_gr_complex)
        )
        self.message_port_register_hier_in("cmd")

        ##################################################
        # Parameters
        ##################################################
        self.modcod = modcod
        self.pilots = pilots
        self.rolloff = rolloff
        self.goldcode = goldcode
        self.sym_rate = sym_rate
        self.sps = sps

        ##################################################
        # Variables
        ##################################################
        self.taps = 200
        self.samp_rate = self.sym_rate * self.sps
        self.ro_factor = self.eval_ro_factor(self.rolloff)
        self.dummy_frames = False

        ##################################################
        # Blocks
        ##################################################
        self.dvbs2acm_bbheader_bb_0 = dvbs2acm.bbheader_bb(self.modcod, self.pilots, self.rolloff, self.goldcode, True)
        self.dvbs2acm_bbscrambler_bb_0 = dvbs2acm.bbscrambler_bb()
        self.dvbs2acm_bch_encoder_bb_0 = dvbs2acm.bch_encoder_bb()
        self.dvbs2acm_ldpc_encoder_bb_0 = dvbs2acm.ldpc_encoder_bb()
        self.dvbs2acm_interleaver_bb_0 = dvbs2acm.interleaver_bb()
        self.dvbs2acm_modulator_bc_0 = dvbs2acm.modulator_bc()
        self.dvbs2acm_physical_cc_0 = dvbs2acm.physical_cc(self.dummy_frames)

        self.interp_fir_filter_xxx_0 = filter.interp_fir_filter_ccc(
            int(self.sps/2), 
            self.build_fir_taps(self.samp_rate, self.ro_factor, self.taps)
        )
        self.interp_fir_filter_xxx_0.declare_sample_delay(0)

        self.blocks_throttle_0_0 = blocks.throttle(gr.sizeof_gr_complex*1, self.samp_rate, True)

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self, 'cmd'), (self.dvbs2acm_bbheader_bb_0, 'cmd'))
        self.connect((self, 0),                             (self.dvbs2acm_bbheader_bb_0, 0)    )
        self.connect((self.dvbs2acm_bbheader_bb_0, 0),      (self.dvbs2acm_bbscrambler_bb_0, 0) )
        self.connect((self.dvbs2acm_bbscrambler_bb_0, 0),   (self.dvbs2acm_bch_encoder_bb_0, 0) )
        self.connect((self.dvbs2acm_bch_encoder_bb_0, 0),   (self.dvbs2acm_ldpc_encoder_bb_0, 0))
        self.connect((self.dvbs2acm_interleaver_bb_0, 0),   (self.dvbs2acm_modulator_bc_0, 0)   )
        self.connect((self.dvbs2acm_ldpc_encoder_bb_0, 0),  (self.dvbs2acm_interleaver_bb_0, 0) )
        self.connect((self.dvbs2acm_modulator_bc_0, 0),     (self.dvbs2acm_physical_cc_0, 0)    )
        self.connect((self.dvbs2acm_physical_cc_0, 0),      (self.interp_fir_filter_xxx_0, 0)   )
        self.connect((self.interp_fir_filter_xxx_0, 0),     (self.blocks_throttle_0_0, 0)       )
        self.connect((self.blocks_throttle_0_0, 0),         (self, 0)                           )

    def get_modcod(self):
        return self.modcod

    def set_modcod(self, modcod):
        self.modcod = modcod
        self.dvbs2acm_bbheader_bb_0.set_modcod(self.modcod)

    def get_pilots(self):
        return self.pilots

    def set_pilots(self, pilots):
        self.pilots = pilots
        self.dvbs2acm_bbheader_bb_0.set_pilots(self.pilots)

    def get_rolloff(self):
        return self.rolloff

    def get_ro_factor(self):
        return self.ro_factor

    def eval_ro_factor(self, rolloff):
        if rolloff == dvbs2acm.RO_0_35:
            return 0.35
        elif rolloff == dvbs2acm.RO_0_25:
            return 0.25
        elif rolloff == dvbs2acm.RO_0_20:
            return 0.20
        elif rolloff == dvbs2acm.RO_0_15:
            return 0.15
        elif rolloff == dvbs2acm.RO_0_10:
            return 0.10
        elif rolloff == dvbs2acm.RO_0_05:
            return 0.05
        else:
            return 0.00

    def set_rolloff(self, rolloff):
        self.rolloff = rolloff
        self.ro_factor = self.eval_ro_factor(self.rolloff)
        self.dvbs2acm_bbheader_bb_0.set_rolloff(self.rolloff)
        self.interp_fir_filter_xxx_0.set_taps(
            self.build_fir_taps(self.samp_rate, self.ro_factor, self.taps)
        )

    def get_goldcode(self):
        return self.goldcode

    def set_goldcode(self, goldcode):
        self.goldcode = goldcode
        self.dvbs2acm_bbheader_bb_0.set_goldcode(self.goldcode)

    def get_sps(self):
        return self.sps

    def set_sps(self, sps):
        self.sps = sps
        self.set_samp_rate(self.sym_rate * self.sps)

    def get_sym_rate(self):
        return self.sym_rate

    def set_sym_rate(self, sym_rate):
        self.sym_rate = sym_rate
        self.set_samp_rate(self.sym_rate * self.sps)

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.blocks_throttle_0_0.set_sample_rate(self.samp_rate)
        self.interp_fir_filter_xxx_0.set_taps(
            self.build_fir_taps(self.samp_rate, self.ro_factor, self.taps)
        )

    def get_taps(self):
        return self.taps

    def set_taps(self, taps):
        self.taps = taps
        self.interp_fir_filter_xxx_0.set_taps(
            self.build_fir_taps(self.samp_rate, self.ro_factor, self.taps)
        )

    def build_fir_taps(self, samp_rate, ro_factor, taps):
        return firdes.root_raised_cosine(
            1.0,
            samp_rate,
            samp_rate / 2,
            ro_factor,
            taps
        )
