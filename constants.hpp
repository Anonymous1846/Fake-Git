#pragma once
#include<filesystem>

namespace Git{
	inline const std::filesystem::path MAIN_DIR = "/tmp/.git";
	inline const std::filesystem::path OBJECTS = MAIN_DIR / "objects";
	inline const std::filesystem::path REFS = MAIN_DIR / "refs";
	
}
