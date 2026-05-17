#pragma once

#include<array>
#include<string>
#include<vector>


std::array<uint8_t, 20> sha1(const std::vector<uint8_t>& data);
std::string to_hex(const std::array<uint8_t, 20>& hash);

