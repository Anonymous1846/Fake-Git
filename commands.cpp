#include "commands.h"
#include "constants.h"
#include<filesystem>
#include<fstream>


bool create_init_directory(const std::filesystem::path&);
bool write_contents(std::ofstream& ,const std::string& );


int init(){
	if(!create_init_directory(Git::MAIN_DIR / "objects" / "pack"))return 1;
       	if(!create_init_directory(Git::MAIN_DIR/ "objects" / "info"))return 1;
       	if(!create_init_directory(Git::MAIN_DIR / "refs" / "heads"))return 1;
       	if(!create_init_directory(Git::MAIN_DIR / "refs"/"tags"))return 1;
	std::ofstream file(Git::MAIN_DIR / "HEAD");
	if(!file){
		return 1;
	}
	return 0;
}

bool create_init_directory(const std::filesystem::path& filePath){
	std::error_code ec;
	std::filesystem::create_directories(filePath, ec);
	if(ec) return false;
	return true;
}

bool write_contents(std::ofstream& fileRef,const std::string& content){
	fileRef << content;
	if(!fileRef) return false;
	return true;
}
