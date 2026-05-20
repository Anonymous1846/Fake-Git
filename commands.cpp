#include <string_view>
#include <filesystem>
#include <algorithm>
#include <iostream>
#include<optional>
#include <fstream>
#include <cstring>
#include <vector>
#include <zlib.h>
#include <string>
#include<chrono>
#include "constants.hpp"
#include "commands.hpp"
#include "objects.hpp"
#include "sha.hpp"

struct TreeObject
{
	std::string mode;
	std::string filename;
	std::array<uint8_t, 20> bytes;
};

struct CommitObject{
	std::string tree;
	std::optional<std::string> parent;
	std::string author;
	std::string commiter;
	std::string commitMsg;
};

namespace{
	std::filesystem::path getObjectPathFromHex(const std::string& hex_data){
		std::string_view dirName(hex_data.data(), 2);
		std::string_view fileName(hex_data.data() + 2, hex_data.size() - 2);
		std::filesystem::path fullFilePath = Git::OBJECTS / dirName / fileName;
		return fullFilePath;
	}
	
}

int write_object(const std::string &sha_hex, const std::vector<uint8_t> &wrappedData);
int hash_object_write(const std::string &type, const std::filesystem::path &path);
int cat_file(const std::string &, const std::string &);
bool create_init_directory(const std::filesystem::path &);
bool write_contents(std::ofstream &, const std::string &);
std::string hash_object(const std::string &type, const std::filesystem::path &path);
std::vector<uint8_t> wrap(const std::string &, const std::vector<uint8_t> &);
std::vector<uint8_t> read_bytes(const std::filesystem::path &);
std::vector<uint8_t> decompress(const std::vector<uint8_t> &);
std::vector<TreeObject> readTree(const std::string &);
std::string writeTree(const std::vector<TreeObject> &);
std::string readRef(const std::string& );


int init()
{
	if (!create_init_directory(Git::OBJECTS / "pack"))
		return 1;
	if (!create_init_directory(Git::OBJECTS / "info"))
		return 1;
	if (!create_init_directory(Git::REFS / "heads"))
		return 1;
	if (!create_init_directory(Git::REFS / "tags"))
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

int cat_file(const std::string &flag, const std::string& hex_data)
{
	std::filesystem::path fullFilePath = getObjectPathFromHex(hex_data);
	if (!std::filesystem::exists(fullFilePath))
	{
		std::cerr << "fatal: not a valid object " << hex_data << "\n";
		return 1;
	}
	std::vector<uint8_t> bytes = read_bytes(fullFilePath);
	std::vector<uint8_t> decompressedBytes = decompress(bytes);
	if (decompressedBytes.empty())
	{
		std::cerr << "fatal: not a valid object " << hex_data << "\n";
		return 1;
	}
	auto sep = std::find(decompressedBytes.begin(), decompressedBytes.end(), '\0');
	if (sep == decompressedBytes.end())
		return 1;
	std::string header(decompressedBytes.begin(), sep);
	std::vector<uint8_t> contents(sep + 1, decompressedBytes.end());
	if (flag == "-p")
	{
		std::for_each(contents.begin(), contents.end(), [](uint8_t value)
					  {
						  std::cout << value; // cat-file -p
					  });
		return 0;
	}
	else
	{
		size_t size = header.find(' ');
		std::string type = header.substr(0, size);
		std::string dataSize = header.substr(size + 1);

		if (flag == "-t")
		{
			std::cout << type;
			return 0;
		}
		else if (flag == "-s")
		{
			std::cout << dataSize;
			return 0;
		}
	}
	return 1;
}

std::vector<TreeObject> readTree(const std::string &hex_data)
{
	std::filesystem::path fileFullPath = getObjectPathFromHex(hex_data);
	std::vector<uint8_t> bytes = read_bytes(fileFullPath);
	std::vector<uint8_t> decompressedBytes = decompress(bytes);
	if (decompressedBytes.empty())
	{
		std::cerr << "fatal: not a valid object " << hex_data << "\n";
		return {};
	}

	auto header_end = std::find(decompressedBytes.begin(),decompressedBytes.end(),'\0');
	if(header_end == decompressedBytes.end()) return {};
	size_t pos = (header_end - decompressedBytes.begin()) + 1;
	std::vector<TreeObject> entries;
	while(pos < decompressedBytes.size()){
		size_t space = pos;
		while(space < decompressedBytes.size() && decompressedBytes[space]!=' ') space++;
		if(space >= decompressedBytes.size()) return {};
		std::string mode(decompressedBytes.begin()+pos,decompressedBytes.begin()+space);
		pos = space + 1;
		size_t nullPoint = pos;
		while(nullPoint < decompressedBytes.size() && decompressedBytes[nullPoint]!='\0') nullPoint++;
		if(nullPoint >= decompressedBytes.size()) return {};
		std::string filename(decompressedBytes.begin()+pos,decompressedBytes.begin()+nullPoint);
		pos = nullPoint + 1;
		if(pos + 20 > decompressedBytes.size()) return {};
		std::array<uint8_t,20> sha_bytes;
		std::copy(decompressedBytes.begin()+pos,decompressedBytes.begin()+pos+20,sha_bytes.begin());
		pos +=20;
		TreeObject treeObject;
		treeObject.mode = mode;
		treeObject.filename = filename;
		treeObject.bytes = sha_bytes;
		entries.push_back(treeObject);

	}
	return entries;
}

int lsTree(const std::string& hex_data,bool name_only){
	std::vector<TreeObject> entries = readTree(hex_data);
	if(entries.empty()) return 1;
	if(name_only)std::for_each(entries.begin(), entries.end(), [](TreeObject &treeObject) {
		std::cout<<treeObject.filename<<"\n"; 
    });
	else 
		std::for_each(entries.begin(), entries.end(), [](TreeObject &treeObject) {
		std::string type = ("40000"==treeObject.mode) || ("040000"==treeObject.mode) ? "tree" : "blob";
		std::cout << treeObject.mode << " " << type << " " 
          << to_hex(treeObject.bytes) << "\t" 
          << treeObject.filename << "\n";
    });
	return 0;
}

std::string writeCommit(const std::string& treeSha,const std::optional<std::string>& parent,const std::string& message){
	int64_t timestamp = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch()).count();
	std::string commitString = "tree "+ treeSha+ "\n";
	if(parent.has_value()) commitString += "parent "+parent.value()+"\n";
	commitString +="author Daniel James <danieljames@gmail.com> "+std::to_string(timestamp) + "+0530\n";
	commitString +="committer Daniel James <danieljames@gmail.com> "+std::to_string(timestamp) + "+0530\n\n";
	commitString += message+"\n";
	
	std::vector<uint8_t> commitData(commitString.begin(),commitString.end());
	std::vector<uint8_t>wrappedData = wrap("commit",commitData);
	std::string sha = to_hex(sha1(wrappedData));
	if(write_object(sha,wrappedData)!=0)return "";
	return sha;
}

