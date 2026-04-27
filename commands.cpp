#include "commands.h"
#include<filesystem>

bool create_directory(std::string);

int init(){
	if(!create_directory(".git/objects/pack") && !create_directory(".git/objects/info") && !create_directory(".git/refs/heads") && !create_directory(".git/refs/tags")) return 1;
	return 0;	
}

bool create_directory(std::string path){
	std::error_code ec;
	std::filesystem::create_directories(path, ec);
	if(ec) return false;
	return true;
}
