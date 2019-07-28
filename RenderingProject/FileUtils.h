#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>
#include <vector>
#include <Windows.h>
#include <filesystem>

//some utility functions for working with files

std::string getWorkingDir();
size_t getFileSize(std::string);

#endif // !FILE_UTILS_H