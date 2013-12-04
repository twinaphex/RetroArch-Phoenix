#ifndef NALL_STRING_COMPARE_HPP
#define NALL_STRING_COMPARE_HPP

static inline char chrlower(char c)
{
  return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c;
}

static inline char chrupper(char c)
{
  return (c >= 'a' && c <= 'z') ? c - ('a' - 'A') : c;
}

static inline int istrcmp(const char *str1, const char *str2)
{
  while(*str1)
  {
    if(chrlower(*str1) != chrlower(*str2))
       break;
    str1++, str2++;
  }
  return (int)chrlower(*str1) - (int)chrlower(*str2);
}

static inline bool wildcard(const char *s, const char *p)
{
   const char *cp = 0, *mp = 0;
   while(*s && *p != '*') {
      if(*p != '?' && *s != *p) return false;
      p++, s++;
   }
   while(*s) {
      if(*p == '*') {
         if(!*++p) return true;
         mp = p, cp = s + 1;
      } else if(*p == '?' || *p == *s) {
         p++, s++;
      } else {
         p = mp, s = cp++;
      }
   }
   while(*p == '*') p++;
   return !*p;
}

static inline bool iwildcard(const char *s, const char *p)
{
   const char *cp = 0, *mp = 0;
   while(*s && *p != '*') {
      if(*p != '?' && chrlower(*s) != chrlower(*p)) return false;
      p++, s++;
   }
   while(*s) {
      if(*p == '*') {
         if(!*++p) return true;
         mp = p, cp = s + 1;
      } else if(*p == '?' || chrlower(*p) == chrlower(*s)) {
         p++, s++;
      } else {
         p = mp, s = cp++;
      }
   }
   while(*p == '*') p++;
   return !*p;
}

static inline bool strbegin(const char *str, const char *key)
{
   int i, ssl = strlen(str), ksl = strlen(key);

   if(ksl > ssl)
      return false;
   return (!memcmp(str, key, ksl));
}

static inline bool istrbegin(const char *str, const char *key)
{
   int ssl = strlen(str), ksl = strlen(key);

   if(ksl > ssl)
      return false;
   for(int i = 0; i < ksl; i++)
   {
      if(str[i] >= 'A' && str[i] <= 'Z')
         if(str[i] != key[i] && str[i]+0x20 != key[i])return false;
      else if(str[i] >= 'a' && str[i] <= 'z')
         if(str[i] != key[i] && str[i]-0x20 != key[i])return false;
      else
         if(str[i] != key[i])return false;
   }
   return true;
}

static inline bool strend(const char *str, const char *key)
{
   int ssl = strlen(str), ksl = strlen(key);

   if(ksl > ssl) return false;
   return (!memcmp(str + ssl - ksl, key, ksl));
}

static inline bool istrend(const char *str, const char *key)
{
  int ssl = strlen(str), ksl = strlen(key);

  if(ksl > ssl) return false;
  for(int i = ssl - ksl, z = 0; i < ssl; i++, z++) {
    if(str[i] >= 'A' && str[i] <= 'Z') {
      if(str[i] != key[z] && str[i]+0x20 != key[z])return false;
    } else if(str[i] >= 'a' && str[i] <= 'z') {
      if(str[i] != key[z] && str[i]-0x20 != key[z])return false;
    } else {
      if(str[i] != key[z])return false;
    }
  }
  return true;
}

#endif
