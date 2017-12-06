#include "stoarg.h"

#define DEF_ARGC 8

int decomp(char*** argv, std::string command) {
  int argsize = DEF_ARGC;
  
  *argv = new char*[argsize];
  for (int i = 0; i < argsize; ++i) (*argv)[i] = NULL;

  char * const buf = new char[command.size()+1];
  char * cur = buf;

  strcpy(buf, command.c_str());

  int i = 0;//index for argv[i]
  char * ptr;//pointer to the space
  ptr = strchr(cur, ' ');//find the first space
  while (ptr) {
    if (ptr == cur)
      //if the argument is beginning with space, delete the backslash and continue searching in the remained string
      ptr = strchr(++cur, ' ');
    
    else if (*(ptr-1) == '\\') {
      //if the space is escaped, delete the backslash, then ignore and continue searching
      *(ptr-1) = 0;
      char * temp = new char[strlen(command.c_str())];
      strcpy(temp, buf);
      strcat(temp, ptr);
      strcpy(buf, temp);
      delete [] temp;
      ptr = strchr(ptr+1, ' ');
    } else {
      
      //wrap the argument into argv[i]
      int len = ptr - cur;
      argvwrap(argv, cur, len, i, argsize);
      cur = ptr + 1;
      ptr = strchr(cur, ' ');
    }
  }
  //At this time cur is pointing to the beginning of the last arg
  if (*cur != ' ' && *cur != 0)
    argvwrap(argv, cur, strlen(cur), i, argsize);

  delete [] buf;

  return i;
}

int argvwrap(char*** argv, char* cur, int len, int &pos, int &argsize) {
  if (pos == argsize){
    //if i exceeds size of argv, realloc argv with double size
    char** temp = new char*[argsize*2]();
    for (int i = 0; i < argsize*2; ++i)
      temp[i] = NULL;//initialized to NULL
    std::copy(*argv, *argv + argsize, temp);
    for (int i = 0; i < argsize; ++i)
      delete [] (*argv)[i];
    delete [] *argv;
    *argv = temp;
    argsize = argsize * 2;
  }
  (*argv)[pos] = new char[len+1];
  strncpy((*argv)[pos], cur, len);
  (*argv)[pos][len] = 0;
  pos = pos+1;
  return 0;
}

int regchk(char* cmd) throw (invalid_cmd) {
  struct stat buf;
  if (!(stat(cmd, &buf) == 0 && S_ISREG(buf.st_mode))) {
    invalid_cmd inv;
    throw inv;
    return -1;
  }else
    return 0;
}

int pathchk(char** cmd, char const * path, int dirlen) throw(invalid_cmd) {
  //This method checks whether there is a required file in the path.
  //If path is provided, it will look for [path]/[cmd]
  //otherwise, it simply look for [cmd]
  //If the file found, it replace the [cmd] with its full path version.
  //then return 0
  char* temp;
  if (path != NULL) {
    int cmdlen;
    cmdlen = strlen(*cmd);
    temp = new char[dirlen+1+cmdlen+1];
    for (int i = 0; i < dirlen + 1 + cmdlen + 1; ++i) temp[i] = 0;
    strncpy(temp, path, dirlen);
    strcat(temp, "/");
    strcat(temp, *cmd);
    temp[dirlen+1+cmdlen] = 0;
    try {
      if(regchk(temp) == 0) {
	delete [] *cmd;
	*cmd = temp;
	return 0;
      }else
	delete [] temp;
      return -1;
    }catch (invalid_cmd& e) {
      delete [] temp;
      throw;
    }
  }else return -1;
}

int pathchk(char** cmd) throw(invalid_cmd) {
  try{
    return regchk(*cmd);
  }catch (std::exception& e) {
    throw;
  }
  return -1;
}

int pathapd (char** cmd, char const * pPath) throw(invalid_cmd) {
  //This method decides whether to and how to append cmd to its full path
  //if cmd shows a relative/absolute pattern, it will try to validate the path
  //if cmd is a single command without path, then it will search in PATH
  //in this case, if a appropriate instance is found, it will append cmd to its full path.

  char* ptr;
  ptr = strchr(*cmd, '/');
  if (ptr == NULL) {
    //this is a single command without path, search in $PATH
    char const * com;
    char const * cur;
    cur = pPath;
    if (pPath == NULL) throw (invalid_cmd());
    while (strlen(cur)) {
      //search for next ':'
      com = strchr(cur, ':');
      int dirlen = (com == NULL) ? strlen(cur) : com-cur;
      try{
	if (pathchk(cmd, cur, dirlen) == 0)
	  return 0;
      } catch (invalid_cmd& e) {}
      if (com == NULL) {//end reached
	invalid_cmd e;
	throw(e);
      }else//starting from the char after ':'
	cur = com + 1;
    }
  } else { //this is a full-path command, check for file
    try {
      if (pathchk(cmd) == 0)
	return 0;
    } catch (invalid_cmd& e) {
      throw;
    }
  }
  return -1;
}
