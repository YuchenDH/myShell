#ifndef __MYEXECUTE_H__
#define __MYEXECUTE_H__

#include <iostream>
#include <string.h>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <errno.h>
#include <cassert>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <exception>
#include <fcntl.h>

#include "mydict.h"
#include "myvar.h"
#include "stoarg.h"
#include "myexception.h"

int single_execute(std::string command, mydict& env, int fd_in, int fd_out);

int forkexe(char** argv, char **envp, int* fd_arr);

void execset(std::string command, std::string key, mydict& env);

int execute(std::string command, mydict& env, int fd_in);

#endif
