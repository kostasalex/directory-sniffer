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
#include <sys/wait.h>
#include <signal.h>

/*Pops a sub string ending with "last char", if not exist returns ""*/
std::string popSubString(std::string &my_string, char lastChar);

void worker(int i, std::string fifoname, std::string, std::string);
#endif