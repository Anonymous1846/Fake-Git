#pragma once 

#include<filesystem>
#include<vector>
#include <cstring>
int init();
std::string hash_object(const std::string& ,const std::filesystem::path& );
int write_object(const std::string& ,const std::vector<uint8_t>& );