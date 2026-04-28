#include "commands.h"
#include<filesystem>
#include<fstream>


bool create_init_directory(std::string);

int init(){
	if(!create_init_directory(Git::MAIN_DIR / "objects" / "pack"))return 1;
       	if(!create_init_directory(Git::MAIN_DIR/ "objects" / "info"))return 1;
       	if(!create_init_directory(Git::MAIN_DIR / "refs" / "heads"))return 1;
       	if(!create_init_directory(Git::MAIN_DIR / "refs"/"tags"))return 1;
	std::ofstream file(Git::MAIN_DIR / "HEAD");
	if(!file){
		return 1;
	}
	if()
	return 0;
}

bool create_init_directory(std::filesystem &path){
	std::error_code ec;
	std::filesystem::create_directories(path, ec);
	if(ec) return false;
	return true;
}

bool write_contents(std::ofstream& fileRef, std::string& content){
	fileRef << content;
}
