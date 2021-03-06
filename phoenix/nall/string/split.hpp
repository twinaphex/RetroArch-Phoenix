#ifndef NALL_STRING_SPLIT_HPP
#define NALL_STRING_SPLIT_HPP

namespace nall {

template<unsigned Limit, bool Insensitive, bool Quoted> lstring& lstring::usplit(const char *key, const char *base) {
  reset();
  if(!key || !*key) return *this;

  const char *p = base;
  unsigned counter = 0;

  while(*p) {
    if(Limit) if(counter >= Limit) break;
    if(quoteskip<Quoted>(p)) continue;
    for(unsigned n = 0;; n++) {
      if(key[n] == 0) {
        strlcpy(operator[](counter++), base, (unsigned)(p - base + 1));
        p += n;
        base = p;
        break;
      }
      if(!chrequal<Insensitive>(key[n], p[n])) { p++; break; }
    }
  }

  operator[](counter) = base;
  return *this;
}

template<unsigned Limit> lstring& lstring::split(const char *key, const char *src) { return usplit<Limit, false, false>(key, src); }
template<unsigned Limit> lstring& lstring::qsplit(const char *key, const char *src) { return usplit<Limit, false, true>(key, src); }

};

#endif
