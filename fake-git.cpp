#include<iostream>

int main(int argc,char **argv){
	if(argc < 2){
		std::cout<<"Usage: fake-git [options] <file-name>"<<std::endl;
		return 1;
	}
	std::string operation = argv[1];
	if(operation == "init"){
		init();
	}


}
