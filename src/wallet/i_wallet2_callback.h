#ifndef __I_WALLET2_CALLBACK__
#define __I_WALLET2_CALLBACK__

#include "crypto/hash.h"
#include "cryptonote_basic/cryptonote_basic.h"
#include "cryptonote_basic/subaddress_index.h"
#include "wipeable_string.h"

namespace tools {

  class i_wallet2_callback
  {
  public:
    // Full wallet callbacks
    virtual void on_new_block(uint64_t height, const cryptonote::block& block) {}
    virtual void on_money_received(uint64_t height, const crypto::hash &txid, const cryptonote::transaction& tx, uint64_t amount, const cryptonote::subaddress_index& subaddr_index, bool is_change, uint64_t unlock_time) {}
    virtual void on_sms_received(uint64_t height, const crypto::hash &txid, const cryptonote::transaction& tx, uint32_t layer, const std::string& data, uint64_t amount, const cryptonote::subaddress_index& subaddr_index) {}
    virtual void on_unconfirmed_money_received(uint64_t height, const crypto::hash &txid, const cryptonote::transaction& tx, uint64_t amount, const cryptonote::subaddress_index& subaddr_index) {}
    virtual void on_money_spent(uint64_t height, const crypto::hash &txid, const cryptonote::transaction& in_tx, uint64_t amount, const cryptonote::transaction& spend_tx, const cryptonote::subaddress_index& subaddr_index) {}
    virtual void on_skip_transaction(uint64_t height, const crypto::hash &txid, const cryptonote::transaction& tx) {}
    virtual boost::optional<epee::wipeable_string> on_get_password(const char *reason) { return boost::none; }
    // Light wallet callbacks
    virtual void on_lw_new_block(uint64_t height) {}
    virtual void on_lw_money_received(uint64_t height, const crypto::hash &txid, uint64_t amount) {}
    virtual void on_lw_unconfirmed_money_received(uint64_t height, const crypto::hash &txid, uint64_t amount) {}
    virtual void on_lw_money_spent(uint64_t height, const crypto::hash &txid, uint64_t amount) {}
    // Device callbacks
    virtual void on_device_button_request(uint64_t code) {}
    virtual void on_device_button_pressed() {}
    virtual boost::optional<epee::wipeable_string> on_device_pin_request() { return boost::none; }
    virtual boost::optional<epee::wipeable_string> on_device_passphrase_request(bool & on_device) { on_device = true; return boost::none; }
    virtual void on_device_progress(const hw::device_progress& event) {};
    // Common callbacks
    virtual void on_pool_tx_removed(const crypto::hash &txid) {}
    virtual ~i_wallet2_callback() {}
  };

}

#endif
