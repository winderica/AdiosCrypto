#pragma once

namespace enhancedSPN {

    int s[] = {
        15, 0, 11, 13, 6, 10, 3, 5, 7, 4, 1, 9, 12, 14, 8, 2
    };

    int sBoxInv[] = {
        1, 10, 15, 6, 9, 7, 4, 8, 14, 11, 5, 2, 12, 3, 13, 0
    };

    int p[] = {
        7, 23, 8, 10, 6, 13, 27, 9, 2, 24, 3, 22, 14, 5, 4, 12, 28, 1, 21, 19, 16, 26, 17, 11, 15, 29, 30, 20, 31, 0, 25, 18
    };

    int pp[] = {
        24, 8, 23, 21, 25, 18, 4, 22, 29, 7, 28, 9, 17, 26, 27, 19, 3, 30, 10, 12, 15, 5, 14, 20, 16, 2, 1, 11, 0, 31, 6, 13
    };

    auto sBox(uint32_t x) {
        uint32_t vr = 0;
        for (int i = 4 * (Mr - 1); i >= 0; i -= 4) {
            vr |= s[(x >> i) & 0xf] << i;
        }
        return vr;
    }

    auto pBox(uint32_t x) {
        uint32_t res = 0;
        for (int i = 31; i >= 0; i--, x >>= 1) {
            if (x & 1) {
                res |= 1 << pp[i];
            }
        }
        return res;
    }

    auto generateRoundKey(uint64_t k, uint32_t rk[]) {
        rk[0] = k >> 32;
        rk[1] = k >> 28;
        rk[2] = k >> 24;
        rk[3] = k >> 20;
        rk[4] = (k >> 16) ^ sBox(rk[3]);
        rk[5] = (k >> 12) ^ sBox(rk[4]);;
        rk[6] = (k >> 8) ^ sBox(rk[5]);;
        rk[7] = (k >> 4) ^ sBox(rk[6]);;
        rk[8] = k ^ rk[7];
    }

    auto SPN(uint32_t x, const uint32_t k[]) {
        uint32_t w[Mr + 1] = {x};
        uint32_t v[Mr];
        for (int r = 0; r < Mr; r++) {
            v[r] = sBox(w[r] ^ k[r]);
            if (r != Mr - 1) {
                w[r + 1] = pBox(v[r]);
            }
        }
        return v[Mr - 1] ^ k[Mr];
    }
}
