#ifndef __wallet_zyre_util_h__
#define __wallet_zyre_util_h__

#include <string>

#include "crypto/crypto.h"
#include "wallet2.h"

namespace zyre { namespace wallet {

  crypto::secret_key get_sms_secret_key(const tools::wallet2 *w_ptr);
  crypto::public_key get_sms_public_key(const crypto::secret_key& sec);
  std::string get_sms_salt(const crypto::secret_key& sec);
  void encrypt(const crypto::public_key public_key, const std::string &plaintext, std::string &ciphertext, crypto::public_key &encryption_public_key, crypto::chacha_iv &iv);
  void decrypt(const std::string &ciphertext, const crypto::public_key &encryption_public_key, const crypto::chacha_iv &iv, const crypto::secret_key &view_secret_key, std::string &plaintext);

}}

#endif
