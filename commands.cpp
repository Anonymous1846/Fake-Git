#include "commands.hpp"
#include "constants.hpp"
#include "sha.hpp"
#include <filesystem>
#include<vector>
#include <fstream>
#include <iostream>
#include <cstring>

bool create_init_directory(const std::filesystem::path &);
bool write_contents(std::ofstream &, const std::string &);
std::string hash_object(const std::filesystem::path& path);


int init()
{
	if (!create_init_directory(Git::MAIN_DIR / "objects" / "pack"))
		return 1;
	if (!create_init_directory(Git::MAIN_DIR / "objects" / "info"))
		return 1;
	if (!create_init_directory(Git::MAIN_DIR / "refs" / "heads"))
		return 1;
	if (!create_init_directory(Git::MAIN_DIR / "refs" / "tags"))
		return 1;
	std::ofstream headFile(Git::MAIN_DIR / "HEAD");
	if (!headFile)
		return 1;
	if (!write_contents(headFile, "ref: refs/heads/master\n"))
		return 1;
	std::ofstream configFile(Git::MAIN_DIR / "config");
	if (!configFile)
		return 1;
	std::string configData = "[core]\n\trepositoryformatversion = 0\n\tfilemode = true\n\tbare = false\n";
	if (!write_contents(configFile, configData))
		return 1;
	return 0;
}

bool create_init_directory(const std::filesystem::path &filePath)
{
	std::error_code ec;
	std::filesystem::create_directories(filePath, ec);
	if (ec)
		return false;
	return true;
}

bool write_contents(std::ofstream &fileRef, const std::string &content)
{
	if (!fileRef)
		return false;
	fileRef << content;
	if (!fileRef)
		return false;
	return true;
}

std::string hash_object(std::filesystem::path& path){
	std::ifstream fileInputStream(path,std::ios::binary | std::ios::ate);
	std::streamsize size = fileInputStream.tellg();
	fileInputStream.seekg(0,std::ios::beg);
	std::vector<uint8_t> buffer(size);

	if(size > 0){
		// if(!fileInputStream.read(buffer.data(),size)){
		// 	return {};
		// }
		fileInputStream.read(reinterpret_cast<char*>(buffer.data()),size);
	}
	std::string header = "blob " + std::to_string(size);
	size_t totalSize = header.size() + 1 + size;
	std::vector<uint8_t> finalBuffer(totalSize);
	std::memcpy(finalBuffer.data(),header.data(),header.size());
	finalBuffer[header.size()] = '\0';
	std::memcpy(finalBuffer.data()+header.size()+1,buffer.data(),size);
	return to_hex(sha1(finalBuffer));
}