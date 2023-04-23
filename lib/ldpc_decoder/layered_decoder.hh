/* -*- c++ -*- */
/*
 * Copyright 2018,2019 Ahmet Inan, Ron Economos.
 *
 * This file is part of gr-dvbs2rx.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef LAYERED_DECODER_HH
#define LAYERED_DECODER_HH

#include "ldpc.hh"
#include <stdlib.h>

template <typename TYPE, typename ALG>
class LDPCDecoder
{
    typedef typename TYPE::value_type code_type;
    TYPE *bnl, *pty;
    uint16_t* pos;
    uint8_t* cnc;
    ALG alg;
    int M, N, K, R, q, CNL, LT;
    bool initialized;

    void reset()
    {
        for (int i = 0; i < LT; ++i)
            bnl[i] = alg.zero();
    }
    bool bad(TYPE* data, TYPE* parity, int blocks)
    {
        for (int i = 0; i < q; ++i) {
            int cnt = cnc[i];
            for (int j = 0; j < M; ++j) {
                TYPE cnv = alg.sign(alg.one(), parity[M * i + j]);
                if (i)
                    cnv = alg.sign(cnv, parity[M * (i - 1) + j]);
                else if (j)
                    cnv = alg.sign(cnv, parity[j + (q - 1) * M - 1]);
                for (int c = 0; c < cnt; ++c)
                    cnv = alg.sign(cnv, data[pos[CNL * (M * i + j) + c]]);
                if (alg.bad(cnv, blocks))
                    return true;
            }
        }
        return false;
    }
    void update(TYPE* data, TYPE* parity)
    {
        TYPE* bl = bnl;
        for (int i = 0; i < q; ++i) {
            int cnt = cnc[i];
            for (int j = 0; j < M; ++j) {
                int deg = cnt + 2 - !(i | j);
                TYPE inp[deg], out[deg];
                for (int c = 0; c < cnt; ++c)
                    inp[c] = out[c] = alg.sub(data[pos[CNL * (M * i + j) + c]], bl[c]);
                inp[cnt] = out[cnt] = alg.sub(parity[M * i + j], bl[cnt]);
                if (i)
                    inp[cnt + 1] = out[cnt + 1] =
                        alg.sub(parity[M * (i - 1) + j], bl[cnt + 1]);
                else if (j)
                    inp[cnt + 1] = out[cnt + 1] =
                        alg.sub(parity[j + (q - 1) * M - 1], bl[cnt + 1]);
                alg.finalp(out, deg);
                for (int c = 0; c < cnt; ++c)
                    data[pos[CNL * (M * i + j) + c]] = alg.add(inp[c], out[c]);
                parity[M * i + j] = alg.add(inp[cnt], out[cnt]);
                if (i)
                    parity[M * (i - 1) + j] = alg.add(inp[cnt + 1], out[cnt + 1]);
                else if (j)
                    parity[j + (q - 1) * M - 1] = alg.add(inp[cnt + 1], out[cnt + 1]);
                for (int d = 0; d < deg; ++d)
                    alg.update(bl++, out[d]);
            }
        }
    }

    void serial_to_parallel(TYPE* data, code_type* code)
    {
        for (int n = 0; n < TYPE::SIZE; n++) {
            for (int j = 0; j < N; j++) {
                reinterpret_cast<code_type*>(data + j)[n] = code[(n * N) + j];
            }
        }
    }

    void parallel_to_serial(TYPE* data, code_type* code)
    {
        for (int n = 0; n < TYPE::SIZE; n++) {
            for (int j = 0; j < N; j++) {
                code[(n * N) + j] = reinterpret_cast<code_type*>(data + j)[n];
            }
        }
    }

public:
    LDPCDecoder() : initialized(false) {}
    void init(LDPCInterface* it)
    {
        if (initialized) {
            free(bnl);
            free(pty);
            delete[] cnc;
            delete[] pos;
        }
        initialized = true;
        LDPCInterface* ldpc = it->clone();
        N = ldpc->code_len();
        K = ldpc->data_len();
        M = ldpc->group_len();
        R = N - K;
        q = R / M;
        CNL = ldpc->links_max_cn() - 2;
        pos = new uint16_t[R * CNL];
        cnc = new uint8_t[R];
        for (int i = 0; i < R; ++i)
            cnc[i] = 0;
        ldpc->first_bit();
        for (int j = 0; j < K; ++j) {
            int* acc_pos = ldpc->acc_pos();
            int bit_deg = ldpc->bit_deg();
            for (int n = 0; n < bit_deg; ++n) {
                int i = acc_pos[n];
                pos[CNL * i + cnc[i]++] = j;
            }
            ldpc->next_bit();
        }
        LT = ldpc->links_total();
        delete ldpc;
        bnl = reinterpret_cast<TYPE*>(aligned_alloc(sizeof(TYPE), sizeof(TYPE) * LT));
        pty = reinterpret_cast<TYPE*>(aligned_alloc(sizeof(TYPE), sizeof(TYPE) * R));
        uint16_t* tmp = new uint16_t[R * CNL];
        for (int i = 0; i < q; ++i)
            for (int j = 0; j < M; ++j)
                for (int c = 0; c < CNL; ++c)
                    tmp[CNL * (M * i + j) + c] = pos[CNL * (q * j + i) + c];
        delete[] pos;
        pos = tmp;
    }
    int operator()(void* buffer, code_type* code, int trials = 25)
    {
        const int blocks = TYPE::SIZE;
        TYPE* data = reinterpret_cast<TYPE*>(buffer);
        TYPE* parity = data + K;
        serial_to_parallel(data, code);
        reset();
        for (int i = 0; i < q; ++i)
            for (int j = 0; j < M; ++j)
                pty[M * i + j] = parity[q * j + i];
        while (bad(data, pty, blocks) && --trials >= 0)
            update(data, pty);
        for (int i = 0; i < q; ++i)
            for (int j = 0; j < M; ++j)
                parity[q * j + i] = pty[M * i + j];
        parallel_to_serial(data, code);
        return trials;
    }
    ~LDPCDecoder()
    {
        if (initialized) {
            free(bnl);
            free(pty);
            delete[] cnc;
            delete[] pos;
        }
    }
};

#endif
