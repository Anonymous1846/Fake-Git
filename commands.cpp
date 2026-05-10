#include "commands.hpp"
#include "constants.hpp"
#include "sha.hpp"
#include <filesystem>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstring>
#include <zlib.h>
#include <string>
#include <string_view>

bool create_init_directory(const std::filesystem::path &);
bool write_contents(std::ofstream &, const std::string &);
std::string hash_object(const std::string &type, const std::filesystem::path &path);
int write_object(const std::string &sha_hex,const std::vector<uint8_t> &wrappedData);
int hash_object_write(const std::string& type, const std::filesystem::path& path);
std::vector<uint8_t> read_bytes(const std::filesystem::path &);
std::vector<uint8_t> wrap(const std::string &, const std::vector<uint8_t> &);
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

int hash_object_write(const std::string& type,const std::filesystem::path& filePath){
	std::vector<uint8_t> content = read_bytes(filePath);
	std::vector<uint8_t> wrappedData = wrap(type,content);
	std::string hexData = to_hex(sha1(wrappedData));
	if(write_object(hexData,wrappedData)!=0) return 1;
	std::cout<< hexData;
	return 0;
}

std::string cat_file(std::string& hex_data){
	std::string_view dirName(hex_data.data(),2);
	std::string_view fileName(hex_data.data()+2,hex_data.size()-2);
	std::filesystem::path fullFilePath = Git::MAIN_DIR / "objects" / dirName / fileName;
	if(fullFilePath.empty()) return NULL;
	
}

int write_object(const std::string& sha_hex,const std::vector<uint8_t> &wrappedData)
{

	uLongf compressedSize = compressBound(wrappedData.size());
	std::vector<uint8_t> compress(compressedSize);

	int RESULT_STAT = compress2(
		compress.data(), &compressedSize,
		wrappedData.data(), wrappedData.size(),
		Z_DEFAULT_COMPRESSION);

	if (RESULT_STAT != Z_OK) return 1;
	std::string_view subDirName(sha_hex.data(),2);
	std::string_view fileData(sha_hex.data()+2,sha_hex.size()-2);
	std::filesystem::path objectPath = Git::MAIN_DIR / "objects" / subDirName / fileData;

	std::filesystem::create_directories(objectPath.parent_path());

	std::ofstream outStream(objectPath,std::ios::binary);
	if(!outStream) return 1;
	outStream.write(reinterpret_cast<char *>(compress.data()),compressedSize);
	if(!outStream) return 1;
	return 0;
}

std::vector<uint8_t> wrap(const std::string &type, const std::vector<uint8_t> &content)
{
	std:: size_t size = content.size();
	std::string header = type + " " + std::to_string(size);
	size_t totalSize = header.size() + 1 + size;
	std::vector<uint8_t> finalBuffer(totalSize);
	std::memcpy(finalBuffer.data(), header.data(), header.size());
	finalBuffer[header.size()] = '\0';
	std::memcpy(finalBuffer.data() + header.size() + 1, content.data(), size);
	return finalBuffer;
}

std::vector<uint8_t> read_bytes(const std::filesystem::path &filePath)
{
	std::ifstream fileInputStream(filePath, std::ios::binary | std::ios::ate);
	if(!fileInputStream){
		std::cerr<<"read_bytes: could open the file: "<<filePath<<"\n";
		return {};
	}
	std::streamsize size = fileInputStream.tellg();
	fileInputStream.seekg(0, std::ios::beg);
	std::vector<uint8_t> buffer(size);

	if (size > 0){
		if (!fileInputStream.read(reinterpret_cast<char *>(buffer.data()), size)) return {};
	}
	return buffer;
}

std::string hash_object(const std::string &type, const std::filesystem::path &path)
{
	std::vector<uint8_t> content = read_bytes(path);
	std::size_t size = content.size();
	std::vector<uint8_t> wrappedContent = wrap(type, content);
	return to_hex(sha1(wrappedContent));
}