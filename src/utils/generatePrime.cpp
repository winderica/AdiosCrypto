#pragma once
#include "millerRabin.cpp"

auto generatePrime(int seed) {
    gmp_randclass randClass(gmp_randinit_default);
    randClass.seed(seed);
    mpz_class res = randClass.get_z_bits(2048);
    if (res % 2 == 0) {
        res += 1;
    }
    while (millerRabin(res, 40) == 0) {
        res += 2;
    }
    return res;
}