#pragma once 

#include<filesystem>
#include<vector>
#include <cstring>
int init();
std::string hash_object(const std::string& ,const std::filesystem::path& );
int write_object(const std::string& ,const std::vector<uint8_t>& );
int hash_object_write(const std::string& ,const std::filesystem::path& );
std::vector<uint8_t> read_bytes(const std::filesystem::path &);
std::vector<uint8_t> wrap(const std::string &, const std::vector<uint8_t> &);
std::vector<uint8_t> cat_file(std::string& );
std::vector<uint8_t> decompress(const std::vector<uint8_t>& );