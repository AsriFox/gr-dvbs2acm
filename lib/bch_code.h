#include <gnuradio/dvbs2acm/dvbs2_config.h>

namespace gr {
namespace dvbs2acm {

struct bch_code {
public:
    unsigned int kbch;
    unsigned int nbch;
    unsigned int code;
    unsigned int shift[6];
    unsigned int m_poly[6];

    /*
     * Reset the shift register with all zeros
     */
    void reset();

    /*
     * Shift the 192 bits register
     */
    void reg_shift_6();

    /*
     * Shift the 160 bits register
     */
    void reg_shift_5();

    /*
     * Shift the 128 bits register
     */
    void reg_shift_4();

    /*
     * Encode the baseband frame.
     * Input must contain at least kbch elements.
     */
    void encode(const unsigned char* in, unsigned char* out);

private:
    bch_code(unsigned int kbch, unsigned int nbch, unsigned int code);

public:
    static bch_code select(dvbs2_framesize_t, dvbs2_code_rate_t);

    // FECFRAME_NORMAL
    static const bch_code bch_code_1_4N;
    static const bch_code bch_code_1_3N;
    static const bch_code bch_code_2_5N;
    static const bch_code bch_code_1_2N;
    static const bch_code bch_code_3_5N;
    static const bch_code bch_code_2_3N;
    static const bch_code bch_code_3_4N;
    static const bch_code bch_code_4_5N;
    static const bch_code bch_code_5_6N;
    static const bch_code bch_code_8_9N;
    static const bch_code bch_code_9_10N;

    // FECFRAME_SHORT
    static const bch_code bch_code_1_4S;
    static const bch_code bch_code_1_3S;
    static const bch_code bch_code_2_5S;
    static const bch_code bch_code_1_2S;
    static const bch_code bch_code_3_5S;
    static const bch_code bch_code_2_3S;
    static const bch_code bch_code_3_4S;
    static const bch_code bch_code_4_5S;
    static const bch_code bch_code_5_6S;
    static const bch_code bch_code_8_9S;
};

} // namespace dvbs2acm
} // namespace gr