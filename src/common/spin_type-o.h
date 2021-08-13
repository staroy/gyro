#ifndef __spin_type_h__
#define __spin_type_h__

#include <math.h>
#include <limits>
#include "crypto/hash.h"

namespace cryptonote
{
  template <typename T>
  T sqrt_helper(T x, T lo, T hi)
  {
    if (lo == hi)
      return lo;
  
    const T mid = (lo + hi + 1) / 2;
    if (x / mid < mid)
      return sqrt_helper<T>(x, lo, mid - 1);
    return sqrt_helper(x, mid, hi);
  }
  
  template <typename T>
  T ct_sqrt(T x)
  {
    return sqrt_helper<T>(x, 0, x / 2 + 1);
  }
  
  template <typename T>
  T isqrt(T x)
  {
    T r = (T) std::sqrt((double) x);
    T sqrt_max = ct_sqrt(std::numeric_limits<T>::max());
  
    while (r < sqrt_max && r * r < x)
      r++;
    while (r > sqrt_max || r * r > x)
      r--;
  
    return r;
  }

  struct spin_type
  {
    int64_t x;
    int64_t y;

    inline bool operator == (const spin_type& s) { return x == s.x && y == s.y; }
    inline bool operator != (const spin_type& s) { return x != s.x || y != s.y; }

    inline uint64_t m() const
    {
      return isqrt(uint64_t(x*x) + uint64_t(y*y));
    }
  };

  spin_type get_spin(const crypto::hash& txid, const crypto::hash& blk_hash, uint64_t m);
}
  
#endif
  
