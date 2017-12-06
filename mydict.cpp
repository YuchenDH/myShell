#include "mydict.h"
#include <sstream>

char** mydict_t::c_str() {
  int i = 0;
  char** ans;
  if (this->empty()) {
    ans = new char*[1];
    ans[0] = NULL;
    return ans;
  } else {
    ans = new char*[this->size()+1];
    for (auto it = this->begin(); it != this->end(); ++it) {
      std::stringstream ss;
      ss << it->first << "=" << it->second;
      ans[i] = new char[ss.str().length()+1];
      strcpy(ans[i], ss.str().c_str());
      ++i;
    }
    ans[i] = NULL;
  }
  return ans;
}

mydict::mydict() : dict(mydict_t()), myenv(mydict_t()){}

mydict::mydict(char** envi) : dict(mydict_t()), myenv(mydict_t()){
  int i = 0;
  while (envi[i]) {
    char* p = strchr(envi[i], '=');
    char* key = new char[p-envi[i] + 1];
    char* val = new char[strlen(p)];

    strncpy(key, envi[i], p-envi[i]);
    key[p-envi[i]] = 0;
    strcpy(val, p+1);

    dict.insert({key, val});
    myenv.insert({key,val});
    
    delete[] key;
    delete[] val;
    ++i;
  }
}

void mydict::insert(std::string key, std::string val) {
  if (dict.find(key) == dict.end()) {
    dict.insert({key, val});
  } else {
    dict.find(key)->second = val;
  }
  //std::cout << "Inserted " << key << ", " << val << std::endl;
}

int mydict::exp(std::string key) {
  //return 0 if successfully export
  //return -1 if key not found in dict
  if (dict.find(key) != dict.end()) {
    if (myenv.find(key) != myenv.end()) {
      myenv.find(key)->second = dict.find(key)->second;
    } else {
      myenv.insert({dict.find(key)->first, dict.find(key)->second});
    }
    return 0;
  } else
    return -1;
}

std::map<std::string, std::string>::iterator mydict::find(const std::string& k) {
  return this->dict.find(k);
}

std::map<std::string, std::string>::iterator mydict::end () {
  return this->dict.end();
}

char** mydict::envp() {return myenv.c_str();}
