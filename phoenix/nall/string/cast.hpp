#ifndef NALL_STRING_CAST_HPP
#define NALL_STRING_CAST_HPP

namespace nall {

//this is needed, as C++0x does not support explicit template specialization inside classes
template<> inline const char* to_string<bool>         (bool v)          { return v ? "true" : "false"; }
template<> inline const char* to_string<signed int>   (signed int v)    { static char temp[256]; snprintf(temp, 255, "%+d", v); return temp; }
template<> inline const char* to_string<unsigned int> (unsigned int v)  { static char temp[256]; snprintf(temp, 255, "%u", v); return temp; }
template<> inline const char* to_string<intmax_t>     (intmax_t v)      { static char temp[256]; snprintf(temp, 255, "%+lld", (long long)v); return temp; }
template<> inline const char* to_string<uintmax_t>    (uintmax_t v)     { static char temp[256]; snprintf(temp, 255, "%llu", (unsigned long long)v); return temp; }
template<> inline const char* to_string<double>       (double v)        { static char temp[256]; snprintf(temp, 255, "%f", v); return temp; }
template<> inline const char* to_string<char*>        (char *v)         { return v; }
template<> inline const char* to_string<const char*>  (const char *v)   { return v; }
template<> inline const char* to_string<string>       (string v)        { return v; }
template<> inline const char* to_string<const string&>(const string &v) { return v; }

template<typename T> lstring& lstring::operator<<(T value) {
  operator[](size()).assign(to_string<T>(value));
  return *this;
}
}

#endif
