auto toUInt16(const string &str) {
    uint16_t res = 0;
    for (auto c: str) {
        res <<= 8;
        res |= c;
    }
    return res;
}

auto toUInt32(const string &str) {
    uint32_t res = 0;
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

auto splitEnhanced(string str) {
    vector<uint32_t> res;
    while (str.size() % 4) {
        str.push_back('\0');
    }
    for (size_t i = 0; i < str.length(); i += 4) {
        res.emplace_back(toUInt32(string(str, i, 4)));
    }
    return res;
}

int randomTest() {
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
    return 0;
}

int randomTestEnhanced() {
    uint32_t enhancedKey[Mr + 1];
    enhancedSPN::generateRoundKey(0x0000000000000000, enhancedKey);
    ifstream plaintext;
    ofstream cipher;
    plaintext.open("../examples/plaintext.txt");
    cipher.open("../examples/cipher_enhanced.txt", ios::binary);
    auto plain = string("\0", 10000000);
    for_each(plain.begin(), plain.end(), [&enhancedKey, &cipher](auto item) {
        uint32_t res = enhancedSPN::SPN(item, enhancedKey);
        char data[4];
        data[3] = static_cast<char>(res & 0xFF);
        data[2] = static_cast<char>((res >> 8) & 0xFF);
        data[1] = static_cast<char>((res >> 16) & 0xFF);
        data[0] = static_cast<char>((res >> 24) & 0xFF);
        cipher.write(data, 4);
    });
    plaintext.close();
    cipher.close();
    return 0;
}