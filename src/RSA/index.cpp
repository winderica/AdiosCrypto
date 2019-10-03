#pragma once

#include "../utils/generatePrime.cpp"
#include "../utils/montgomery.cpp"

struct SimpleRSA {
    mpz_class p;
    mpz_class q;
    mpz_class ql;
    mpz_class pi;
    mpz_class qi;
    mpz_class dp;
    mpz_class dq;
    mpz_class e = 65537;
    mpz_class d;
    mpz_class n;

    auto encrypt(const mpz_class &m) {
        return modExp(m, e, n);
    }

    auto decrypt(const mpz_class &c) {
        return modExp(c, d, n);
    }

    auto decryptMont(const mpz_class &c) {
        return montExp2(c, d, n);
    }

    mpz_class decryptCRT(const mpz_class &c) {
        mpz_class m1 = modExp(c, dp, p);
        mpz_class m2 = modExp(c, dq, q);
        mpz_class h = qi * (m1 - m2) % p;
//        if (m1 < m2) {
//            h = qi * (m1 + ql - m2) % p;
//        } else {
//            h = qi * (m1 - m2) % p;
//        }
        return m2 + h * q;
    }

    void generateParams() {
        srandom(time(nullptr));
        p = generatePrime(random());
        q = generatePrime(random());
        pi = modInv(p, q);
        qi = modInv(q, p);
        ql = q / p * p;
        n = p * q;
        auto phi = (p - 1) * (q - 1);
        d = modInv(e, phi);
        dp = d % (p - 1);
        dq = d % (q - 1);
    }

    SimpleRSA() {
        generateParams();
    }
};