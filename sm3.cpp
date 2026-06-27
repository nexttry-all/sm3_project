#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
using namespace std;

const uint32_t IV[8] = {
    0x7380166F, 0x4914B2B9, 0x172442D7, 0xDA8A0600,
    0xA96F30BC, 0x163138AA, 0xE38DEE4D, 0xB0FB0E4E
};

uint32_t rotl(uint32_t x, int n) {
    n = n % 32;
    if (n == 0) return x;
    return (x << n) | (x >> (32 - n));
}

uint32_t T(int j) {
    return (j <= 15) ? 0x79CC4519 : 0x7A879D8A;
}

uint32_t FF(int j, uint32_t X, uint32_t Y, uint32_t Z) {
    if (j <= 15) return X ^ Y ^ Z;
    return (X & Y) | (X & Z) | (Y & Z);
}

uint32_t GG(int j, uint32_t X, uint32_t Y, uint32_t Z) {
    if (j <= 15) return X ^ Y ^ Z;
    return (X & Y) | (~X & Z);
}

vector<unsigned char> pad_message(const string& msg) {
    vector<unsigned char> data(msg.begin(), msg.end());
    data.push_back(0x80);
    while ((data.size() % 64) != 56) {
        data.push_back(0x00);
    }
    uint64_t bit_len = static_cast<uint64_t>(msg.size()) * 8;
    for (int i = 7; i >= 0; i--) {
        data.push_back((bit_len >> (i * 8)) & 0xFF);
    }
    return data;
}

void sm3_compress(vector<uint32_t>& V, const vector<unsigned char>& block) {
    uint32_t W[68];
    uint32_t W1[64];

    for (int i = 0; i < 16; i++) {
        W[i] = ((uint32_t)block[i*4] << 24) |
               ((uint32_t)block[i*4+1] << 16) |
               ((uint32_t)block[i*4+2] << 8) |
               (uint32_t)block[i*4+3];
    }

    for (int i = 16; i < 68; i++) {
        W[i] = W[i-16] ^ W[i-9] ^ rotl(W[i-3], 15);
        W[i] = W[i] ^ rotl(W[i], 15) ^ rotl(W[i], 23);
        W[i] = W[i] ^ rotl(W[i-13], 7) ^ W[i-6];
    }

    for (int i = 0; i < 64; i++) {
        W1[i] = W[i] ^ W[i+4];
    }

    uint32_t A = V[0], B = V[1], C = V[2], D = V[3];
    uint32_t E = V[4], F = V[5], G = V[6], H = V[7];

    for (int j = 0; j < 64; j++) {
        uint32_t SS1 = rotl((rotl(A, 12) + E + rotl(T(j), j)) & 0xFFFFFFFF, 7);
        uint32_t SS2 = SS1 ^ rotl(A, 12);
        uint32_t TT1 = (FF(j, A, B, C) + D + SS2 + W1[j]) & 0xFFFFFFFF;
        uint32_t TT2 = (GG(j, E, F, G) + H + SS1 + W[j]) & 0xFFFFFFFF;

        D = C;
        C = rotl(B, 9);
        B = A;
        A = TT1;

        H = G;
        G = rotl(F, 19);
        F = E;
        E = TT2 ^ rotl(TT2, 9) ^ rotl(TT2, 17);
    }

    V[0] = A ^ V[0];
    V[1] = B ^ V[1];
    V[2] = C ^ V[2];
    V[3] = D ^ V[3];
    V[4] = E ^ V[4];
    V[5] = F ^ V[5];
    V[6] = G ^ V[6];
    V[7] = H ^ V[7];
}