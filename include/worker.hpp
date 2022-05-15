#ifndef worker_hpp
#define worker_hpp

#include <cstring>
#include <iostream>
#include <filesystem>
#include <unistd.h>
#include <sys/inotify.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

/*Pops a sub string ending with "last char", if not exist returns ""*/
std::string popSubString(std::string &my_string, char lastChar);

/*Returns url and erases url + all chars before. If url not exist returns ""*/
std::string popUrl(std::string& my_string);

void cleanUrl(std::string& url);

void worker(int i, std::string fifoname, std::string path);
#endif