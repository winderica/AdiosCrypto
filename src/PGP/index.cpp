#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/rand.h>
#include <zlib.h>

class Compress {
public:
    static auto compress(const string &str) {
        z_stream zs;
        memset(&zs, 0, sizeof(zs));

        if (deflateInit(&zs, Z_BEST_COMPRESSION) != Z_OK) {
            throw runtime_error("deflateInit failed while compressing.");
        }

        zs.next_in = (Bytef *) str.data();
        zs.avail_in = str.size();
        int ret;
        char buf[32768];
        string res;

        do {
            zs.next_out = reinterpret_cast<Bytef *>(buf);
            zs.avail_out = sizeof(buf);

            ret = deflate(&zs, Z_FINISH);

            if (res.size() < zs.total_out) {
                res.append(buf, zs.total_out - res.size());
            }
        } while (ret == Z_OK);

        deflateEnd(&zs);

        if (ret != Z_STREAM_END) {
            ostringstream oss;
            oss << "Exception during zlib compression: (" << ret << ") " << zs.msg;
            throw (runtime_error(oss.str()));
        }

        return res;
    }

    static auto decompress(const string &str) {
        z_stream zs;
        memset(&zs, 0, sizeof(zs));

        if (inflateInit(&zs) != Z_OK) {
            throw runtime_error("inflateInit failed while decompressing.");
        }

        zs.next_in = (Bytef *) str.data();
        zs.avail_in = str.size();

        int ret;
        char buf[32768];
        string res;

        do {
            zs.next_out = reinterpret_cast<Bytef *>(buf);
            zs.avail_out = sizeof(buf);

            ret = inflate(&zs, 0);

            if (res.size() < zs.total_out) {
                res.append(buf, zs.total_out - res.size());
            }

        } while (ret == Z_OK);

        inflateEnd(&zs);

        if (ret != Z_STREAM_END) {
            ostringstream oss;
            oss << "Exception during zlib decompression: (" << ret << ") "
                << zs.msg;
            throw runtime_error(oss.str());
        }

        return res;
    }

};

class SimpleAES {
    static const int BLOCK_SIZE = 16;

public:
    static auto encrypt(const string &plain, unsigned char *key, unsigned char *iv) {
        string cipher;
        unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> ctx(EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free);
        int rc = EVP_EncryptInit_ex(ctx.get(), EVP_aes_256_cbc(), nullptr, key, iv);
        assert(rc == 1 && "EVP_EncryptInit_ex failed");

        cipher.resize(plain.size() + BLOCK_SIZE);
        int out_len1;

        rc = EVP_EncryptUpdate(ctx.get(), (unsigned char *) &cipher[0], &out_len1, (unsigned char *) plain.c_str(), plain.size());
        assert(rc == 1 && "EVP_EncryptUpdate failed");

        int out_len2;
        rc = EVP_EncryptFinal_ex(ctx.get(), (unsigned char *) &cipher[0] + out_len1, &out_len2);
        assert(rc == 1 && "EVP_EncryptFinal_ex failed");

        cipher.resize(out_len1 + out_len2);
        return cipher;
    }

    static auto decrypt(const string &cipher, unsigned char *key, unsigned char *iv) {
        string plain;
        unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> ctx(EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free);
        int rc = EVP_DecryptInit_ex(ctx.get(), EVP_aes_256_cbc(), nullptr, key, iv);
        assert(rc == 1 && "EVP_DecryptInit_ex failed");

        plain.resize(cipher.size());
        int out_len1;

        rc = EVP_DecryptUpdate(ctx.get(), (unsigned char *) &plain[0], &out_len1, (unsigned char *) cipher.c_str(), cipher.size());
        assert(rc == 1 && "EVP_DecryptUpdate failed");

        int out_len2;
        rc = EVP_DecryptFinal_ex(ctx.get(), (unsigned char *) &plain[0] + out_len1, &out_len2);
        assert(rc == 1 && "EVP_DecryptFinal_ex failed");

        plain.resize(out_len1 + out_len2);
        return plain;
    }

    SimpleAES() {
        EVP_add_cipher(EVP_aes_256_cbc());
    }

};

bignum_ctx *ctx = BN_CTX_new();
EC_GROUP *group = EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1);

class SimpleEC {

    EC_KEY *ecKey;
public:
    BIO *out;

    SimpleEC() {
        ecKey = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
        EC_KEY_generate_key(ecKey);

        out = BIO_new(BIO_s_file());
        BIO_set_fp(out, stdout, BIO_NOCLOSE);
    }

    static auto getRandomKey() {
        auto randKey = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
        EC_KEY_generate_key(randKey);

        return randKey;
    }

    auto getPublicKey() {
        auto G = EC_POINT_new(group);
        EC_POINT_copy(G, EC_KEY_get0_public_key(ecKey));

        auto Pa = EC_POINT_new(group);
        EC_POINT_mul(group, Pa, nullptr, G, EC_KEY_get0_private_key(ecKey), ctx);

        return pair(G, Pa);
    }

    auto getPrivateKey() {
        return EC_KEY_get0_private_key(ecKey);
    }

    static auto getXY(const EC_POINT *point) {
        BIGNUM *x_a = BN_new(), *y_a = BN_new();
        EC_POINT_get_affine_coordinates_GFp(group, point, x_a, y_a, ctx);
        return pair(x_a, y_a);
    }

    static auto setXY(BIGNUM *x, BIGNUM *y) {
        EC_POINT *point = EC_POINT_new(group);
        EC_POINT_set_affine_coordinates(group, point, x, y, ctx);
        return point;
    }

