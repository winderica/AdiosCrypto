#pragma once

mpz_class modInv(mpz_class a, mpz_class n) {
    if (n == 1) {
        return 0;
    }
    mpz_class n0 = n, x = 1, y = 0;

    while (a > 1) {
        mpz_class q = a / n;
        mpz_class t = n;
        n = a % n;
        a = t;
        t = y;
        y = x - q * y;
        x = t;
    }
    return x < 0 ? x + n0 : x;
}
