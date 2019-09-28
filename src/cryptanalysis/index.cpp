#pragma once

auto enumerate(const vector<pair<uint16_t, uint16_t>> &pairs, const pair<uint16_t, uint16_t> &keyPair) {
    auto known = (keyPair.first << 8) + keyPair.second;
    uint16_t key[Nr + 1];
    for (auto i = 0; i <= 0xfffff; i++) {
        for (auto j = 0; j < 16; j++) {
            SPN::generateRoundKey((i << 12) + (j << 4) + known, key);
            if (SPN::SPN(pairs[0].first, key) == pairs[0].second) {
                if (SPN::SPN(pairs[1].first, key) == pairs[1].second) {
                    return (i << 12) + (j << 4) + known;
                }
            }
        }
    }
    return 0;
}

auto linear(const vector<pair<uint16_t, uint16_t>> &pairs) {
    vector<vector<int>> count(16, vector<int>(16));
    for (auto &[x, y]: pairs) {
        auto x5 = (x >> 11) & 1;
        auto x7 = (x >> 9) & 1;
        auto x8 = (x >> 8) & 1;
        auto Y1 = (y >> 8) & 0xf;
        auto Y3 = y & 0xf;
        for (auto i = 0; i < 16; i++) {
            for (auto j = 0; j < 16; j++) {
                auto U1 = SPN::sBoxInv[i ^ Y1];
                auto U3 = SPN::sBoxInv[j ^ Y3];
                auto u6 = (U1 >> 2) & 1;
                auto u8 = U1 & 1;
                auto u14 = (U3 >> 2) & 1;
                auto u16 = U3 & 1;
                auto z = x5 ^x7 ^x8 ^u6 ^u8 ^u14 ^u16;
                if (z == 0) {
                    count[i][j]++;
                }
            }
        }
    }
    auto max = -1;
    int size = pairs.size();
    pair<int, int> keyPair;
    for (auto i = 0; i < 16; i++) {
        for (auto j = 0; j < 16; j++) {
            count[i][j] = abs(count[i][j] - size / 2);
            if (count[i][j] > max) {
                max = count[i][j];
                keyPair = {i, j};
            }
        }
    }

    return enumerate(pairs, keyPair);
}

auto differential(const vector<tuple<uint16_t, uint16_t, uint16_t, uint16_t>> &tuples) {
    vector<vector<int>> count(16, vector<int>(16));
    for (auto &[x, y, xx, yy]: tuples) {
        auto Y1 = (y >> 12) & 0xf;
        auto Y2 = (y >> 8) & 0xf;
        auto Y3 = (y >> 4) & 0xf;
        auto Y4 = y & 0xf;
        auto YY1 = (yy >> 12) & 0xf;
        auto YY2 = (yy >> 8) & 0xf;
        auto YY3 = (yy >> 4) & 0xf;
        auto YY4 = yy & 0xf;
        if (Y1 == YY1 && Y3 == YY3) {
            for (auto i = 0; i < 16; i++) {
                for (auto j = 0; j < 16; j++) {
                    auto U2 = SPN::sBoxInv[i ^ Y2] ^SPN::sBoxInv[i ^ YY2];
                    auto U4 = SPN::sBoxInv[j ^ Y4] ^SPN::sBoxInv[j ^ YY4];
                    if (U2 == 0b0110 && U4 == 0b0110) {
                        count[i][j]++;
                    }
                }
            }
        }
    }

    auto max = -1;
    pair<int, int> keyPair;
    for (auto i = 0; i < 16; i++) {
        for (auto j = 0; j < 16; j++) {
            if (count[i][j] > max) {
                max = count[i][j];
                keyPair = {i, j};
            }
        }
    }
    vector<pair<uint16_t, uint16_t>> pairs;
    pairs.emplace_back(get<0>(tuples[0]), get<1>(tuples[0]));
    pairs.emplace_back(get<2>(tuples[0]), get<3>(tuples[0]));
    return enumerate(pairs, keyPair);
}
