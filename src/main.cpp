#include <bitset>
#include <vector>
#include <tuple>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <gmpxx.h>
#include <memory>
#include <cassert>
#include <random>

using namespace std;

#define Nr 4
#define Mr 14

#include "SPN/index.cpp"
#include "SPN/enhanced.cpp"
#include "cryptanalysis/index.cpp"
#include "RSA/index.cpp"
#include "utils/randomTest.cpp"
#include "PGP/index.cpp"
#include "rainbow/index.cpp"

void printMenu() {
    cout <<
         "\033[1;31m   ____                  _        \033[0m\n"
         "\033[1;32m  / ___|_ __ _   _ _ __ | |_ ___  \033[0m\n"
         "\033[1;33m | |   | '__| | | | '_ \\| __/ _ \\ \033[0m\n"
         "\033[1;34m | |___| |  | |_| | |_) | || (_) |\033[0m\n"
         "\033[1;35m  \\____|_|   \\__, | .__/ \\__\\___/ \033[0m\n"
         "\033[1;36m             |___/|_|             \033[0m\n"
         "===================================================\n"
         "1: SPN                                2: Linear Attack\n"
         "3: Differential Attack                4: Enhanced SPN\n"
         "5: SPN Random Test                    6: Enhanced SPN Random Test\n"
         "7: RSA                                8: RSA With Montgomery \n"
         "9: RSA With Chinese Remain Theorem    10: AES\n"
         "11: PGP                               12: Generate Rainbow Table\n"
         "13: Bruteforce with Rainbow Table     0: quit\n"
         "===================================================\n";
}

