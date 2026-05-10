#include<iostream>
#include<zlib.h>
#include "commands.hpp"
#include"sha.hpp"
#include <cstring>

int main(int argc,char **argv){
	if(argc < 2){
		std::cout<<"Usage: fake-git [options] <file-name>"<<std::endl;
		return 1;
	}
	std::string operation = argv[1];
	if(operation == "init"){
		init();
	}
	else if(operation == "hash-object"){
		
		if(argc == 3){
			std::cout<< hash_object("blob",argv[2]) << "\n";
		}
		else if(argc == 4 && std::string(argv[2])=="-w"){
			return hash_object_write("blob",argv[3]);
		}
		else{
			std::cout<<"Usage: fake-git hash-object [OPTION] <file-name>"<<std::endl;
			return 1;
		}
		
		
	}
	
}
