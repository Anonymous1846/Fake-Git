#include "commands.h"
#include "constants.h"
#include<filesystem>
#include<fstream>
#include<iostream>



bool create_init_directory(const std::filesystem::path&);
bool write_contents(std::ofstream& ,const std::string& );

int init(){
	if(!create_init_directory(Git::MAIN_DIR / "objects" / "pack"))return 1;
       	if(!create_init_directory(Git::MAIN_DIR/ "objects" / "info"))return 1;
       	if(!create_init_directory(Git::MAIN_DIR / "refs" / "heads"))return 1;
       	if(!create_init_directory(Git::MAIN_DIR / "refs"/"tags"))return 1;
		std::ofstream headFile(Git::MAIN_DIR / "HEAD");
		if(!headFile) return 1;
		if(!write_contents(headFile,"ref: refs/heads/master\n"))return 1;
		std::ofstream configFile(Git::MAIN_DIR / "config");
		if(!configFile) return 1;
		std::string configData = "[core]\n\trepositoryformatversion = 0\n\tfilemode - true\n\tbare = false\n";
		if(!write_contents(configFile, configData)) return 1;
		return 0;
}

bool create_init_directory(const std::filesystem::path& filePath){
	std::error_code ec;
	std::filesystem::create_directories(filePath, ec);
	if(ec) return false;
	return true;
}

bool write_contents(std::ofstream& fileRef,const std::string& content){
	if(!fileRef) return false;
	fileRef << content;
	if(!fileRef) return false;
	return true;
}
