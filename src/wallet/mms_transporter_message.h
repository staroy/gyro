#ifndef __mms_transporter_message_h__
#define __mms_transporter_message_h__

#include "serialization/keyvalue_serialization.h"
#include "cryptonote_basic/cryptonote_basic.h"
#include "cryptonote_basic/cryptonote_boost_serialization.h"
#include "cryptonote_basic/account_boost_serialization.h"
#include "cryptonote_basic/cryptonote_basic.h"
#include "net/http_server_impl_base.h"
#include "net/http_client.h"
#include "net/abstract_http_client.h"
#include "common/util.h"
#include "wipeable_string.h"
#include <vector>

namespace mms
{

struct transport_message_t
{
  cryptonote::account_public_address source_monero_address;
  cryptonote::account_public_address destination_monero_address;
  //crypto::chacha_iv iv;
  //crypto::public_key encryption_public_key;
  uint64_t timestamp;
  uint32_t type;
  std::string subject;
  std::string content;
  crypto::hash hash;
  crypto::signature signature;
  uint32_t round;
  uint32_t signature_count;
  std::string transport_id;

  BEGIN_KV_SERIALIZE_MAP()
    KV_SERIALIZE(source_monero_address)
    KV_SERIALIZE(destination_monero_address)
    //KV_SERIALIZE_VAL_POD_AS_BLOB(iv)
    //KV_SERIALIZE_VAL_POD_AS_BLOB(encryption_public_key)
    KV_SERIALIZE(timestamp)
    KV_SERIALIZE(type)
    KV_SERIALIZE(subject)
    KV_SERIALIZE(content)
    KV_SERIALIZE_VAL_POD_AS_BLOB(hash)
    KV_SERIALIZE_VAL_POD_AS_BLOB(signature)
    KV_SERIALIZE(round)
    KV_SERIALIZE(signature_count)
    KV_SERIALIZE(transport_id)
  END_KV_SERIALIZE_MAP()
};

typedef epee::misc_utils::struct_init<transport_message_t> transport_message;

}

#endif
