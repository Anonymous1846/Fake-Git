#pragma once

#include <cstdint>
#include<vector>
#include <string>
#include <filesystem>


std::vector<uint8_t> wrap(const std::string &, const std::vector<uint8_t> &);
std::vector<uint8_t> decompress(const std::vector<uint8_t>& );
std::vector<uint8_t> read_bytes(const std::filesystem::path &);
