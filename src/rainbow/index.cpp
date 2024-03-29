#include <openssl/md5.h>

#define hashTime 1000
#define passwordLength 7
#define charsetSize 36
unsigned char charset[charsetSize + 1] = "0123456789abcdefghijklmnopqrstuvwxyz";
#define progress "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define progressWidth 60

void printProgress(double percentage) {
    int val = (int) (percentage * 100);
    int left = (int) (percentage * progressWidth);
    int right = progressWidth - left;
    printf("\r%3d%% [%.*s%*s]", val, left, progress, right, "");
    fflush(stdout);
}

void reduction(unsigned char *res, const unsigned char *hash, unsigned int i) {
    for (auto j = 0; j < passwordLength; j++) {
        unsigned char a = hash[j] ^hash[j + 1] ^hash[j + 2] ^hash[j + 3];
        unsigned char b = hash[i & 0xf];
        res[j] = charset[(a + b) % charsetSize];
    }
}

void generateTable(int total) {
    cout << endl << "Generating table." << endl;
    auto fp = fopen("../examples/table.txt", "wb");
    unsigned char head[passwordLength], tail[passwordLength], md5[MD5_DIGEST_LENGTH];
    for (auto j = 0; j < total; j++) {
        printProgress((double) j / total);
        for (auto i = 0; i < passwordLength; i++) {
            head[i] = tail[i] = charset[randMT() % charsetSize];
        }
        for (auto i = 0; i < hashTime; i++) {
            MD5(tail, passwordLength, md5);
            reduction(tail, md5, i);
        }
        fwrite(head, passwordLength, sizeof(unsigned char), fp);
        fwrite(tail, passwordLength, sizeof(unsigned char), fp);
    }
    cout << endl << "Finished." << endl;
    fclose(fp);
}

auto findTable(const unsigned char *hash) {
    FILE *fp = fopen("../examples/table.txt", "rb");
    unsigned char tail[passwordLength], res[passwordLength], md5[MD5_DIGEST_LENGTH], buf[passwordLength * 2];
    for (auto t = hashTime - 1; t >= 0; t--) {
        rewind(fp);
        printProgress(1 - (double) t / hashTime);
        strncpy((char *) md5, (char *) hash, MD5_DIGEST_LENGTH);
        for (auto i = t; i < hashTime - 1; i++) {
            reduction(tail, md5, i);
            MD5(tail, passwordLength, md5);
        }
        reduction(tail, md5, hashTime - 1);
        while (fread(buf, passwordLength * 2, sizeof(unsigned char), fp) > 0) {
            unsigned char buf2[passwordLength];
            strncpy((char *) buf2, (char *) buf + passwordLength, passwordLength);
            if (strncmp((char *) buf2, (char *) tail, passwordLength) == 0) {
                strncpy((char *) res, (char *) buf, passwordLength);
                for (auto i = 0; i < t; i++) {
                    MD5(res, passwordLength, md5);
                    reduction(res, md5, i);
                }
                MD5(res, passwordLength, md5);
                if (strncmp((char *) md5, (char *) hash, passwordLength) == 0) {
                    fclose(fp);
                    cout << endl;
                    cout << "Bruteforced: password " << string((char *) res, passwordLength) << " hash ";
                    for (auto j : md5) {
                        printf("%02hhx", j);
                    }
                    cout << endl;
                    return true;
                }
            }
        }
    }
    fclose(fp);
    cout << endl << "Failed." << endl;
    return false;
}

void bruteforce() {
    auto success = 0;
    for (auto i = 0; i < 10; i++) {
        unsigned char password[passwordLength], hash[MD5_DIGEST_LENGTH];
        for (unsigned char &j : password) {
            j = charset[randMT() % charsetSize];
        }
        MD5(password, passwordLength, hash);
        cout << "Round " << i + 1 << ": password " << string((char *) password, passwordLength) << " hash ";
        for (auto j : hash) {
            printf("%02hhx", j);
        }
        cout << endl;
        if (findTable(hash)) {
            success++;
        }
    }
    cout << "Success rate " << success * 10 << "%" << endl;
}
