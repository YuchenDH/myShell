#ifndef __MYEXCEPTION_H__
#define __MYEXCEPTION_H__

#include <exception>

class invalid_cmd : public std::exception {
 public:
  virtual const char* what() const throw() {
    return "Invald argument occurred";
  }
};

class safe : public std::exception {
 public:
  virtual const char* what() const throw() {
    return "";
  }
};

class failed_redir : public std::exception {
 public:
  virtual const char* what() const throw() {
    return "Failed to redirect stdio/stderr";
  }
};

class illegal_redir : public std::exception {
 public:
  virtual const char* what() const throw() {
    return "redirection conflict with usage of pipe";
  }
};

class failed_fork : public std::exception {
 public:
  virtual const char* what() const throw() {
    return "fork() failed";
  }
};

class failed_exec : public std::exception {
 public:
  virtual const char* what() const throw() {
    return "execve() failed";
  }
};

#endif
