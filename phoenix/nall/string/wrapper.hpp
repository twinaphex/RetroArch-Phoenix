#ifndef NALL_STRING_WRAPPER_HPP
#define NALL_STRING_WRAPPER_HPP

namespace nall {

template<unsigned limit> string& string::ltrim(const char *key) { nall::ltrim<limit>(data, key); return *this; }
template<unsigned limit> string& string::rtrim(const char *key) { nall::rtrim<limit>(data, key); return *this; }
template<unsigned limit> string& string::trim(const char *key, const char *rkey) { nall::trim <limit>(data, key, rkey); return *this; }

optional<unsigned> string::position(const char *key) const { return strpos(data, key); }
optional<unsigned> string::iposition(const char *key) const { return istrpos(data, key); }
optional<unsigned> string::qposition(const char *key) const { return qstrpos(data, key); }
optional<unsigned> string::iqposition(const char *key) const { return iqstrpos(data, key); }

}

#endif
