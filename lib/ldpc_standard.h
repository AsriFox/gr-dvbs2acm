#include <gnuradio/dvbs2acm/dvbs2_config.h>
#include <cstdint>

namespace gr {
namespace dvbs2 {

struct ldpc_std_values {
public:
    unsigned int frame_size;
    unsigned int frame_size_real;
    unsigned int kbch;
    unsigned int nbch;
    unsigned int q_val;
    unsigned int bch_code;
    int table;
    int Xs;
    int P;
    int Xp;

private:
    ldpc_std_values();

public:
    ldpc_std_values(gr::dvbs2::dvbs2_framesize_t, gr::dvbs2::dvbs2_code_rate_t);
    static ldpc_std_values from_tag(uint64_t);
};

} // namespace dvbs2
} // namespace gr