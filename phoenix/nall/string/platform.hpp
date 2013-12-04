#ifndef NALL_STRING_PLATFORM_HPP
#define NALL_STRING_PLATFORM_HPP

namespace nall {

string currentpath() {
  char path[PATH_MAX];
  if(::getcwd(path)) {
    strtr(path, "\\", "/");
    if(strend(path, "/") == false)
       strlcat(path, "/", sizeof(path));
    string result(path);
    return result;
  }
  return "./";
}

string userpath() {
  char path[PATH_MAX];
  if(::userpath(path)) {
    strtr(path, "\\", "/");
    if(strend(path, "/") == false)
       strlcat(path, "/", sizeof(path));
    string result(path);
    return result;
  }
  return currentpath();
}

string realpath(const char *name) {
  char path[PATH_MAX];
  if(::realpath(name, path)) {
    strtr(path, "\\", "/");
    string result(path);
    return result;
  }
  return userpath();
}

}

#endif
