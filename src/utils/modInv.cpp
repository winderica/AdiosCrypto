#pragma once

mpz_class modInv(mpz_class a, mpz_class m) {
    if (m == 1)
        return 0;
    mpz_class m0 = m;
    mpz_class y = 0, x = 1;

    while (a > 1) {
        mpz_class q = a / m;
        mpz_class t = m;
        m = a % m, a = t;
        t = y;
        y = x - q * y;
        x = t;
    }
    if (x < 0)
        x += m0;
    return x;
}
