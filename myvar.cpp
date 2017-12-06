#include "myvar.h"

int varsubstitute (std::string& command, mydict dict) {
  int cnt = 0;
  std::size_t dol;
  std::size_t spa;
  std::size_t cur = 0;
  while (true) {
    //find next occurance of '$'
    dol = command.find('$', cur);

    if (dol != std::string::npos) {
      //if found, find next occurance of ' ' to complete a var name
      
      spa = command.find(' ', dol+1);
      if (spa != std::string::npos) {
	//if found, extract the var name
	std::string key = command.substr(dol+1, spa-dol-1);

	//search in the dict for var value
	if (dict.find(key) != dict.end()) {
	  //if there is such a value, replace var $name with the value
	  std::string val = dict.find(key)->second;
	  command.replace(dol, key.size()+1, val);
	  ++cnt;
	}else {
	  cur = dol + 1; //else, continue searching
	  continue;
	}
	
      }else {
	// reached the end, extract the var name to the end
	std::string key = command.substr(dol+1);

	if (dict.find(key) != dict.end()) {
	  std::string val = dict.find(key)->second;
	  command.replace(dol, key.size()+1, val);
	  ++cnt;
	}else {
	  cur = dol + 1;
	  continue;
	}
      }
    }else break;//if no $ found, exit cycle
  }
  return cnt;
}

int* ioredir(int& argc, char *** argv) {
  int* fd = new int[3];
  int del[3];
  //fd contains overriding filedescriptor.
  //if such overriding does not exist, return fd = &{-1, -1, -1}.
  //Caller free fd!
  
  for (int i = 0; i < 3; ++i){
    fd[i] = -1;
    del[i] = -1;
  }
  
  for (int i = 1; i < argc - 1; ++i) {
    if (strcmp((*argv)[i], "<") == 0) {
      fd[0] = ioredir_helper((*argv)[i+1], O_RDONLY);
      del[0] = i;
    }else if (strcmp((*argv)[i], ">") == 0) {
      fd[1] = ioredir_helper((*argv)[i+1], O_CREAT|O_RDWR);
      del[1] = i;
    }else if (strcmp((*argv)[i], "2>") == 0) {
      fd[2] = ioredir_helper((*argv)[i+1], O_CREAT|O_RDWR);
      del[2] = i;
    }
  }

  for (int i = 0; i < 3; ++i) {
    if (del[i] > 0) {
      char** newargv = new char*[argc-1];
      int j, k;
      j = 0;
      k = 0;
      while ( (*argv)[j] ) {
	if ( j != del[i] && j != del[i]+1 ) {
	  newargv[k++] = (*argv)[j++];
	}else
	  delete [] (*argv)[j++];
      }
      newargv[k] = NULL;
      delete [] *argv;
      *argv = newargv;
      argc = argc-2;
    }
  }
  
  return fd;
}

int ioredir_helper(char* filename, int flag) {
  int fd = -1;
  fd = open(filename, flag, 0666);
  if (fd < 0) {
    std::cerr << "error opening file " << filename << "\n";
    perror("open(filename)");
  }
  return fd;
}
