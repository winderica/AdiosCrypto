#pragma once

namespace SPN {

    int s[] = {
        0xe, 0x4, 0xd, 0x1,
        0x2, 0xf, 0xb, 0x8,
        0x3, 0xa, 0x6, 0xc,
        0x5, 0x9, 0x0, 0x7
    };

    int sBoxInv[] = {
        0xe, 0x3, 0x4, 0x8,
        0x1, 0xc, 0xa, 0xf,
        0x7, 0xd, 0x9, 0x6,
        0xb, 0x2, 0x0, 0x5
    };

    int p[] = {
        0, 4, 8, 12,
        1, 5, 9, 13,
        2, 6, 10, 14,
        3, 7, 11, 15
    };

    int pp[] = {
        15, 11, 7, 3,
        14, 10, 6, 2,
        13, 9, 5, 1,
        12, 8, 4, 0
    };

    auto sBox(uint16_t x) {
        uint16_t vr = 0;
        for (int i = 4 * (Nr - 1); i >= 0; i -= 4) {
            vr |= s[(x >> i) & 0xf] << i;
        }
        return vr;
    }

    auto pBox(uint16_t x) {
        uint16_t res = 0;
        for (int i = 15; i >= 0; i--, x >>= 1) {
            if (x & 1) {
                res |= 1 << pp[i];
            }
        }
        return res;
    }

    auto generateRoundKey(uint32_t k, uint16_t rk[]) {
        rk[0] = k >> 16;
        rk[1] = k >> 12;
        rk[2] = k >> 8;
        rk[3] = k >> 4;
        rk[4] = k;
    }

    auto SPN(uint16_t x, const uint16_t k[]) {
        uint16_t w[Nr + 1] = {x};
        uint16_t v[Nr];
        for (int r = 0; r < Nr; r++) {
            v[r] = sBox(w[r] ^ k[r]);
            if (r != Nr - 1) {
                w[r + 1] = pBox(v[r]);
            }
        }
        return v[Nr - 1] ^ k[Nr];
    }
}
