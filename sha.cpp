#include <cstdio>
#include "sha.hpp"

namespace
{
    std::uint32_t rotl(uint32_t x, int n)
    {
        return (x << n | (x >> (32 - n)));
    }

    std::uint32_t f1(std::uint32_t b, std::uint32_t c, std::uint32_t d)
    {
        return (b & c) | (~b & d);
    }
    std::uint32_t f2(std::uint32_t b, std::uint32_t c, std::uint32_t d)
    {
        return b ^ c ^ d;
    }
    std::uint32_t f3(std::uint32_t b, std::uint32_t c, std::uint32_t d)
    {
        return (b & c) | (c & d) | (b & d);
    }
    std::uint32_t f4(std::uint32_t b, std::uint32_t c, std::uint32_t d)
    {
        return b ^ c ^ d;
    }
    void pad(std::vector<uint8_t>& data)
    {
        uint64_t originalSize = data.size() * 8;

        size_t padded = data.size() + 1;
        padded += (56 - padded % 64 + 64) % 64;
        padded += 8;
        data.reserve(padded);
        data.push_back(0x80);
        while ((data.size() % 64) != 56)
        {
            data.push_back(0x00);
        }
        data.push_back((originalSize >> 56) & 0xFF);
        data.push_back((originalSize >> 48) & 0xFF);
        data.push_back((originalSize >> 40) & 0xFF);
        data.push_back((originalSize >> 32) & 0xFF);
        data.push_back((originalSize >> 24) & 0xFF);
        data.push_back((originalSize >> 16) & 0xFF);
        data.push_back((originalSize >> 8) & 0xFF);
        data.push_back((originalSize >> 0) & 0xFF);
    }
}
std::string to_hex(const std::array<uint8_t, 20> &hash)
{
    char buf[41];
    for (int i = 0; i < 20; i++)
    {
        snprintf(buf + i * 2, 3,"%02x", hash[i]);
    }
    return std::string(buf, 40);
}

std::array<uint8_t, 20> sha1(const std::vector<uint8_t>& data)
{
    std::vector<uint8_t> copy = data;
    pad(copy);

    uint32_t h0 = 0x67452301;
    uint32_t h1 = 0xEFCDAB89;
    uint32_t h2 = 0x98BADCFE;
    uint32_t h3 = 0x10325476;
    uint32_t h4 = 0xC3D2E1F0;
    for (size_t initialStart = 0; initialStart < copy.size(); initialStart += 64)
    {
        uint32_t finalWord[80];
        for (int i = 0; i < 16; i++)
        {
            finalWord[i] = (copy[initialStart + i * 4] << 24) |
                           (copy[initialStart + i * 4 + 1] << 16) |
                           (copy[initialStart + i * 4 + 2] << 8) |
                           copy[initialStart + i * 4 + 3];
        }

        uint32_t a = h0;
        uint32_t b = h1;
        uint32_t c = h2;
        uint32_t d = h3;
        uint32_t e = h4;
        // LOOP 1 — expand message schedule (16 to 79)
        for (int i = 16; i < 80; i++)
        {
            finalWord[i] = rotl(finalWord[i - 3] ^ finalWord[i - 8] ^ finalWord[i - 14] ^ finalWord[i - 16], 1);
        }

        // LOOP 2 — run all 80 rounds (0 to 79)
        for (int i = 0; i < 80; i++)
        {
            uint32_t f, k;
            if (i < 20)
            {
                f = f1(b, c, d);
                k = 0x5A827999;
            }
            else if (i < 40)
            {
                f = f2(b, c, d);
                k = 0x6ED9EBA1;
            }
            else if (i < 60)
            {
                f = f3(b, c, d);
                k = 0x8F1BBCDC;
            }
            else
            {
                f = f4(b, c, d);
                k = 0xCA62C1D6;
            }

            uint32_t temp = rotl(a, 5) + f + e + k + finalWord[i];
            e = d;
            d = c;
            c = rotl(b, 30);
            b = a;
            a = temp;
        }
        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
        h4 += e;
    }
    std::array<uint8_t, 20> result;
    result[0] = (h0 >> 24) & 0xFF;
    result[1] = (h0 >> 16) & 0xFF;
    result[2] = (h0 >> 8) & 0xFF;
    result[3] = (h0 >> 0) & 0xFF;

    result[4] = (h1 >> 24) & 0xFF;
    result[5] = (h1 >> 16) & 0xFF;
    result[6] = (h1 >> 8) & 0xFF;
    result[7] = (h1 >> 0) & 0xFF;

    result[8] = (h2 >> 24) & 0xFF;
    result[9] = (h2 >> 16) & 0xFF;
    result[10] = (h2 >> 8) & 0xFF;
    result[11] = (h2 >> 0) & 0xFF;

    result[12] = (h3 >> 24) & 0xFF;
    result[13] = (h3 >> 16) & 0xFF;
    result[14] = (h3 >> 8) & 0xFF;
    result[15] = (h3 >> 0) & 0xFF;

    result[16] = (h4 >> 24) & 0xFF;
    result[17] = (h4 >> 16) & 0xFF;
    result[18] = (h4 >> 8) & 0xFF;
    result[19] = (h4 >> 0) & 0xFF;
    return result;
}