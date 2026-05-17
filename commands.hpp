#pragma once 

#include<filesystem>
#include <cstring>
#include<vector>


int init();
std::string hash_object(const std::string& ,const std::filesystem::path& );
int write_object(const std::string& ,const std::vector<uint8_t>& );
int hash_object_write(const std::string& ,const std::filesystem::path& );
int cat_file(const std::string&,const std::string& );
int lsTree(const std::string& ,bool);