int main() {
    int option;
    SimpleRSA rsa;
    do {
        printMenu();
        cout << "Please input your option[0-13]:" << endl;
        int isValid = scanf("%d", &option);
        if (!isValid) {
            cout << "Invalid input!" << endl;
            while (getchar() != '\n');
        } else {
            if (option == 0) {
                break;
            }
            switch (option) {
                case 1: {
                    cout << "SPN" << endl;
                    cout << "===========================" << endl;
                    uint16_t roundKey[Nr + 1];
                    auto plain = 0x26b7;
                    auto key = 0x3a94d63f;
                    SPN::generateRoundKey(key, roundKey);
                    auto start = chrono::system_clock::now();
                    printf("Plain: %x\nKey: %x\nCipher: %x\n", plain, key, SPN::SPN(plain, roundKey));
                    auto end = chrono::system_clock::now();
                    cout << "Time: " << chrono::duration<double>(end - start).count() << "s" << endl;
                    cout << "===========================" << endl;
                    getchar();
                    break;
                }
                case 2: {
                    auto key = 0x3a94d63f;
                    uint16_t roundKey[Nr + 1];
                    SPN::generateRoundKey(key, roundKey);
                    cout << "Linear Attack" << endl;
                    cout << "===========================" << endl;
                    auto T = 8000;
                    vector<pair<uint16_t, uint16_t>> pairs;
                    for (int i = 0; i < T; i++) {
                        uint16_t x = randMT() % 0xffff;
                        pairs.emplace_back(x, SPN::SPN(x, roundKey));
                    }
                    auto start = chrono::system_clock::now();
                    printf("Bruteforced key: %x\n", linear(pairs));
                    auto end = chrono::system_clock::now();
                    cout << "Time: " << chrono::duration<double>(end - start).count() << "s" << endl;
                    cout << "===========================" << endl;
                    getchar();
                    break;
                }
                case 3: {
                    auto key = 0x3a94d63f;
                    uint16_t roundKey[Nr + 1];
                    auto T = 8000;
                    SPN::generateRoundKey(key, roundKey);
                    cout << "Differential Attack" << endl;
                    cout << "===========================" << endl;
                    vector<tuple<uint16_t, uint16_t, uint16_t, uint16_t>> tuples;
                    for (int i = 0; i < T; i++) {
                        uint16_t x = randMT() % 0xffff;
                        uint16_t y = x ^0x0b00;
                        tuples.emplace_back(x, SPN::SPN(x, roundKey), y, SPN::SPN(y, roundKey));
                    }
                    auto start = chrono::system_clock::now();
                    printf("Bruteforced key: %x\n", differential(tuples));
                    auto end = chrono::system_clock::now();
                    cout << "Time: " << chrono::duration<double>(end - start).count() << "s" << endl;
                    cout << "===========================" << endl;
                    getchar();
                    break;
                }
                case 4: {
                    cout << "Enhanced SPN" << endl;
                    cout << "===========================" << endl;
                    auto plain = "00000000000000000000000000000000";
                    auto key = "0000000000000000000000000000000000000000000000000000000000000000";
                    uint8_t roundKey[16 * (Mr + 1)];
                    enhancedSPN::generateRoundKey(hex2Bytes(key).data(), roundKey);
                    auto bytesVector = hex2Bytes(plain);
                    uint8_t bytes[bytesVector.size()];
                    copy(bytesVector.begin(), bytesVector.end(), bytes);
                    auto start = chrono::system_clock::now();
                    enhancedSPN::SPN(bytes, roundKey);
                    auto end = chrono::system_clock::now();
                    cout << "Plain: " << plain << endl
                         << "Key: " << key << endl
                         << "Cipher: " << bytes2Hex(bytes, bytesVector.size()) << endl;
                    cout << "Time: " << chrono::duration<double>(end - start).count() << "s" << endl;
                    cout << "===========================" << endl;
                    getchar();
                    break;
                }
                case 5: {
                    cout << "SPN Random Test" << endl;
                    cout << "===========================" << endl;
                    auto start = chrono::system_clock::now();
                    randomTest();
                    auto end = chrono::system_clock::now();
                    cout << "Time: " << chrono::duration<double>(end - start).count() << "s" << endl;
                    cout << "===========================" << endl;
                    getchar();
                    break;
                }
                case 6: {
                    cout << "Enhanced SPN Random Test" << endl;
                    cout << "===========================" << endl;
                    auto start = chrono::system_clock::now();
                    randomTestEnhanced();
                    auto end = chrono::system_clock::now();
                    cout << "Time: " << chrono::duration<double>(end - start).count() << "s" << endl;
                    cout << "===========================" << endl;
                    getchar();
                    break;
                }
                case 7: {
                    cout << "RSA 2048 (100 rounds)" << endl;
                    cout << "===========================" << endl;
                    auto message = randMT();
                    cout << "Message: " << message << endl;
                    auto cipher = rsa.encrypt(message);
                    cout << "Cipher: " << cipher << endl;
                    auto start = chrono::system_clock::now();
                    for (int i = 0; i < 100; i++) {
                        rsa.decrypt(cipher);
                    }
                    auto end = chrono::system_clock::now();
                    cout << "Time: " << chrono::duration<double>(end - start).count() / 100 << "s" << endl;
                    cout << "Decrypted: " << rsa.decrypt(cipher) << endl;
                    cout << "===========================" << endl;
                    getchar();
                    break;
                }
                case 8: {
                    cout << "RSA 2048 With Montgomery" << endl;
                    cout << "===========================" << endl;
                    auto message = randMT();
                    cout << "Message: " << message << endl;
                    auto cipher = rsa.encrypt(message);
                    cout << "Cipher: " << cipher << endl;
                    auto start = chrono::system_clock::now();
                    cout << "Decrypted: " << rsa.decryptMont(cipher) << endl;
                    auto end = chrono::system_clock::now();
                    cout << "Time: " << chrono::duration<double>(end - start).count() << "s" << endl;
                    cout << "===========================" << endl;
                    getchar();
                    break;
                }
                case 9: {
                    cout << "RSA 2048 With Chinese Remain Theorem (100 rounds)" << endl;
                    cout << "===========================" << endl;
                    auto message = randMT();
                    cout << "Message: " << message << endl;
                    auto cipher = rsa.encrypt(message);
                    cout << "Cipher: " << cipher << endl;
                    auto start = chrono::system_clock::now();
                    for (int i = 0; i < 100; i++) {
                        rsa.decryptCRT(cipher);
                    }
                    auto end = chrono::system_clock::now();
                    cout << "Time: " << chrono::duration<double>(end - start).count() / 100 << "s" << endl;
                    cout << "Decrypted: " << rsa.decryptCRT(cipher) << endl;
                    cout << "===========================" << endl;
                    getchar();
                    break;
                }
                case 10: {
                    cout << "AES" << endl;
                    cout << "===========================" << endl;
                    u_char aesIv[16];
                    RAND_bytes(aesIv, 16);
                    u_char aesKey[32];
                    RAND_bytes(aesKey, 32);
                    string plaintext = "test aes";
                    cout << "Plaintext: " << plaintext << endl;
                    auto start = chrono::system_clock::now();
                    auto cipherText = SimpleAES::encrypt(plaintext, aesKey, aesIv);
                    cout << "Cipher: " << cipherText << endl;
                    cout << SimpleAES::decrypt(cipherText, aesKey, aesIv) << endl;
                    auto end = chrono::system_clock::now();
                    cout << "Time: " << chrono::duration<double>(end - start).count() << "s" << endl;
                    cout << "===========================" << endl;
                    getchar();
                    break;
                }
                case 11: {
                    cout << "PGP" << endl;
                    cout << "===========================" << endl;
                    ifstream plainFile;
                    ofstream cipherFile;
                    ofstream decryptedFile;
                    plainFile.open("../examples/pgp_plain.txt");
                    cipherFile.open("../examples/pgp_cipher.txt");
                    decryptedFile.open("../examples/pgp_decrypted.txt");
                    stringstream buffer;
                    buffer << plainFile.rdbuf();
                    cout << "Loaded File." << endl;
                    u_char aesIv[16];
                    RAND_bytes(aesIv, 16);
                    SimpleEC ec1;
                    SimpleEC ec2;
                    auto start = chrono::system_clock::now();
                    auto pgp = encryptPGP(buffer.str(), ec2.getPublicKey().first, ec2.getPublicKey().second, ec1.getPrivateKey(), aesIv);
                    cout << "Encrypted File." << endl;
                    cipherFile << pgp;
                    decryptedFile << decryptPGP(pgp, ec2.getPrivateKey(), ec1.getPublicKey().first, aesIv);
                    cout << "Decrypted File." << endl;
                    auto end = chrono::system_clock::now();
                    cout << "Time: " << chrono::duration<double>(end - start).count() << "s" << endl;
                    plainFile.close();
                    cipherFile.close();
                    decryptedFile.close();
                    cout << "===========================" << endl;
                    getchar();
                    break;
                }
                case 12: {
                    cout << "Generate Rainbow Table" << endl;
                    cout << "===========================" << endl;
                    auto start = chrono::system_clock::now();
                    generateTable(10000000);
                    auto end = chrono::system_clock::now();
                    cout << "Time: " << chrono::duration<double>(end - start).count() << "s" << endl;
                    cout << "===========================" << endl;
                    getchar();
                    break;
                }
                case 13: {
                    cout << "Bruteforce with Rainbow Table" << endl;
                    cout << "===========================" << endl;
                    auto start = chrono::system_clock::now();
                    bruteforce();
                    auto end = chrono::system_clock::now();
                    cout << "Time: " << chrono::duration<double>(end - start).count() << "s" << endl;
                    cout << "===========================" << endl;
                    getchar();
                    break;
                }
                default: {
                    cout << "Invalid input!" << endl;
                    getchar();
                    break;
                }
            }
        }
        cout << "Type anything to go back." << endl;
        while (getchar() != '\n');
        cout << "\033c";
    } while (option);
}
