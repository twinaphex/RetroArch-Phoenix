#ifndef NALL_UTILITY_HPP
#define NALL_UTILITY_HPP

#include <type_traits>
#include <utility>

namespace nall {
  template<bool C, typename T = bool> struct enable_if { typedef T type; };
  template<typename T> struct enable_if<false, T> {};
  template<typename C, typename T = bool> struct mp_enable_if : enable_if<C::value, T> {};

  template<typename T> struct base_from_member {
    T value;
    base_from_member(T value_) : value(value_) {}
  };

  template<typename T> class optional {
    bool valid;
    T value;
  public:
    inline operator bool() const { return valid; }
    inline const T& operator()() const { if(!valid) throw; return value; }
    inline optional<T>& operator=(const optional<T> &source) { valid = source.valid; value = source.value; return *this; }
    inline optional(bool valid, const T &value) : valid(valid), value(value) {}
  };
}

#endif
