#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2023 AsriFox.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from gnuradio import analog
from gnuradio import dvbs2acm
from gnuradio import gr
from gnuradio.filter import firdes
from gnuradio.fft import window
import sys
import signal

class rx_hier(gr.hier_block2):
    """
    DVB-S2 ACM Receiver stack.
    Prototype flowgraph: gr-dvbs2acm/examples/dvbs2acm_rx_hier.py
    """
    def __init__(self, rolloff=dvbs2acm.RO_0_20, goldcode=0, sps=2.0, agc_rate=1e-5, agc_gain=1, agc_ref=1, 
                 sym_sync_loop_bw=0.0045, sym_sync_damping=1, sym_sync_rrc_delay=5, sym_sync_n_subfilt=128, 
                 sym_sync_interp_method=0, pl_freq_est_period=20, ldpc_max_trials=25, debug=0):

        gr.hier_block2.__init__(self, 
            "DVB-S2 ACM Receiver",
            gr.io_signature(1, 1, gr.sizeof_gr_complex*1),
            gr.io_signature.makev(3, 3, [gr.sizeof_char*1, gr.sizeof_gr_complex*1, gr.sizeof_gr_complex*1])
        )

        ##################################################
        # Parameters
        ##################################################
        self.debug = debug
        self.goldcode = goldcode
        self.rolloff = rolloff
        self.sps = sps
        self.agc_gain = agc_gain
        self.agc_rate = agc_rate
        self.agc_ref = agc_ref
        self.sym_sync_damping = sym_sync_damping
        self.sym_sync_loop_bw = sym_sync_loop_bw
        self.sym_sync_rrc_delay = sym_sync_rrc_delay
        self.sym_sync_n_subfilt = sym_sync_n_subfilt
        self.sym_sync_interp_method = sym_sync_interp_method
        self.pl_freq_est_period = pl_freq_est_period
        self.ldpc_max_trials = ldpc_max_trials

        ##################################################
        # Variables
        ##################################################
        self.ro_factor = self.eval_ro_factor(self.rolloff)
        self.pl_enable_acm = True
        self.pl_enable_mis = False

        ##################################################
        # Blocks
        ##################################################
        self.analog_agc_xx_0 = analog.agc_cc(self.agc_rate, self.agc_ref, self.agc_gain)
        self.analog_agc_xx_0.set_max_gain(65536)

        self.dvbs2acm_rotator_cc_0 = dvbs2acm.rotator_cc(0.0, False)

        self.dvbs2acm_symbol_sync_cc_0 = dvbs2acm.symbol_sync_cc(
            self.sps, self.sym_sync_loop_bw, self.sym_sync_damping, self.ro_factor, 
            self.sym_sync_rrc_delay, self.sym_sync_n_subfilt, self.sym_sync_interp_method)

        self.dvbs2acm_plsync_cc_0 = dvbs2acm.plsync_cc(
            self.goldcode, self.pl_freq_est_period, self.sps, 
            self.debug, self.pl_enable_acm, self.pl_enable_mis, 
            0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF)

        self.dvbs2acm_ldpc_decoder_hybrid_cb_0 = dvbs2acm.ldpc_decoder_hybrid_cb(True, self.ldpc_max_trials, self.debug)
        
        self.dvbs2acm_bch_decoder_bb_1 = dvbs2acm.bch_decoder_bb(self.debug)
        
        self.dvbs2acm_bbdescrambler_bb_0 = dvbs2acm.bbdescrambler_bb()
        
        self.dvbs2acm_bbdeheader_bb_0 = dvbs2acm.bbdeheader_bb(debug)

        ##################################################
        # Connections
        ##################################################
        self.connect((self, 0), (self.analog_agc_xx_0, 0))

        self.connect((self.analog_agc_xx_0, 0), (self.dvbs2acm_rotator_cc_0, 0))

        self.connect((self.dvbs2acm_rotator_cc_0, 0), (self.dvbs2acm_symbol_sync_cc_0, 0))

        self.connect((self.dvbs2acm_symbol_sync_cc_0, 0), (self.dvbs2acm_plsync_cc_0, 0))
        self.connect((self.dvbs2acm_symbol_sync_cc_0, 0), (self, 1))

        self.connect((self.dvbs2acm_plsync_cc_0, 0), (self.dvbs2acm_ldpc_decoder_hybrid_cb_0, 0))
        self.connect((self.dvbs2acm_plsync_cc_0, 0), (self, 2))
        self.msg_connect((self.dvbs2acm_plsync_cc_0, 'rotator_phase_inc'), (self.dvbs2acm_rotator_cc_0, 'cmd'))

        self.connect((self.dvbs2acm_ldpc_decoder_hybrid_cb_0, 0), (self.dvbs2acm_bch_decoder_bb_1, 0))

        self.connect((self.dvbs2acm_bch_decoder_bb_1, 0), (self.dvbs2acm_bbdescrambler_bb_0, 0))

        self.connect((self.dvbs2acm_bbdescrambler_bb_0, 0), (self.dvbs2acm_bbdeheader_bb_0, 0))

        self.connect((self.dvbs2acm_bbdeheader_bb_0, 0), (self, 0))

    # def get_debug(self):
    #     return self.debug

    # def set_debug(self, debug):
    #     self.debug = debug

    # def get_goldcode(self):
    #     return self.goldcode

    # def set_goldcode(self, goldcode):
    #     self.goldcode = goldcode
    #     self.dvbs2acm_plsync_cc_0.set_gold_code(goldcode)

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

