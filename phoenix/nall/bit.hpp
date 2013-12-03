#ifndef NALL_BIT_HPP
#define NALL_BIT_HPP

namespace nall {
  namespace bit {
    //lowest(0b1110) == 0b0010
    template<typename T> inline T lowest(const T x) {
      return x & -x;
    }

    //clear_lowest(0b1110) == 0b1100
    template<typename T> inline T clear_lowest(const T x) {
      return x & (x - 1);
    }

    //set_lowest(0b0101) == 0b0111
    template<typename T> inline T set_lowest(const T x) {
      return x | (x + 1);
    }

    //round up to next highest single bit:
    //round(15) == 16, round(16) == 16, round(17) == 32
    inline unsigned round(unsigned x) {
      if((x & (x - 1)) == 0) return x;
      while(x & (x - 1)) x &= x - 1;
      return x << 1;
    }
  }
}

#endif
