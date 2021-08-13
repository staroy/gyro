#ifndef __spinner_type_h__
#define __spinner_type_h__

#include "cryptonote_basic.h"
#include "gyro.h"
#include "serialization/binary_archive.h"
#include "serialization/variant.h"
#include "serialization/string.h"
#include "serialization/vector.h"
#include "serialization/crypto.h"
#include "serialization/binary_utils.h"

namespace cryptonote
{
  //---------------------------------------------------------------
  struct spinner_tx_proof
  {
    struct pair {
      crypto::public_key key;
      crypto::signature sig;

      BEGIN_SERIALIZE()
        FIELDS(key)
        FIELDS(sig)
      END_SERIALIZE()
    };

    crypto::hash txid;
    std::vector<pair> pairs;

    BEGIN_SERIALIZE()
      FIELDS(txid)
      FIELDS(pairs)
    END_SERIALIZE()
  };

  //---------------------------------------------------------------
  struct spinner_info
  {
    cryptonote::account_public_address adr; // spinner address
    spinner_tx_proof proof; // tx proof
    crypto::public_key pub; // spinner session public key
    crypto::signature sig; // signature of spinner session public key on spinner view key
    uint64_t prevuos_height; // prevuos height of spinner block (0 if first)

    BEGIN_SERIALIZE()
      FIELDS(adr)
      FIELDS(proof)
      FIELDS(pub)
      FIELDS(sig)
      VARINT_FIELD(prevuos_height)
    END_SERIALIZE()
  };
  //---------------------------------------------------------------
  struct spinner_data
  {
    spinner_info nfo;
    uint64_t all_count; // count of all spinners
    uint64_t locked; // medium of base locked
    uint64_t medium_locked; // medium of base locked
    spin_type spin; // spin
    spin_type medium_spin; // spin

    uint64_t get_interval_for_next_time();

    BEGIN_SERIALIZE()
      FIELDS(nfo)
      VARINT_FIELD(all_count)
      VARINT_FIELD(medium_locked)
      uint64_t spin_hi, spin_lo, medium_spin_hi, medium_spin_lo;
      if (typename Archive<W>::is_saving()) {
        spin_hi = ((spin >> 64) & 0xffffffffffffffff).convert_to<uint64_t>();
        spin_lo = (spin & 0xffffffffffffffff).convert_to<uint64_t>();
        medium_spin_hi = ((medium_spin >> 64) & 0xffffffffffffffff).convert_to<uint64_t>();
        medium_spin_lo = (medium_spin & 0xffffffffffffffff).convert_to<uint64_t>();
      }
      VARINT_FIELD(spin_hi)
      VARINT_FIELD(spin_lo)
      VARINT_FIELD(medium_spin_hi)
      VARINT_FIELD(medium_spin_lo)
      if (!typename Archive<W>::is_saving()) {
        spin = spin_hi;
        spin <<= 64;
        spin |= spin_lo;
        medium_spin = medium_spin_hi;
        medium_spin <<= 64;
        medium_spin |= medium_spin_lo;
      }
    END_SERIALIZE()
  };

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
}
  
#endif
  
