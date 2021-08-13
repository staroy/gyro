#include "spin_type.h"

namespace cryptonote
{
  spin_type get_spin(const crypto::hash& txid, const crypto::hash& blk_hash, uint64_t m)
  {
    union {
      struct {
        int32_t x1, y1, x2, y2, x3, y3, x4, y4;
      } v;
      crypto::hash h;
    } c;

    c.h = txid;

    int32_t x = c.v.x1; x ^= c.v.x2; x ^= c.v.x3; x ^= c.v.x4;
    int32_t y = c.v.y1; y ^= c.v.y2; y ^= c.v.y3; y ^= c.v.y4;

    c.h = blk_hash;

    x ^= c.v.x1; x ^= c.v.x2; x ^= c.v.x3; x ^= c.v.x4;
    y ^= c.v.y1; y ^= c.v.y2; y ^= c.v.y3; y ^= c.v.y4;

    int64_t xx = x; xx *= x;
    int64_t yy = y; yy *= y;

    uint64_t r = m; r *= m;
    int64_t k = (xx + yy) / int64_t(r);

    spin_type ret;

    ret.x = (x < 0) ? -isqrt(xx / k) : isqrt(xx / k);
    ret.y = (y < 0) ? -isqrt(yy / k) : isqrt(yy / k);

    return ret;
  }

}
