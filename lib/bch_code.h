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
    bch_code(unsigned int code);

public:
    static bch_code select(int modcod);
    static bch_code select_normal(dvbs2_code_rate_t);
    static bch_code select_short(dvbs2_code_rate_t);
    // static bch_code select_vlsnr(dvbs2_code_rate_t);

    static const bch_code bch_code_invalid;
    static const bch_code bch_code_N8;
    static const bch_code bch_code_N10;
    static const bch_code bch_code_N12;
    static const bch_code bch_code_S12;
    static const bch_code bch_code_M12;
};

} // namespace dvbs2acm
} // namespace gr