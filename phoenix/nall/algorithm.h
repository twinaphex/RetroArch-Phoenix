#ifndef NALL_ALGORITHM_H
#define NALL_ALGORITHM_H

#undef min
#undef max

template<typename T, typename U> T min(const T &t, const U &u)
{
   return t < u ? t : u;
}

template<typename T, typename U> T max(const T &t, const U &u)
{
   return t > u ? t : u;
}

#endif
