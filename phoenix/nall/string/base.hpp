#ifndef NALL_STRING_BASE_HPP
#define NALL_STRING_BASE_HPP

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <nall/concept.hpp>
#include <nall/function.hpp>
#include <nall/vector.hpp>
#include <nall/windows/utf8.hpp>

namespace nall {
  class string;
  class lstring;
  template<typename T> inline const char* to_string(T);

  class string {
  public:
    inline void reserve(unsigned);

    template<typename... Args> inline string& assign(Args&&... args);
    template<typename... Args> inline string& append(Args&&... args);

    inline bool readfile(const string&);

    template<unsigned Limit = 0> inline string& replace(const char*, const char*);
    template<unsigned Limit = 0> inline string& ireplace(const char*, const char*);
    template<unsigned Limit = 0> inline string& qreplace(const char*, const char*);
    template<unsigned Limit = 0> inline string& iqreplace(const char*, const char*);

    inline bool equals(const char*) const;
    inline bool iequals(const char*) const;

    inline bool wildcard(const char*) const;
    inline bool iwildcard(const char*) const;

    inline bool beginswith(const char*) const;
    inline bool ibeginswith(const char*) const;
    inline bool iendswith(const char*) const;

    template<unsigned limit = 0> inline string& ltrim(const char *key = " ");
    template<unsigned limit = 0> inline string& rtrim(const char *key = " ");
    template<unsigned limit = 0> inline string& trim(const char *key = " ", const char *rkey = 0);

    inline operator const char*() const;
    inline char* operator()();
    inline char& operator[](int);

    inline bool operator==(const char*) const;
    inline bool operator!=(const char*) const;
    inline bool operator< (const char*) const;
    inline bool operator<=(const char*) const;
    inline bool operator> (const char*) const;
    inline bool operator>=(const char*) const;

    inline string& operator=(const string&);
    inline string& operator=(string&&);

    template<typename... Args> inline string(Args&&... args);
    inline string(const string&);
    inline string(string&&);
    inline ~string();

    //internal functions
    inline string& assign_(const char*);
    inline string& append_(const char*);

  protected:
    char *data;
    unsigned size;

    template<unsigned Limit, bool Insensitive, bool Quoted> inline string& ureplace(const char*, const char*);
  };

  class lstring : public linear_vector<string> {
  public:
    template<typename T> inline lstring& operator<<(T value);

    inline optional<unsigned> find(const char*) const;
    template<unsigned Limit = 0> inline lstring& split(const char*, const char*);
    template<unsigned Limit = 0> inline lstring& qsplit(const char*, const char*);

    lstring();
    lstring(std::initializer_list<string>);

  protected:
    template<unsigned Limit, bool Insensitive, bool Quoted> inline lstring& usplit(const char*, const char*);
  };

  //convert.hpp
  inline uintmax_t hex(const char *str);
  inline intmax_t integer(const char *str);
  inline uintmax_t decimal(const char *str);
  inline uintmax_t binary(const char *str);
  inline double fp(const char *str);

  //platform.hpp
  inline string realpath(const char *name);
  inline string userpath();
  inline string currentpath();

  //strl.hpp
  inline unsigned strlcpy(char *dest, const char *src, unsigned length);
  inline unsigned strlcat(char *dest, const char *src, unsigned length);

  //strpos.hpp
  inline optional<unsigned> strpos(const char *str, const char *key);
  inline optional<unsigned> istrpos(const char *str, const char *key);
  inline optional<unsigned> qstrpos(const char *str, const char *key);
  template<bool Insensitive = false, bool Quoted = false> inline optional<unsigned> ustrpos(const char *str, const char *key);

  //trim.hpp
  template<unsigned limit = 0> inline char* ltrim(char *str, const char *key = " ");
  template<unsigned limit = 0> inline char* rtrim(char *str, const char *key = " ");
  template<unsigned limit = 0> inline char* trim(char *str, const char *key = " ", const char *rkey = 0);

  //utility.hpp
  template<bool Insensitive> alwaysinline bool chrequal(char x, char y);
  template<bool Quoted, typename T> alwaysinline bool quoteskip(T *&p);
  template<bool Quoted, typename T> alwaysinline bool quotecopy(char *&t, T *&p);
  inline unsigned strlcpy(string &dest, const char *src, unsigned length);
  inline unsigned strlcat(string &dest, const char *src, unsigned length);
  inline string substr(const char *src, unsigned start = 0, unsigned length = ~0u);
  inline string sha256(const uint8_t *data, unsigned size);

  template<unsigned length = 0, char padding = ' '> inline string integer(intmax_t value);
  template<unsigned length = 0, char padding = ' '> inline string linteger(intmax_t value);
  template<unsigned length = 0, char padding = ' '> inline string decimal(uintmax_t value);
  template<unsigned length = 0, char padding = ' '> inline string ldecimal(uintmax_t value);
  template<unsigned length = 0, char padding = '0'> inline string hex(uintmax_t value);
  template<unsigned length = 0, char padding = '0'> inline string binary(uintmax_t value);
  inline unsigned fp(char *str, double value);
  inline string fp(double value);
  inline string fp(double value, unsigned precision);
};

#endif
