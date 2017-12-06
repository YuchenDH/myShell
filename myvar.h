#ifndef __MYVAR_H__
#define __MYVAR_H__

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "mydict.h"

int varsubstitute (std::string& command, mydict dict);

int* ioredir(int& argc, char *** argv);

int ioredir_helper(char* filename, int flag);

#endif
