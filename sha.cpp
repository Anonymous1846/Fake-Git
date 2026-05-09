#include "sha.hpp"

std::array<uint8_t,20> sha1(const std::vector<uint8_t>& data){
    return {};
}

std::string to_hex(const std::array<uint8_t, 20>& hash){
    return "";
}


std::uint32_t rotl(uint32_t x,int n){
    return (x << n | (x >> (32-n)));
}

std::uint32_t f1(std::uint32_t b,std::uint32_t c,std::uint32_t d){
    return ( b & c) | ( ~b & d );
}
std::uint32_t f2(std::uint32_t b,std::uint32_t c,std::uint32_t d){
    return b ^ c ^ d;
}
std::uint32_t f3(std::uint32_t b,std::uint32_t c,std::uint32_t d){
    return (b & c) | (c & d) | (b & d);
}
std::uint32_t f4(std::uint32_t b,std::uint32_t c,std::uint32_t d){
   return b ^ c ^ d;
}
void pad(std::vector<uint8_t>& data){
    uint64_t orginalSize = data.size() * 8;
    data.push_back(0x80);
    while((data.size() % 64) != 56){
        data.push_back(0x00);
    }
    data.push_back((orginalSize >> 56) & 0xFF);
    data.push_back((orginalSize >> 48) & 0xFF);
    data.push_back((orginalSize >> 40) & 0xFF);
    data.push_back((orginalSize >> 32) & 0xFF);
    data.push_back((orginalSize >> 24) & 0xFF);
    data.push_back((orginalSize >> 16) & 0xFF);
    data.push_back((orginalSize >> 8) & 0xFF);
    data.push_back((orginalSize >> 0) & 0xFF);
}
