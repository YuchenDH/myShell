#ifndef __MYDICT_H__
#define __MYDICT_H__
#include <iostream>
#include <cstring>
#include <string.h>
#include <map>

//This class is a map<std::string, std::string> with method to export a c-style string array used as envp
class mydict_t : public std::map<std::string, std::string> {
 public:
  char** c_str();
};

class mydict {
  mydict_t dict;
  mydict_t myenv;

 public:
  mydict();
  mydict(char** envi);
  void insert(std::string key, std::string val);
  int exp(std::string key);
  std::map<std::string, std::string>::iterator find (const std::string& k);
  std::map<std::string, std::string>::iterator end ();
  char** envp();
  void print();
};

#endif
