#include "commands.h"
#include<filesystem>
#include<fstream>


bool create_init_directory(std::string);

int init(){
	if(!create_init_directory(".fake-git/objects/pack"))return 1;
       	if(!create_init_directory(".fake-git/objects/info"))return 1;
       	if(!create_init_directory(".fake-git/refs/heads"))return 1;
       	if(!create_init_directory(".fake-git/refs/tags"))return 1;
	std::ofstream file(".fake-git/HEAD");
	if(!file){
		return 1;
	}
	if()
	return 0;
}

bool create_init_directory(std::string path){
	std::error_code ec;
	std::filesystem::create_directories(path, ec);
	if(ec) return false;
	return true;
}

bool write_contents(std::ofstream fileRef, std::string content){
	
}
