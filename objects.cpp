#include <filesystem>
#include <iostream>
#include <cstring>
#include <fstream>
#include <cstdint>
#include <vector>
#include <zlib.h>


std::vector<uint8_t> wrap(const std::string &type, const std::vector<uint8_t> &content){
	std:: size_t size = content.size();
	std::string header = type + " " + std::to_string(size);
	size_t totalSize = header.size() + 1 + size;
	std::vector<uint8_t> finalBuffer(totalSize);
	std::memcpy(finalBuffer.data(), header.data(), header.size());
	finalBuffer[header.size()] = '\0';
	std::memcpy(finalBuffer.data() + header.size() + 1, content.data(), size);
	return finalBuffer;
}

std::vector<uint8_t> decompress(const std::vector<uint8_t>& compressed){
	z_stream ztr{};
	ztr.zalloc = Z_NULL;
	ztr.zfree = Z_NULL;
	ztr.opaque = Z_NULL;
	ztr.next_in = (Bytef *) compressed.data();
	ztr.avail_in = compressed.size();
	inflateInit(&ztr);
	
	std::vector<uint8_t> result;
	uint8_t chunk[8192];
	int ret;
	do{
		ztr.next_out = chunk;
		ztr.avail_out = sizeof(chunk);
		ret = inflate(&ztr,Z_NO_FLUSH);
		size_t filled = sizeof(chunk) - ztr.avail_out;
		result.insert(result.end(),chunk, chunk + filled);
	}while(ret == Z_OK);
	inflateEnd(&ztr);

	if(ret != Z_STREAM_END) return {};
	return result;
}

std::vector<uint8_t> read_bytes(const std::filesystem::path &filePath){
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