std::string readRef(const std::string& reFname){
	std::filesystem::path refPath = Git::MAIN_DIR / reFname;
	std::ifstream refStream(refPath);
	if(!refStream) return "";
	std::string content((std::istreambuf_iterator<char>(refStream)), std::istreambuf_iterator<char>());
	if(content.substr(0,5)=="ref: "){
		std::string refPath = content.substr(5);
		return readRef(reFname);
	}
	if (!content.empty() && content.back() == '\n') content.pop_back();

	return content;
}

std::string writeTree(const std::vector<TreeObject> &entries)
{
	std::vector<uint8_t> entry_bytes;
	std::vector<TreeObject> sorted = entries;
	std::sort(sorted.begin(),sorted.end(),[](
		const TreeObject& treeObject1,const TreeObject& treeObject2
	){
		bool isDir1 = (treeObject1.mode == "40000" || treeObject1.mode == "040000");
		bool isDir2 = (treeObject2.mode == "40000" || treeObject2.mode == "040000");
		std::string name1 = treeObject1.filename + (isDir1 ? "/" : "");
		std::string name2 = treeObject2.filename + (isDir2 ? "/" : "");
		return name1 < name2;
	});
	for (const TreeObject &treeObject : sorted)
	{
		for (char c : treeObject.mode)
			entry_bytes.push_back(c);

		entry_bytes.push_back(' ');

		for (char c : treeObject.filename)
			entry_bytes.push_back(c);

		entry_bytes.push_back('\0');

		for (uint8_t b : treeObject.bytes)
			entry_bytes.push_back(b);
	}
	std::vector<uint8_t> wrappedBytes = wrap("tree",entry_bytes);
	std::string sha = to_hex(sha1(wrappedBytes));
	if(write_object(sha,wrappedBytes)!=0) return "";
	
	return sha;
}

int write_object(const std::string &sha_hex, const std::vector<uint8_t> &wrappedData)
{

	uLongf compressedSize = compressBound(wrappedData.size());
	std::vector<uint8_t> compress(compressedSize);

	int RESULT_STAT = compress2(
		compress.data(), &compressedSize,
		wrappedData.data(), wrappedData.size(),
		Z_DEFAULT_COMPRESSION);

	if (RESULT_STAT != Z_OK)
		return 1;
	std::string_view subDirName(sha_hex.data(), 2);
	std::string_view fileData(sha_hex.data() + 2, sha_hex.size() - 2);
	std::filesystem::path objectPath = Git::OBJECTS / subDirName / fileData;

	std::filesystem::create_directories(objectPath.parent_path());

	std::ofstream outStream(objectPath, std::ios::binary);
	if (!outStream)
		return 1;
	outStream.write(reinterpret_cast<char *>(compress.data()), compressedSize);
	if (!outStream)
		return 1;
	return 0;
}

std::string hash_object(const std::string &type, const std::filesystem::path &path)
{
	std::vector<uint8_t> content = read_bytes(path);
	std::vector<uint8_t> wrappedContent = wrap(type, content);
	return to_hex(sha1(wrappedContent));
}

int hash_object_write(const std::string &type, const std::filesystem::path &filePath)
{
	std::vector<uint8_t> content = read_bytes(filePath);
	std::vector<uint8_t> wrappedData = wrap(type, content);
	std::string hexData = to_hex(sha1(wrappedData));
	if (write_object(hexData, wrappedData) != 0)
		return 1;
	std::cout << hexData;
	return 0;
}