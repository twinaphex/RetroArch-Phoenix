#ifndef NALL_BIT_H
#define NALL_BIT_H

//round up to next highest single bit:
//round(15) == 16, round(16) == 16, round(17) == 32
static inline unsigned round(unsigned x)
{
   if((x & (x - 1)) == 0)
      return x;
   while(x & (x - 1))
      x &= x - 1;
   return x << 1;
}

#endif
