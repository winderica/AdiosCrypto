auto hex2Bytes(const string &hex) {
    vector<uint8_t> bytes;

    for (unsigned long i = 0; i < hex.length(); i += 2) {
        string byteString = hex.substr(i, 2);
        uint8_t byte = strtol(byteString.c_str(), nullptr, 16);
        bytes.push_back(byte);
    }

    return bytes;
}

char hexArray[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

auto bytes2Hex(const uint8_t bytes[], unsigned long size) {
    string s(size * 2, ' ');
    for (unsigned long i = 0; i < size; ++i) {
        s[2 * i] = hexArray[(bytes[i] & 0xF0) >> 4];
        s[2 * i + 1] = hexArray[bytes[i] & 0xF];
    }
    return s;
}

auto toUInt16(const string &str) {
    uint16_t res = 0;
    for (auto c: str) {
        res <<= 8;
        res |= c;
    }
    return res;
}

auto split(string str) {
    vector<uint16_t> res;
    while (str.size() % 2) {
        str.push_back('\0');
    }
    for (size_t i = 0; i < str.length(); i += 2) {
        res.emplace_back(toUInt16(string(str, i, 2)));
    }
    return res;
}

auto randomTest() {
    uint16_t key[Nr + 1];
    SPN::generateRoundKey(0x3a94d63f, key);
    ifstream plaintext;
    ofstream cipher;
    plaintext.open("../examples/plaintext.txt");
    cipher.open("../examples/cipher.txt", ios::binary);
    auto plain = split(string(istreambuf_iterator<char>(plaintext), istreambuf_iterator<char>()));
    for_each(plain.begin(), plain.end(), [&key, &cipher](auto item) {
        uint16_t res = SPN::SPN(item, key);
        char data[2];
        data[1] = static_cast<char>(res & 0xFF);
        data[0] = static_cast<char>(res >> 8 & 0xFF);
        cipher.write(data, 2);
    });
    plaintext.close();
    cipher.close();
}

auto getCTR(uint8_t counter[], int n) {
    for (int i = 15; i >= 0; i--) {
        counter[i] = n & 0xff;
        n >>= 8;
    }
}

auto addCTR(uint8_t counter[], const char cipher[]) {
    for (int i = 0; i < 16; i++) {
        counter[i] ^= cipher[i];
    }
}

auto randomTestEnhanced() {
    auto key = "0000000000000000000000000000000000000000000000000000000000000000";
    uint8_t roundKey[16 * (Mr + 1)];
    enhancedSPN::generateRoundKey(hex2Bytes(key).data(), roundKey);
    ifstream plaintext;
    ofstream cipher;
    plaintext.open("../examples/plaintext.txt");
    cipher.open("../examples/cipher_enhanced.txt", ios::binary);
    auto plain = string(istreambuf_iterator<char>(plaintext), istreambuf_iterator<char>());
    while (plain.size() % 16) {
        plain.push_back('\0');
    }
    auto plainBytes = plain.c_str();
    int counter = 0;
    uint8_t bytes[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    for (int i = 0; i < plain.length(); i += 16) {
        getCTR(bytes, counter++);
        enhancedSPN::SPN(bytes, roundKey);
        addCTR(bytes, plainBytes + i);
        cipher.write((char *) bytes, 16);
    }
    plaintext.close();
    cipher.close();
}