    static auto encrypt(const EC_POINT *M, EC_POINT *G, EC_POINT *Pa, const BIGNUM *k) {
        EC_POINT *P = EC_POINT_new(group), *C = EC_POINT_new(group);
        EC_POINT_mul(group, P, nullptr, G, k, ctx);

        EC_POINT_mul(group, C, nullptr, Pa, k, ctx);

        EC_POINT_add(group, C, M, C, ctx);

//        BIGNUM *x_a = BN_new(), *y_a = BN_new();
//        EC_POINT_get_affine_coordinates_GFp(group, M, x_a, y_a, ctx);
//        BIO_printf(out, "Message Point P (x,y): ");
//        BN_print(out, x_a);
//        BIO_printf(out, ",");
//        BN_print(out, y_a);
//        BIO_printf(out, "\n");
//        EC_POINT_get_affine_coordinates_GFp(group, C, x_a, y_a, ctx);
//        BIO_printf(out, "Encrypted Point P (x,y): ");
//        BN_print(out, x_a);
//        BIO_printf(out, ",");
//        BN_print(out, y_a);
//        BIO_printf(out, "\n");
//        BN_free(x_a);
//        BN_free(y_a);
        return pair(P, C);
    }

    static auto decrypt(pair<EC_POINT *, EC_POINT *> cipher, const BIGNUM *Na) {
        auto P = cipher.first;
        auto C = cipher.second;
        EC_POINT_mul(group, P, nullptr, P, Na, ctx);

        EC_POINT_invert(group, P, ctx);

        EC_POINT_add(group, C, C, P, ctx);

//        BIGNUM *x_b = BN_new(), *y_b = BN_new();
//        EC_POINT_get_affine_coordinates_GFp(group, C, x_b, y_b, ctx);
//        BIO_printf(out, "Decrypted point P (x,y): ");
//        BN_print(out, x_b);
//        BIO_printf(out, ",");
//        BN_print(out, y_b);
//        BIO_printf(out, "\n");
//        BN_free(x_b);
//        BN_free(y_b);
        return C;
    }

    static auto sign(const string &message, const BIGNUM *privateKey) {
        auto key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
        EC_KEY_set_private_key(key, privateKey);
        return ECDSA_do_sign((unsigned char *) message.c_str(), message.length(), key);
    }

    static auto verify(const string &message, BIGNUM *r, BIGNUM *s, EC_POINT *publicKey) {
        auto sig = ECDSA_SIG_new();
        ECDSA_SIG_set0(sig, r, s);
        auto key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
        EC_KEY_set_public_key(key, publicKey);
        return ECDSA_do_verify((unsigned char *) message.c_str(), message.length(), sig, key) == 1;
    }
};

auto encryptPGP(const string& message, EC_POINT *G, EC_POINT *Pa, const BIGNUM *privateKey, unsigned char *iv) {
    auto compressed = Compress::compress(message);
    auto signedDigest = SimpleEC::sign(compressed, privateKey);
    compressed += BN_bn2hex(ECDSA_SIG_get0_r(signedDigest));
    compressed += BN_bn2hex(ECDSA_SIG_get0_s(signedDigest));
    unsigned char key[32];
    auto randomKey = SimpleEC::getRandomKey();
    BN_bn2bin(SimpleEC::getXY(EC_KEY_get0_public_key(randomKey)).first, key);
    auto cipher = SimpleAES::encrypt(compressed, key, iv);
    auto encryptedKey = SimpleEC::encrypt(EC_KEY_get0_public_key(randomKey), G, Pa, EC_KEY_get0_private_key(randomKey));
    auto encryptedP = SimpleEC::getXY(encryptedKey.first);
    auto encryptedC = SimpleEC::getXY(encryptedKey.second);
    return cipher + BN_bn2hex(encryptedP.first) + BN_bn2hex(encryptedP.second) + BN_bn2hex(encryptedC.first) + BN_bn2hex(encryptedC.second);
}

auto decryptPGP(const string &encrypted, const BIGNUM *Na, EC_POINT *publicKey, unsigned char *iv) {
    try {
        auto length = encrypted.length();
        auto cipher = encrypted.substr(0, length - 64 * 4);
        auto PX = encrypted.substr(length - 64 * 4, 64);
        auto PY = encrypted.substr(length - 64 * 3, 64);
        auto CX = encrypted.substr(length - 64 * 2, 64);
        auto CY = encrypted.substr(length - 64 * 1, 64);
        BIGNUM *pX = BN_new();
        BIGNUM *pY = BN_new();
        BIGNUM *cX = BN_new();
        BIGNUM *cY = BN_new();
        BN_hex2bn(&pX, PX.c_str());
        BN_hex2bn(&pY, PY.c_str());
        BN_hex2bn(&cX, CX.c_str());
        BN_hex2bn(&cY, CY.c_str());
        unsigned char key[32];
        auto encryptedKey = pair(SimpleEC::setXY(pX, pY), SimpleEC::setXY(cX, cY));
        BN_bn2bin(SimpleEC::getXY(SimpleEC::decrypt(encryptedKey, Na)).first, key);
        auto decrypted = SimpleAES::decrypt(cipher, key, iv);
        auto compressed = decrypted.substr(0, decrypted.length() - 64 * 2);
        auto digestR = decrypted.substr(decrypted.length() - 64 * 2, 64);
        auto digestS = decrypted.substr(decrypted.length() - 64 * 1, 64);
        BIGNUM *r = BN_new();
        BIGNUM *s = BN_new();
        BN_hex2bn(&r, digestR.c_str());
        BN_hex2bn(&s, digestS.c_str());
        if (SimpleEC::verify(compressed, r, s, publicKey)) {
            return Compress::decompress(compressed);
        } else {
            throw runtime_error("Verify failed");
        }
    } catch (...) {
        throw runtime_error("Failed to decrypt");
    }
}
