#pragma once

#include "./modExp.cpp"
#include "./randMT.cpp"

auto millerRabin(mpz_class n, int k) {
    if (n <= 2 || n % 2 == 0) {
        return false;
    }
    mpz_class r = 1;
    mpz_class d = n - 1;
    while (d % 2 == 0) {
        d >>= 1;
        r <<= 1;
    }
    gmp_randclass randClass(gmp_randinit_default);
    randClass.seed(randMT());
    for (auto i = 0; i < k; i++) {
        mpz_class a = randClass.get_z_range(n - 4) + 2;
        auto x = modExp(a, d, n);
        if (x == 1 || x == n - 1) {
            continue;
        }
        bool isComposite = true;
        for (int j = 0; j < r - 1; j++) {
            x = modExp(x, 2, n);
            if (x == n - 1) {
                isComposite = false;
                break;
            }
        }
        if (isComposite) {
            return false;
        }
    }
    return true;
}
