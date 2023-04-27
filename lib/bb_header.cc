#include "bb_header.hh"
#include <gnuradio/dvbs2acm/dvbs2_config.h>

void build_crc8_table(unsigned char* crc_tab)
{
    const int crc_poly_rev = 0xD5;
    int r, crc;
    for (int i = 0; i < 256; i++) {
        r = i;
        crc = 0;
        for (int j = 1 << 7; j >= 1; j >>= 1) {
            if (((r & j) ? 1 : 0) ^ ((crc & 0x80) ? 1 : 0)) {
                crc = (crc << 1) ^ crc_poly_rev;
            } else {
                crc <<= 1;
            }
        }
        crc_tab[i] = crc;
    }
}

/*
 * MSB is sent first
 *
 * The polynomial has been reversed
 */
unsigned int check_crc8_bits(const unsigned char* in, int length)
{
    const int crc_poly = 0xAB;
    int crc = 0;
    int b;
    for (int i = 0; i < length; i++) {
        b = in[i] ^ (crc & 0x01);
        crc >>= 1;
        if (b) {
            crc ^= crc_poly;
        }
    }
    return crc;
}

int unpack_bits_8(int temp, unsigned char* out)
{
    int n = 1 << 7;
    for (int i = 0; i < 8; i++) {
        out[i] = (temp & n) ? 1 : 0;
        n >>= 1;
    }
    return 8;
}

int unpack_bits_16(int temp, unsigned char* out)
{
    int n = 1 << 15;
    for (int i = 0; i < 16; i++) {
        out[i] = (temp & n) ? 1 : 0;
        n >>= 1;
    }
    return 16;
}

void BBHeader::add_to_frame(unsigned char* frame, int count, bool nibble, bool dvbs2x_alternate)
{
    frame[0] = ts_gs >> 1;
    frame[1] = ts_gs & 1;
    frame[2] = sis_mis;
    frame[3] = ccm_acm;
    frame[4] = issyi & 1;
    frame[5] = npd & 1;
    if (dvbs2x_alternate) {
        frame[6] = 1;
        frame[7] = 1;
    } else {
        frame[6] = ro >> 1;
        frame[7] = ro & 1;
    }

    int offset = 8;
    if (sis_mis == SIS_MIS_MULTIPLE) {
        // TODO: NOT IMPLEMENTED
        offset += unpack_bits_8(isi, &frame[offset]);
    } else {
        for (int i = 0; i < 8; i++) {
            frame[offset++] = 0;
        }
    }

    offset += unpack_bits_16(upl, &frame[offset]);

    offset += unpack_bits_16(dfl, &frame[offset]);

    offset += unpack_bits_8(sync, &frame[offset]);

    int temp = count;
    if (count != 0) {
        temp = (TRANSPORT_PACKET_LENGTH - count) * 8;
    }
    if (!nibble) {
        temp += 4;
    }
    offset += unpack_bits_16(temp, &frame[offset]);

    // Append CRC to the end of BBHEADER
    auto crc = check_crc8_bits(frame, offset);
    unpack_bits_8(crc, &frame[offset]);
}

void BBHeader::parse(const unsigned char* in)
{
    // Parse the BBHEADER
    ts_gs = *in++ << 1;
    ts_gs |= *in++;
    sis_mis = *in++;
    ccm_acm = *in++;
    issyi = *in++;
    npd = *in++;
    ro = *in++ << 1;
    ro |= *in++;
    isi = 0;
    if (sis_mis == 0) {
        for (int n = 7; n >= 0; n--) {
            isi |= *in++ << n;
        }
    } else {
        in += 8;
    }
    upl = 0;
    for (int n = 15; n >= 0; n--) {
        upl |= *in++ << n;
    }
    dfl = 0;
    for (int n = 15; n >= 0; n--) {
        dfl |= *in++ << n;
    }
    sync = 0;
    for (int n = 7; n >= 0; n--) {
        sync |= *in++ << n;
    }
    syncd = 0;
    for (int n = 15; n >= 0; n--) {
        syncd |= *in++ << n;
    }
    in += 8; // Skip the last byte (CRC-8), processed in the beginning.
}