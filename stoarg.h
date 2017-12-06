#ifndef __STOARG_H__
#define __STOARG_H__

//This header includes a method and a helper to convert a single line commad into a array of c-style string (i.e. char**)

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <exception>
#include "myexception.h"

int decomp(char*** argv, std::string command);

int argvwrap(char*** argv, char* cur, int len, int &pos, int &argsize);

int regchk(char* cmd) throw (invalid_cmd);

int pathchk(char** cmd, char* path, int dirlen) throw(invalid_cmd);

int pathchk(char** cmd) throw(invalid_cmd);

int pathapd (char** cmd, char const * pPath) throw(invalid_cmd);

#endif
