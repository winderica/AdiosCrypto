#pragma once

auto modExp(mpz_class a, mpz_class b, mpz_class n) {
    a = a % n;
    mpz_class result = 1;
    mpz_class x = a;
    while (b > 0) {
        if (b % 2 == 1) {
            result *= x;
            result %= n;
        }
        b >>= 1;
        x *= x;
        x %= n;
    }
    return result;
}
