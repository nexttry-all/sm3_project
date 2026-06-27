#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <chrono>
using namespace std;

#include "sm3.cpp"

void test_sm3() {
    cout << "===== SM3 单元测试 =====" << endl;
    
    // 1. 空串测试
    string msg1 = "";
    auto padded1 = pad_message(msg1);
    vector<uint32_t> V1(IV, IV + 8);
    for (size_t i = 0; i < padded1.size(); i += 64) {
        vector<unsigned char> block(padded1.begin() + i, padded1.begin() + i + 64);
        sm3_compress(V1, block);
    }
    uint32_t expected1[8] = {
        0x1AB21D83, 0x55CFA17F, 0x8E611948, 0x31E81A8F,
        0x22BEC8C7, 0x28FEFB74, 0x7ED035EB, 0x5082AA2B
    };
    bool pass1 = true;
    for (int i = 0; i < 8; i++) {
        if (V1[i] != expected1[i]) pass1 = false;
    }
    
    // 2. "abc" 测试
    string msg2 = "abc";
    auto padded2 = pad_message(msg2);
    vector<uint32_t> V2(IV, IV + 8);
    for (size_t i = 0; i < padded2.size(); i += 64) {
        vector<unsigned char> block(padded2.begin() + i, padded2.begin() + i + 64);
        sm3_compress(V2, block);
    }
    uint32_t expected2[8] = {
        0x66C7F0F4, 0x62EEEDD9, 0xD1F2D46B, 0xDC10E4E2,
        0x4167C487, 0x5CF2F7A2, 0x297DA02B, 0x8F4BA8E0
    };
    bool pass2 = true;
    for (int i = 0; i < 8; i++) {
        if (V2[i] != expected2[i]) pass2 = false;
    }
    
    // 3. 512比特消息测试 ("abcd" 重复16次)
    string msg3 = "abcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcd";
    auto padded3 = pad_message(msg3);
    vector<uint32_t> V3(IV, IV + 8);
    for (size_t i = 0; i < padded3.size(); i += 64) {
        vector<unsigned char> block(padded3.begin() + i, padded3.begin() + i + 64);
        sm3_compress(V3, block);
    }
    uint32_t expected3[8] = {
        0xdebe9ff9, 0x2275b8a1, 0x38604889, 0xc18e5a4d,
        0x6fdb70e5, 0x387e5765, 0x293dcba3, 0x9c0c5732
    };
    bool pass3 = true;
    for (int i = 0; i < 8; i++) {
        if (V3[i] != expected3[i]) pass3 = false;
    }
    
    cout << "空串:      " << (pass1 ? "✅ PASS" : "❌ FAIL") << endl;
    cout << "\"abc\":     " << (pass2 ? "✅ PASS" : "❌ FAIL") << endl;
    cout << "\"abcd\"×16: " << (pass3 ? "✅ PASS" : "❌ FAIL") << endl;
    cout << endl;
}

void benchmark_sm3() {
    const int TEST_SIZE = 1024 * 1024;
    string data(TEST_SIZE, 'a');
    
    // 预热
    for (int i = 0; i < 10; i++) {
        auto padded = pad_message(data);
        vector<uint32_t> V(IV, IV + 8);
        for (size_t j = 0; j < padded.size(); j += 64) {
            vector<unsigned char> block(padded.begin() + j, padded.begin() + j + 64);
            sm3_compress(V, block);
        }
    }
    
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < 100; i++) {
        auto padded = pad_message(data);
        vector<uint32_t> V(IV, IV + 8);
        for (size_t j = 0; j < padded.size(); j += 64) {
            vector<unsigned char> block(padded.begin() + j, padded.begin() + j + 64);
            sm3_compress(V, block);
        }
    }
    auto end = chrono::high_resolution_clock::now();
    
    double total_time = chrono::duration<double>(end - start).count();
    double total_data = (double)TEST_SIZE * 100 / (1024 * 1024);
    double throughput = total_data / total_time;
    
    cout << "SM3性能测试 (1MB数据, 100次):" << endl;
    cout << "总耗时: " << total_time << " 秒" << endl;
    cout << "吞吐量: " << throughput << " MB/s" << endl;
}

int main() {
    test_sm3();
    
    string msg = "abc";
    vector<unsigned char> padded = pad_message(msg);
    vector<uint32_t> V(IV, IV + 8);
    for (size_t i = 0; i < padded.size(); i += 64) {
        vector<unsigned char> block(padded.begin() + i, padded.begin() + i + 64);
        sm3_compress(V, block);
    }
    
    cout << "SM3(\"" << msg << "\") = ";
    for (uint32_t word : V) {
        printf("%08x", word);
    }
    cout << endl << endl;
    
    benchmark_sm3();
    return 0;
}