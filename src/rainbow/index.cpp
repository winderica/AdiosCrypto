#include <openssl/md5.h>

#define hashTime 1000
#define passwordLength 7
#define charsetSize 36
u_char charset[charsetSize + 1] = "0123456789abcdefghijklmnopqrstuvwxyz";
#define progress "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define progressWidth 60

void printProgress(double percentage) {
    int val = (int) (percentage * 100);
    int left = (int) (percentage * progressWidth);
    int right = progressWidth - left;
    printf("\r%3d%% [%.*s%*s]", val, left, progress, right, "");
    fflush(stdout);
}

void reduction(u_char *res, const u_char *hash, u_int index) {
    for (auto i = 0; i < passwordLength; i++) {
        res[i] = charset[(hash[i] + index) % charsetSize];
    }
}

void generateTable(int total) {
    cout << endl << "Generating table." << endl;
    srandom(time(nullptr));
    auto fp = fopen("../examples/table.txt", "wb");
    u_char head[passwordLength], tail[passwordLength], md5[MD5_DIGEST_LENGTH];
    for (auto j = 0; j < total; j++) {
        printProgress((double) j / total);
        for (auto i = 0; i < passwordLength; i++) {
            head[i] = tail[i] = charset[(random() + j) % charsetSize];
        }
        for (auto i = 0; i < hashTime; i++) {
            MD5(tail, passwordLength, md5);
            reduction(tail, md5, i);
        }
        fwrite(head, passwordLength, sizeof(u_char), fp);
        fwrite(tail, passwordLength, sizeof(u_char), fp);
    }
    cout << endl << "Finished." << endl;
    fclose(fp);
}

bool findTable(const u_char *hash) {
    FILE *fp = fopen("../examples/table.txt", "rb");
    u_char tail[passwordLength], res[passwordLength], md5[MD5_DIGEST_LENGTH], buf[passwordLength * 2];
    for (auto t = hashTime - 1; t >= 0; t--) {
        rewind(fp);
        printProgress(1 - (double) t / hashTime);
        strncpy((char *) md5, (char *) hash, MD5_DIGEST_LENGTH);
        for (auto i = t; i < hashTime - 1; i++) {
            reduction(tail, md5, i);
            MD5(tail, passwordLength, md5);
        }
        reduction(tail, md5, hashTime - 1);
        while (fread(buf, passwordLength * 2, sizeof(u_char), fp) > 0) {
            u_char buf2[passwordLength];
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
    srandom(time(nullptr));
    auto success = 0;
    for (auto i = 0; i < 10; i++) {
        u_char password[passwordLength], hash[MD5_DIGEST_LENGTH];
        for (auto j = 0; j < passwordLength; j++) {
            password[j] = charset[(j + random()) % charsetSize];
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
