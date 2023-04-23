#include <gnuradio/dvbs2acm/dvbs2_config.h>
#include <cstdint>
#include <vector>

namespace gr {
namespace dvbs2 {

struct ldpc_std_values {
public:
    unsigned int frame_size;
    unsigned int frame_size_real;
    unsigned int kbch;
    unsigned int nbch;
    unsigned int bch_code;

    unsigned int q_val;
    int Xs;
    int P;
    int Xp;
    std::vector<std::vector<int>> table;

private:
    ldpc_std_values(unsigned int frame_size,
                    unsigned int frame_size_real,
                    unsigned int kbch,
                    unsigned int nbch,
                    unsigned int bch_code,
                    unsigned int q_val,
                    int Xs,
                    int P,
                    int Xp,
                    std::vector<std::vector<int>> table);

public:
    static ldpc_std_values std(gr::dvbs2::dvbs2_framesize_t, gr::dvbs2::dvbs2_code_rate_t);

    // FECFRAME_NORMAL
    static const ldpc_std_values ldpc_tab_1_4N;
    static const ldpc_std_values ldpc_tab_1_3N;
    static const ldpc_std_values ldpc_tab_2_5N;
    static const ldpc_std_values ldpc_tab_1_2N;
    static const ldpc_std_values ldpc_tab_3_5N;
    static const ldpc_std_values ldpc_tab_2_3N;
    static const ldpc_std_values ldpc_tab_3_4N;
    static const ldpc_std_values ldpc_tab_4_5N;
    static const ldpc_std_values ldpc_tab_5_6N;
    static const ldpc_std_values ldpc_tab_8_9N;
    static const ldpc_std_values ldpc_tab_9_10N;

    // FECFRAME_SHORT
    static const ldpc_std_values ldpc_tab_1_4S;
    static const ldpc_std_values ldpc_tab_1_3S;
    static const ldpc_std_values ldpc_tab_2_5S;
    static const ldpc_std_values ldpc_tab_1_2S;
    static const ldpc_std_values ldpc_tab_3_5S;
    static const ldpc_std_values ldpc_tab_2_3S;
    static const ldpc_std_values ldpc_tab_3_4S;
    static const ldpc_std_values ldpc_tab_4_5S;
    static const ldpc_std_values ldpc_tab_5_6S;
    static const ldpc_std_values ldpc_tab_8_9S;
};

} // namespace dvbs2
} // namespace gr