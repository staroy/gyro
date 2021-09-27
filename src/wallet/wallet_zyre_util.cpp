
#include "string_tools.h"
#include "misc_log_ex.h"
#include "wallet_zyre_util.h"

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "wallet.sms.util"

namespace zyre { namespace wallet {

  crypto::secret_key get_sms_secret_key(const tools::wallet2 *w_ptr)
  {
    const auto& acc = w_ptr->get_account();
    auto& hwdev = acc.get_device();
    return
      hwdev.get_subaddress_secret_key(
        hwdev.get_subaddress_secret_key(
          acc.get_keys().m_view_secret_key, {1,1}), {1,1});
  }

  crypto::public_key get_sms_public_key(const crypto::secret_key& cipher_sec)
  {
     crypto::public_key cipher_pub;
     crypto::secret_key_to_public_key(cipher_sec, cipher_pub);
     return cipher_pub;
  }
  
  std::string get_sms_salt(const crypto::secret_key& sec)
  {
     std::string tmp = "SMS-SALT-" + epee::string_tools::pod_to_hex(sec);
     crypto::hash hash;
     crypto::cn_fast_hash(tmp.data(), tmp.size(), hash);
     return epee::string_tools::pod_to_hex(hash);
  }

  void encrypt(const crypto::public_key public_key, const std::string &plaintext, std::string &ciphertext, crypto::public_key &encryption_public_key, crypto::chacha_iv &iv)
  {
    crypto::secret_key encryption_secret_key;
    crypto::generate_keys(encryption_public_key, encryption_secret_key);

    crypto::key_derivation derivation;
    bool success = crypto::generate_key_derivation(public_key, encryption_secret_key, derivation);
    THROW_WALLET_EXCEPTION_IF(!success, tools::error::wallet_internal_error, "Failed to generate key derivation for message encryption");

    crypto::chacha_key chacha_key;
    crypto::generate_chacha_key(&derivation, sizeof(derivation), chacha_key, 1);
    iv = crypto::rand<crypto::chacha_iv>();
    ciphertext.resize(plaintext.size());
    crypto::chacha20(plaintext.data(), plaintext.size(), chacha_key, iv, &ciphertext[0]);
  }

  void decrypt(const std::string &ciphertext, const crypto::public_key &encryption_public_key, const crypto::chacha_iv &iv, const crypto::secret_key &view_secret_key, std::string &plaintext)
  {
    crypto::key_derivation derivation;
    bool success = crypto::generate_key_derivation(encryption_public_key, view_secret_key, derivation);
    THROW_WALLET_EXCEPTION_IF(!success, tools::error::wallet_internal_error, "Failed to generate key derivation for message decryption");
    crypto::chacha_key chacha_key;
    crypto::generate_chacha_key(&derivation, sizeof(derivation), chacha_key, 1);
    plaintext.resize(ciphertext.size());
    crypto::chacha20(ciphertext.data(), ciphertext.size(), chacha_key, iv, &plaintext[0]);
  }

}}
