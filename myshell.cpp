#include <iostream>
#include <sstream>
#include <errno.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string.h>

#include "mydict.h"
#include "stoarg.h"
#include "myexception.h"
#include "myvar.h"
#include "myexecute.h"

#define DEF_ARGC 8

void printsuffix();

extern char **environ;

int main(int argc, char** argv) {

  mydict env(environ);

  while (true) {
    
    printsuffix();

    std::string command;
    if (std::cin) {
      std::getline(std::cin, command);
      varsubstitute(command, env);
      //std::cout << "Your command is " << command <<std::endl;
      //if (command == "exit") return 0;
      execute(command, env, -1);
    }
    if (std::cin.eof()) {
      std::cout << "[EOF reached]\n";
      break;
    }
  }
  return EXIT_SUCCESS;
}

void printsuffix() {
  char* curdir = get_current_dir_name();
  std::cout << "myShell:" << curdir << " $";
  free(curdir);
}
