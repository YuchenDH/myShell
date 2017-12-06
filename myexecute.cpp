#include "myexecute.h"

int single_execute(std::string command, mydict& env, int fd_in = -1, int fd_out = -1) {
  //std::cout << "Executing " << command << " < " << fd_arr[0];
  //std::cout << " > "  << fd_arr[1] << "\n";
  //only used to execute non-piped command
  int ret_val = 0;
  //1. Judge empty & exit command
  
  if (command == "")
    return ret_val;
  if (command == "exit")
    exit(0);

  char **argv, **envp;
  int argc;
  int *fd_arr;
  try {
    //1.5 envp set
    envp = env.envp();
    
    //2. argv & argc set
    argc = decomp(&argv, command);
    assert(argv);
    if (argc < 1)
      return ret_val;

    //3. IO redirect
    fd_arr = ioredir(argc, &argv);
    if (fd_arr[0]!=-1 && fd_in!=-1)
      throw (illegal_redir());
    else if (fd_in != 0)
      fd_arr[0] = fd_in;
    if (fd_arr[1]!=-1 && fd_out!=-1)
      throw (illegal_redir());
    else if (fd_out != 0)
      fd_arr[1] = fd_out;
    
    //4. cd
    if (strcmp(argv[0], "cd") == 0) {
      if (argc > 1) {
	if (chdir(argv[1]) != 0) {
	  std::cout << "cd:    : No such file or directory\n";
	  std::cout << argc;
	}
      } else if (argc == 1) {
	chdir(env.find("HOME")->second.c_str());
      } 
      throw (safe());
    }
    else if (strcmp(argv[0], "set") == 0) {
      if (argc > 2) {
	std::string key = argv[1];
	execset(command, key, env);
	throw (safe());
      } else {
	std::cout << "Usage: set [key] [value]\n";
	throw (safe());
      }
    }
    else if (strcmp(argv[0], "export") == 0) {
      if (argc == 2) {
	std::string key = argv[1];
	if (env.exp(key) == -1)
	  std::cout << "Undefined object " << key << std::endl;
	throw (safe());
      } else {
	std::cout << "Usage: export [key]\n";
	throw (safe());
      }
    }

    //5. path append
    pathapd(argv, env.find("PATH")->second.c_str());
    
    //7. fork and execute
    forkexe(argv, envp, fd_arr);

    //*catch any exception
  } catch (safe& e) {
    ret_val = 0;
  } catch(invalid_cmd& e) {
    std::cout << "Command " << argv[0] << " not found\n";
    ret_val = 1;
  } catch(illegal_redir& e) {
    for (int k = 0; k < 3; ++k) 
      if (fd_arr[k] != -1) close(fd_arr[k]);

    //if(fd_in != -1) close(fd_in);
    //if(fd_out != -1) close(fd_out);
    throw;
  } catch (std::exception& e) {
    int deli = 0;
    while (argv[deli]) delete[] argv[deli++];
    delete[] argv;
    deli = 0;
  }
  
  //8. end up and clean up alloced mem
  int deli = 0;
  while (argv[deli])
    delete[] argv[deli++];
  delete[] argv;
  deli = 0;
  while (envp[deli])
    delete[] envp[deli++];
  delete[] envp;
  deli = 0;
  delete[] fd_arr;

  return ret_val;
}

int forkexe(char** argv, char** envp, int* fd_arr) {
  pid_t cpid = fork();
  if (cpid == -1) {//fork failed
    perror("fork");
    throw(failed_fork());
  } else if (cpid == 0) {//this is child process
    try {
      if (fd_arr)
	for (size_t p = 0; p < 3; ++p) {
	  if (fd_arr[p] > -1) {
	    if (dup2(fd_arr[p], p) == -1) {
	      perror("dup2");
	      throw(failed_redir());
	    }
	  }
	}
      
      if (execve(argv[0], &argv[0], envp) < 0) {
	perror("error on execve");
	throw(failed_exec());
      }
      
    } catch(failed_redir& e) {
      for (size_t p = 0; p < 3; ++p)
	if (fd_arr[p]!=-1) 
	  close(fd_arr[p]);
    } catch(failed_exec& e) {
      for (size_t p = 0; p < 3; ++p)
	if (fd_arr[p]!=-1)
	  close(fd_arr[p]);
    }
    exit(EXIT_FAILURE);
  } else {//this is parent process
    int status;
    
    if (waitpid(cpid, &status, 0) == -1) {
      perror("waitpid()");
      std::exception waitpid;
      throw(waitpid);
    } else {
      if (WIFEXITED(status)) {
	std::cout << "Program exited with status ";
	std::cout << WEXITSTATUS(status) << std::endl;
	return 0;
      } else if (WIFSIGNALED(status)) {
	std::cout << "Program was killed by signal ";
	std::cout << WTERMSIG(status) << std::endl;
	return -1;
      }
    }
  }
  return -1;
}

void execset( std::string command, std::string key, mydict& env) {
  size_t setpos = command.find("set ", 0);
  size_t endofkey = command.find(key, setpos + 4) + key.size();

  std::string val = command.substr(endofkey+1);
  env.insert(key, val);
}

int execute(std::string command, mydict& env, int fd_in = -1){
  //check if there is any | in command
  size_t pos = command.find('|');
  int newPipe[2];
  
  if (pos != std::string::npos) {
    std::string front_command = command.substr(0, pos);
    std::string back_command = command.substr(pos+1);
    //build a new pipe newPipe
    if (pipe(newPipe) == -1) {
      perror("pipe() failed\n");
      return -1;
    }

    pid_t cpid = fork();

    if (cpid < 0) {
      perror("fork() failed\n");
    }else if (cpid == 0) {
      //child process
      //single_execute with pPipe_in(if exist)/newPipe[in]
      single_execute(front_command, env, fd_in, newPipe[1]);
      close(newPipe[0]);
      close(newPipe[1]);
      exit(EXIT_SUCCESS);
    }else {
      //parent process
      //pipe_excute(remaining command, env, newPipe[out])
      execute(back_command, env, newPipe[0]);
      int status;
      if (waitpid(cpid, &status, 0) == -1) {
	perror("waitpid()");
      }
      close(newPipe[0]);
      close(newPipe[1]);
      return 0;
    }
  }else//single command
    single_execute(command, env, fd_in, -1);
  
  return 0;
}
