#ifndef INCLUDED_LDPC_ENCODE_TABLES_H
#define INCLUDED_LDPC_ENCODE_TABLES_H

#include <gnuradio/dvbs2acm/dvbs2_config.h>
#include <cstdint>
#include <vector>

namespace gr {
namespace dvbs2acm {

struct ldpc_encode_table {
public:
    unsigned int frame_size;
    unsigned int frame_size_real;
    unsigned int nbch;
    unsigned int q_val;
    int Xs;
    int P;
    int Xp;
    std::vector<std::vector<int>> table;

private:
    ldpc_encode_table(unsigned int,
                      unsigned int,
                      unsigned int,
                      unsigned int,
                      int,
                      int,
                      int,
                      std::vector<std::vector<int>>);

public:
    static ldpc_encode_table select(dvbs2_modcod_t, dvbs2_vlsnr_header_t);

    static const ldpc_encode_table ldpc_tab_invalid;
    // FECFRAME_NORMAL
    // DVB-S2
    static const ldpc_encode_table ldpc_tab_1_4N;
    static const ldpc_encode_table ldpc_tab_1_3N;
    static const ldpc_encode_table ldpc_tab_2_5N;
    static const ldpc_encode_table ldpc_tab_1_2N;
    static const ldpc_encode_table ldpc_tab_3_5N;
    static const ldpc_encode_table ldpc_tab_2_3N;
    static const ldpc_encode_table ldpc_tab_3_4N;
    static const ldpc_encode_table ldpc_tab_4_5N;
    static const ldpc_encode_table ldpc_tab_5_6N;
    static const ldpc_encode_table ldpc_tab_8_9N;
    static const ldpc_encode_table ldpc_tab_9_10N;
    // DVB-S2X
    static const ldpc_encode_table ldpc_tab_13_45N;
    static const ldpc_encode_table ldpc_tab_9_20N;
    static const ldpc_encode_table ldpc_tab_90_180N;
    static const ldpc_encode_table ldpc_tab_96_180N;
    static const ldpc_encode_table ldpc_tab_11_20N;
    static const ldpc_encode_table ldpc_tab_100_180N;
    static const ldpc_encode_table ldpc_tab_104_180N;
    static const ldpc_encode_table ldpc_tab_26_45N;
    static const ldpc_encode_table ldpc_tab_18_30N;
    static const ldpc_encode_table ldpc_tab_28_45N;
    static const ldpc_encode_table ldpc_tab_23_36N;
    static const ldpc_encode_table ldpc_tab_116_180N;
    static const ldpc_encode_table ldpc_tab_20_30N;
    static const ldpc_encode_table ldpc_tab_124_180N;
    static const ldpc_encode_table ldpc_tab_25_36N;
    static const ldpc_encode_table ldpc_tab_128_180N;
    static const ldpc_encode_table ldpc_tab_13_18N;
    static const ldpc_encode_table ldpc_tab_132_180N;
    static const ldpc_encode_table ldpc_tab_22_30N;
    static const ldpc_encode_table ldpc_tab_135_180N;
    static const ldpc_encode_table ldpc_tab_140_180N;
    static const ldpc_encode_table ldpc_tab_7_9N;
    static const ldpc_encode_table ldpc_tab_154_180N;

    // FECFRAME_SHORT
    // DVB-S2
    static const ldpc_encode_table ldpc_tab_1_4S;
    static const ldpc_encode_table ldpc_tab_1_3S;
    static const ldpc_encode_table ldpc_tab_2_5S;
    static const ldpc_encode_table ldpc_tab_1_2S;
    static const ldpc_encode_table ldpc_tab_3_5S;
    static const ldpc_encode_table ldpc_tab_2_3S;
    static const ldpc_encode_table ldpc_tab_3_4S;
    static const ldpc_encode_table ldpc_tab_4_5S;
    static const ldpc_encode_table ldpc_tab_5_6S;
    static const ldpc_encode_table ldpc_tab_8_9S;
    // DVB-S2X
    static const ldpc_encode_table ldpc_tab_11_45S;
    static const ldpc_encode_table ldpc_tab_4_15S;
    static const ldpc_encode_table ldpc_tab_14_45S;
    static const ldpc_encode_table ldpc_tab_7_15S;
    static const ldpc_encode_table ldpc_tab_8_15S;
    static const ldpc_encode_table ldpc_tab_26_45S;
    static const ldpc_encode_table ldpc_tab_32_45S;

    // VL-SNR
    static const ldpc_encode_table ldpc_tab_VLSNR_2_9N;
    static const ldpc_encode_table ldpc_tab_VLSNR_1_5M;
    static const ldpc_encode_table ldpc_tab_VLSNR_11_45M;
    static const ldpc_encode_table ldpc_tab_VLSNR_1_3M;
    static const ldpc_encode_table ldpc_tab_VLSNR_1_5SF2;
    static const ldpc_encode_table ldpc_tab_VLSNR_11_45SF2;
    static const ldpc_encode_table ldpc_tab_VLSNR_1_5S;
    static const ldpc_encode_table ldpc_tab_VLSNR_4_15S;
    static const ldpc_encode_table ldpc_tab_VLSNR_1_3S;
};

} // namespace dvbs2acm
} // namespace gr

#endif /* INCLUDED_LDPC_ENCODE_TABLES_H */