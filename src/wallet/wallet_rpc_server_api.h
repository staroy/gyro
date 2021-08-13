
#pragma  once

#include <string>
#include "common/util.h"
#include "math_helper.h"
#include "wallet_rpc_server_commands_defs.h"
#include "wallet2.h"

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "wallet.rpc.api"

namespace tools
{
  /************************************************************************/
  /*                                                                      */
  /************************************************************************/
  class wallet_rpc_server_api
  {
    bool m_restricted;
    wallet2 *m_wallet;

  public:
    typedef epee::net_utils::connection_context_base connection_context;

    static const char* tr(const char* str);

    wallet_rpc_server_api(wallet2 *cr);


    bool on_getbalance(const wallet_rpc::COMMAND_RPC_GET_BALANCE::request& req, wallet_rpc::COMMAND_RPC_GET_BALANCE::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_getaddress(const wallet_rpc::COMMAND_RPC_GET_ADDRESS::request& req, wallet_rpc::COMMAND_RPC_GET_ADDRESS::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_getaddress_index(const wallet_rpc::COMMAND_RPC_GET_ADDRESS_INDEX::request& req, wallet_rpc::COMMAND_RPC_GET_ADDRESS_INDEX::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_create_address(const wallet_rpc::COMMAND_RPC_CREATE_ADDRESS::request& req, wallet_rpc::COMMAND_RPC_CREATE_ADDRESS::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_label_address(const wallet_rpc::COMMAND_RPC_LABEL_ADDRESS::request& req, wallet_rpc::COMMAND_RPC_LABEL_ADDRESS::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_get_accounts(const wallet_rpc::COMMAND_RPC_GET_ACCOUNTS::request& req, wallet_rpc::COMMAND_RPC_GET_ACCOUNTS::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_create_account(const wallet_rpc::COMMAND_RPC_CREATE_ACCOUNT::request& req, wallet_rpc::COMMAND_RPC_CREATE_ACCOUNT::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_label_account(const wallet_rpc::COMMAND_RPC_LABEL_ACCOUNT::request& req, wallet_rpc::COMMAND_RPC_LABEL_ACCOUNT::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_get_account_tags(const wallet_rpc::COMMAND_RPC_GET_ACCOUNT_TAGS::request& req, wallet_rpc::COMMAND_RPC_GET_ACCOUNT_TAGS::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_tag_accounts(const wallet_rpc::COMMAND_RPC_TAG_ACCOUNTS::request& req, wallet_rpc::COMMAND_RPC_TAG_ACCOUNTS::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_untag_accounts(const wallet_rpc::COMMAND_RPC_UNTAG_ACCOUNTS::request& req, wallet_rpc::COMMAND_RPC_UNTAG_ACCOUNTS::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_set_account_tag_description(const wallet_rpc::COMMAND_RPC_SET_ACCOUNT_TAG_DESCRIPTION::request& req, wallet_rpc::COMMAND_RPC_SET_ACCOUNT_TAG_DESCRIPTION::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_getheight(const wallet_rpc::COMMAND_RPC_GET_HEIGHT::request& req, wallet_rpc::COMMAND_RPC_GET_HEIGHT::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_transfer(const wallet_rpc::COMMAND_RPC_TRANSFER::request& req, wallet_rpc::COMMAND_RPC_TRANSFER::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_transfer_split(const wallet_rpc::COMMAND_RPC_TRANSFER_SPLIT::request& req, wallet_rpc::COMMAND_RPC_TRANSFER_SPLIT::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_sign_transfer(const wallet_rpc::COMMAND_RPC_SIGN_TRANSFER::request& req, wallet_rpc::COMMAND_RPC_SIGN_TRANSFER::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_describe_transfer(const wallet_rpc::COMMAND_RPC_DESCRIBE_TRANSFER::request& req, wallet_rpc::COMMAND_RPC_DESCRIBE_TRANSFER::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_submit_transfer(const wallet_rpc::COMMAND_RPC_SUBMIT_TRANSFER::request& req, wallet_rpc::COMMAND_RPC_SUBMIT_TRANSFER::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_sweep_dust(const wallet_rpc::COMMAND_RPC_SWEEP_DUST::request& req, wallet_rpc::COMMAND_RPC_SWEEP_DUST::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_sweep_all(const wallet_rpc::COMMAND_RPC_SWEEP_ALL::request& req, wallet_rpc::COMMAND_RPC_SWEEP_ALL::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_sweep_single(const wallet_rpc::COMMAND_RPC_SWEEP_SINGLE::request& req, wallet_rpc::COMMAND_RPC_SWEEP_SINGLE::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_relay_tx(const wallet_rpc::COMMAND_RPC_RELAY_TX::request& req, wallet_rpc::COMMAND_RPC_RELAY_TX::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_make_integrated_address(const wallet_rpc::COMMAND_RPC_MAKE_INTEGRATED_ADDRESS::request& req, wallet_rpc::COMMAND_RPC_MAKE_INTEGRATED_ADDRESS::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_split_integrated_address(const wallet_rpc::COMMAND_RPC_SPLIT_INTEGRATED_ADDRESS::request& req, wallet_rpc::COMMAND_RPC_SPLIT_INTEGRATED_ADDRESS::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_store(const wallet_rpc::COMMAND_RPC_STORE::request& req, wallet_rpc::COMMAND_RPC_STORE::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_get_payments(const wallet_rpc::COMMAND_RPC_GET_PAYMENTS::request& req, wallet_rpc::COMMAND_RPC_GET_PAYMENTS::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_get_bulk_payments(const wallet_rpc::COMMAND_RPC_GET_BULK_PAYMENTS::request& req, wallet_rpc::COMMAND_RPC_GET_BULK_PAYMENTS::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_incoming_transfers(const wallet_rpc::COMMAND_RPC_INCOMING_TRANSFERS::request& req, wallet_rpc::COMMAND_RPC_INCOMING_TRANSFERS::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_rescan_blockchain(const wallet_rpc::COMMAND_RPC_RESCAN_BLOCKCHAIN::request& req, wallet_rpc::COMMAND_RPC_RESCAN_BLOCKCHAIN::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_set_tx_notes(const wallet_rpc::COMMAND_RPC_SET_TX_NOTES::request& req, wallet_rpc::COMMAND_RPC_SET_TX_NOTES::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_get_tx_notes(const wallet_rpc::COMMAND_RPC_GET_TX_NOTES::request& req, wallet_rpc::COMMAND_RPC_GET_TX_NOTES::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_set_attribute(const wallet_rpc::COMMAND_RPC_SET_ATTRIBUTE::request& req, wallet_rpc::COMMAND_RPC_SET_ATTRIBUTE::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_get_attribute(const wallet_rpc::COMMAND_RPC_GET_ATTRIBUTE::request& req, wallet_rpc::COMMAND_RPC_GET_ATTRIBUTE::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_get_tx_key(const wallet_rpc::COMMAND_RPC_GET_TX_KEY::request& req, wallet_rpc::COMMAND_RPC_GET_TX_KEY::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_check_tx_key(const wallet_rpc::COMMAND_RPC_CHECK_TX_KEY::request& req, wallet_rpc::COMMAND_RPC_CHECK_TX_KEY::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_get_tx_proof(const wallet_rpc::COMMAND_RPC_GET_TX_PROOF::request& req, wallet_rpc::COMMAND_RPC_GET_TX_PROOF::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_check_tx_proof(const wallet_rpc::COMMAND_RPC_CHECK_TX_PROOF::request& req, wallet_rpc::COMMAND_RPC_CHECK_TX_PROOF::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_get_spend_proof(const wallet_rpc::COMMAND_RPC_GET_SPEND_PROOF::request& req, wallet_rpc::COMMAND_RPC_GET_SPEND_PROOF::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_check_spend_proof(const wallet_rpc::COMMAND_RPC_CHECK_SPEND_PROOF::request& req, wallet_rpc::COMMAND_RPC_CHECK_SPEND_PROOF::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_get_reserve_proof(const wallet_rpc::COMMAND_RPC_GET_RESERVE_PROOF::request& req, wallet_rpc::COMMAND_RPC_GET_RESERVE_PROOF::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_check_reserve_proof(const wallet_rpc::COMMAND_RPC_CHECK_RESERVE_PROOF::request& req, wallet_rpc::COMMAND_RPC_CHECK_RESERVE_PROOF::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_get_transfers(const wallet_rpc::COMMAND_RPC_GET_TRANSFERS::request& req, wallet_rpc::COMMAND_RPC_GET_TRANSFERS::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_get_transfer_by_txid(const wallet_rpc::COMMAND_RPC_GET_TRANSFER_BY_TXID::request& req, wallet_rpc::COMMAND_RPC_GET_TRANSFER_BY_TXID::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_sign(const wallet_rpc::COMMAND_RPC_SIGN::request& req, wallet_rpc::COMMAND_RPC_SIGN::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_verify(const wallet_rpc::COMMAND_RPC_VERIFY::request& req, wallet_rpc::COMMAND_RPC_VERIFY::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_export_outputs(const wallet_rpc::COMMAND_RPC_EXPORT_OUTPUTS::request& req, wallet_rpc::COMMAND_RPC_EXPORT_OUTPUTS::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_import_outputs(const wallet_rpc::COMMAND_RPC_IMPORT_OUTPUTS::request& req, wallet_rpc::COMMAND_RPC_IMPORT_OUTPUTS::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_export_key_images(const wallet_rpc::COMMAND_RPC_EXPORT_KEY_IMAGES::request& req, wallet_rpc::COMMAND_RPC_EXPORT_KEY_IMAGES::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_import_key_images(const wallet_rpc::COMMAND_RPC_IMPORT_KEY_IMAGES::request& req, wallet_rpc::COMMAND_RPC_IMPORT_KEY_IMAGES::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_make_uri(const wallet_rpc::COMMAND_RPC_MAKE_URI::request& req, wallet_rpc::COMMAND_RPC_MAKE_URI::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_parse_uri(const wallet_rpc::COMMAND_RPC_PARSE_URI::request& req, wallet_rpc::COMMAND_RPC_PARSE_URI::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_get_address_book(const wallet_rpc::COMMAND_RPC_GET_ADDRESS_BOOK_ENTRY::request& req, wallet_rpc::COMMAND_RPC_GET_ADDRESS_BOOK_ENTRY::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_add_address_book(const wallet_rpc::COMMAND_RPC_ADD_ADDRESS_BOOK_ENTRY::request& req, wallet_rpc::COMMAND_RPC_ADD_ADDRESS_BOOK_ENTRY::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_edit_address_book(const wallet_rpc::COMMAND_RPC_EDIT_ADDRESS_BOOK_ENTRY::request& req, wallet_rpc::COMMAND_RPC_EDIT_ADDRESS_BOOK_ENTRY::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_delete_address_book(const wallet_rpc::COMMAND_RPC_DELETE_ADDRESS_BOOK_ENTRY::request& req, wallet_rpc::COMMAND_RPC_DELETE_ADDRESS_BOOK_ENTRY::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_refresh(const wallet_rpc::COMMAND_RPC_REFRESH::request& req, wallet_rpc::COMMAND_RPC_REFRESH::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_rescan_spent(const wallet_rpc::COMMAND_RPC_RESCAN_SPENT::request& req, wallet_rpc::COMMAND_RPC_RESCAN_SPENT::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_start_spinning(const wallet_rpc::COMMAND_RPC_START_SPINNING::request& req, wallet_rpc::COMMAND_RPC_START_SPINNING::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_stop_spinning(const wallet_rpc::COMMAND_RPC_STOP_SPINNING::request& req, wallet_rpc::COMMAND_RPC_STOP_SPINNING::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_get_languages(const wallet_rpc::COMMAND_RPC_GET_LANGUAGES::request& req, wallet_rpc::COMMAND_RPC_GET_LANGUAGES::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_change_wallet_password(const wallet_rpc::COMMAND_RPC_CHANGE_WALLET_PASSWORD::request& req, wallet_rpc::COMMAND_RPC_CHANGE_WALLET_PASSWORD::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_is_multisig(const wallet_rpc::COMMAND_RPC_IS_MULTISIG::request& req, wallet_rpc::COMMAND_RPC_IS_MULTISIG::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_prepare_multisig(const wallet_rpc::COMMAND_RPC_PREPARE_MULTISIG::request& req, wallet_rpc::COMMAND_RPC_PREPARE_MULTISIG::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_make_multisig(const wallet_rpc::COMMAND_RPC_MAKE_MULTISIG::request& req, wallet_rpc::COMMAND_RPC_MAKE_MULTISIG::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_export_multisig(const wallet_rpc::COMMAND_RPC_EXPORT_MULTISIG::request& req, wallet_rpc::COMMAND_RPC_EXPORT_MULTISIG::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_import_multisig(const wallet_rpc::COMMAND_RPC_IMPORT_MULTISIG::request& req, wallet_rpc::COMMAND_RPC_IMPORT_MULTISIG::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_finalize_multisig(const wallet_rpc::COMMAND_RPC_FINALIZE_MULTISIG::request& req, wallet_rpc::COMMAND_RPC_FINALIZE_MULTISIG::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_exchange_multisig_keys(const wallet_rpc::COMMAND_RPC_EXCHANGE_MULTISIG_KEYS::request& req, wallet_rpc::COMMAND_RPC_EXCHANGE_MULTISIG_KEYS::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_sign_multisig(const wallet_rpc::COMMAND_RPC_SIGN_MULTISIG::request& req, wallet_rpc::COMMAND_RPC_SIGN_MULTISIG::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_submit_multisig(const wallet_rpc::COMMAND_RPC_SUBMIT_MULTISIG::request& req, wallet_rpc::COMMAND_RPC_SUBMIT_MULTISIG::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_validate_address(const wallet_rpc::COMMAND_RPC_VALIDATE_ADDRESS::request& req, wallet_rpc::COMMAND_RPC_VALIDATE_ADDRESS::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_set_daemon(const wallet_rpc::COMMAND_RPC_SET_DAEMON::request& req, wallet_rpc::COMMAND_RPC_SET_DAEMON::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_set_log_level(const wallet_rpc::COMMAND_RPC_SET_LOG_LEVEL::request& req, wallet_rpc::COMMAND_RPC_SET_LOG_LEVEL::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_set_log_categories(const wallet_rpc::COMMAND_RPC_SET_LOG_CATEGORIES::request& req, wallet_rpc::COMMAND_RPC_SET_LOG_CATEGORIES::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_estimate_tx_size_and_weight(const wallet_rpc::COMMAND_RPC_ESTIMATE_TX_SIZE_AND_WEIGHT::request& req, wallet_rpc::COMMAND_RPC_ESTIMATE_TX_SIZE_AND_WEIGHT::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);
    bool on_get_version(const wallet_rpc::COMMAND_RPC_GET_VERSION::request& req, wallet_rpc::COMMAND_RPC_GET_VERSION::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);

    //json rpc v2
    bool on_query_key(const wallet_rpc::COMMAND_RPC_QUERY_KEY::request& req, wallet_rpc::COMMAND_RPC_QUERY_KEY::response& res, epee::json_rpc::error& er, const connection_context *ctx = NULL);

    // helpers
    void fill_transfer_entry(tools::wallet_rpc::transfer_entry &entry, const crypto::hash &txid, const crypto::hash &payment_id, const tools::wallet2::payment_details &pd);
    void fill_transfer_entry(tools::wallet_rpc::transfer_entry &entry, const crypto::hash &txid, const tools::wallet2::confirmed_transfer_details &pd);
    void fill_transfer_entry(tools::wallet_rpc::transfer_entry &entry, const crypto::hash &txid, const tools::wallet2::unconfirmed_transfer_details &pd);
    void fill_transfer_entry(tools::wallet_rpc::transfer_entry &entry, const crypto::hash &payment_id, const tools::wallet2::pool_payment_details &pd);
    bool not_open(epee::json_rpc::error& er);
    void handle_rpc_exception(const std::exception_ptr& e, epee::json_rpc::error& er, int default_error_code);

    template<typename Ts, typename Tu>
    bool fill_response(std::vector<tools::wallet2::pending_tx> &ptx_vector,
        bool get_tx_key, Ts& tx_key, Tu &amount, Tu &fee, Tu &weight, std::string &multisig_txset, std::string &unsigned_txset, bool do_not_relay,
        Ts &tx_hash, bool get_tx_hex, Ts &tx_blob, bool get_tx_metadata, Ts &tx_metadata, epee::json_rpc::error &er);

    bool validate_transfer(const std::list<wallet_rpc::transfer_destination>& destinations, const std::string& payment_id, std::vector<cryptonote::tx_destination_entry>& dsts, std::vector<uint8_t>& extra, bool at_least_one_destination, epee::json_rpc::error& er);

    void check_background_spinning();
  };
}
