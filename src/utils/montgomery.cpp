#pragma once

#include "modInv.cpp"

mpz_class montMul(mpz_class a, mpz_class b, mpz_class n, mpz_class r, mpz_class nn) {
    mpz_class t = a * b;
    mpz_class m = t % r * nn % r;
    t += m * n;
    t /= r;
    return t % n;
}

mpz_class montMul2(mpz_class a, mpz_class b, mpz_class n, int k, mpz_class nn) {
    mpz_class t = 0;
    for (int i = 0; i < k; i += 32) {
        mpz_class ai = a & 0xffffffff;
        t += (ai * b) << i;
        mpz_class m = ((t >> i) * nn) & 0xffffffff;
        t += (m * n) << i;
        a >>= 32;
    }
    t >>= k;
    return t % n;
}

mpz_class montExp(mpz_class a, mpz_class b, mpz_class n) {
    mpz_class r = 1;
    mpz_class t = n;
    while (t) {
        r <<= 2;
        r %= n;
        t >>= 1;
    }

    mpz_class nn = -modInv(n, r);
    mpz_class A = a * r % n;
    mpz_class prod = r % n;

    while (b != 0) {
        if (b % 2 == 1) {
            prod = montMul(prod, A, n, r, nn);
        }
        b >>= 1;
        A = montMul(A, A, n, r, nn);
    }
    prod = montMul(prod, 1, n, r, nn);
    return prod < 0 ? prod + n : prod;
}

mpz_class montExp2(mpz_class a, mpz_class b, mpz_class n) {
    mpz_class r = 1;
    mpz_class t = n;
    int k = 0;
    while (t) {
        r <<= 2;
        r %= n;
        k += 2;
        t >>= 1;
    }
    mpz_class u = 1;
    u <<= 32;
    mpz_class nn = -modInv(n, u);
    mpz_class A = a * r % n;
    mpz_class prod = r;

    while (b != 0) {
        if (b % 2 == 1) {
            prod = montMul2(prod, A, n, k, nn);
        }
        b >>= 1;
        A = montMul2(A, A, n, k, nn);
    }
    prod = montMul2(prod, 1, n, k, nn);
    return prod < 0 ? prod + n : prod;
}
