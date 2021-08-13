#ifndef __spinner_type_h__
#define __spinner_type_h__

#include "cryptonote_basic.h"
#include "serialization/binary_archive.h"
#include "serialization/variant.h"
#include "serialization/string.h"
#include "serialization/vector.h"
#include "serialization/crypto.h"
#include "serialization/binary_utils.h"
#include "serialization/spin_type.h"
#include "serialization/gyro_type.h"

#include "common/spin.h"
#include "common/gyro_raw_type.h"

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
    uint64_t approximately_count; // approximately count of spinner 
    uint64_t locked; // base locked
    uint64_t medium_locked; // medium of base locked
    uint64_t spin; // spin
    uint64_t medium_spin; // spin
    uint64_t interval; // time interval
    uint64_t medium_interval; // medium time interval

    BEGIN_SERIALIZE()
      FIELDS(nfo)
      VARINT_FIELD(approximately_count)
      VARINT_FIELD(locked)
      VARINT_FIELD(medium_locked)
      VARINT_FIELD(spin)
      VARINT_FIELD(medium_spin)
      VARINT_FIELD(interval)
      VARINT_FIELD(medium_interval)
    END_SERIALIZE()
  };
}
  
#endif
  
