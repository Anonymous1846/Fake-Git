#pragma once

#include<array>
#include<string>
#include<vector>


std::array<uint8_t, 20> sha1(const std::vector<uint8_t>& data);
std::string to_hex(const std::array<uint8_t, 20>& hash);
std::uint32_t rotl(uint32_t,int );
std::uint32_t f1(std::uint32_t b,std::uint32_t c,std::uint32_t d);
std::uint32_t f2(std::uint32_t b,std::uint32_t c,std::uint32_t d);
std::uint32_t f3(std::uint32_t b,std::uint32_t c,std::uint32_t d);
std::uint32_t f4(std::uint32_t b,std::uint32_t c,std::uint32_t d);
void pad(std::vector<uint8_t>& data);
