#pragma once
#include "wallet_rpc_server_api.h"
#include "zyre/zyre.hpp"

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "wallet.zyre"

#define ON_RPC_PROXY_METHOD_DECL(f_name, st_name) \
  void on_##f_name(const tools::wallet_rpc::COMMAND_RPC_##st_name::request& req, zyre::resp_t r);

namespace zyre { namespace wallet {

  class rpc_proxy
  {
    tools::wallet_rpc_server_api rpc_;
  public:
    rpc_proxy(tools::wallet2 *w);
    void init(zyre::client& zyre);

    ON_RPC_PROXY_METHOD_DECL( getbalance                   , GET_BALANCE                      )
    ON_RPC_PROXY_METHOD_DECL( getaddress                   , GET_ADDRESS                      )
    ON_RPC_PROXY_METHOD_DECL( getaddress_index             , GET_ADDRESS_INDEX                )
    ON_RPC_PROXY_METHOD_DECL( create_address               , CREATE_ADDRESS                   )
    ON_RPC_PROXY_METHOD_DECL( label_address                , LABEL_ADDRESS                    )
    ON_RPC_PROXY_METHOD_DECL( get_accounts                 , GET_ACCOUNTS                     )
    ON_RPC_PROXY_METHOD_DECL( create_account               , CREATE_ACCOUNT                   )
    ON_RPC_PROXY_METHOD_DECL( label_account                , LABEL_ACCOUNT                    )
    ON_RPC_PROXY_METHOD_DECL( get_account_tags             , GET_ACCOUNT_TAGS                 )
    ON_RPC_PROXY_METHOD_DECL( tag_accounts                 , TAG_ACCOUNTS                     )
    ON_RPC_PROXY_METHOD_DECL( untag_accounts               , UNTAG_ACCOUNTS                   )
    ON_RPC_PROXY_METHOD_DECL( set_account_tag_description  , SET_ACCOUNT_TAG_DESCRIPTION      )
    ON_RPC_PROXY_METHOD_DECL( getheight                    , GET_HEIGHT                       )
    ON_RPC_PROXY_METHOD_DECL( transfer                     , TRANSFER                         )
    ON_RPC_PROXY_METHOD_DECL( transfer_split               , TRANSFER_SPLIT                   )
    ON_RPC_PROXY_METHOD_DECL( sign_transfer                , SIGN_TRANSFER                    )
    ON_RPC_PROXY_METHOD_DECL( describe_transfer            , DESCRIBE_TRANSFER                )
    ON_RPC_PROXY_METHOD_DECL( submit_transfer              , SUBMIT_TRANSFER                  )
    ON_RPC_PROXY_METHOD_DECL( sweep_dust                   , SWEEP_DUST                       )
    ON_RPC_PROXY_METHOD_DECL( sweep_all                    , SWEEP_ALL                        )
    ON_RPC_PROXY_METHOD_DECL( sweep_single                 , SWEEP_SINGLE                     )
    ON_RPC_PROXY_METHOD_DECL( relay_tx                     , RELAY_TX                         )
    ON_RPC_PROXY_METHOD_DECL( make_integrated_address      , MAKE_INTEGRATED_ADDRESS          )
    ON_RPC_PROXY_METHOD_DECL( split_integrated_address     , SPLIT_INTEGRATED_ADDRESS         )
    ON_RPC_PROXY_METHOD_DECL( store                        , STORE                            )
    ON_RPC_PROXY_METHOD_DECL( get_payments                 , GET_PAYMENTS                     )
    ON_RPC_PROXY_METHOD_DECL( get_bulk_payments            , GET_BULK_PAYMENTS                )
    ON_RPC_PROXY_METHOD_DECL( incoming_transfers           , INCOMING_TRANSFERS               )
    ON_RPC_PROXY_METHOD_DECL( rescan_blockchain            , RESCAN_BLOCKCHAIN                )
    ON_RPC_PROXY_METHOD_DECL( set_tx_notes                 , SET_TX_NOTES                     )
    ON_RPC_PROXY_METHOD_DECL( get_tx_notes                 , GET_TX_NOTES                     )
    ON_RPC_PROXY_METHOD_DECL( set_attribute                , SET_ATTRIBUTE                    )
    ON_RPC_PROXY_METHOD_DECL( get_attribute                , GET_ATTRIBUTE                    )
    ON_RPC_PROXY_METHOD_DECL( get_tx_key                   , GET_TX_KEY                       )
    ON_RPC_PROXY_METHOD_DECL( check_tx_key                 , CHECK_TX_KEY                     )
    ON_RPC_PROXY_METHOD_DECL( get_tx_proof                 , GET_TX_PROOF                     )
    ON_RPC_PROXY_METHOD_DECL( check_tx_proof               , CHECK_TX_PROOF                   )
    ON_RPC_PROXY_METHOD_DECL( get_spend_proof              , GET_SPEND_PROOF                  )
    ON_RPC_PROXY_METHOD_DECL( check_spend_proof            , CHECK_SPEND_PROOF                )
    ON_RPC_PROXY_METHOD_DECL( get_reserve_proof            , GET_RESERVE_PROOF                )
    ON_RPC_PROXY_METHOD_DECL( check_reserve_proof          , CHECK_RESERVE_PROOF              )
    ON_RPC_PROXY_METHOD_DECL( get_transfers                , GET_TRANSFERS                    )
    ON_RPC_PROXY_METHOD_DECL( get_transfer_by_txid         , GET_TRANSFER_BY_TXID             )
    ON_RPC_PROXY_METHOD_DECL( sign                         , SIGN                             )
    ON_RPC_PROXY_METHOD_DECL( verify                       , VERIFY                           )
    ON_RPC_PROXY_METHOD_DECL( export_outputs               , EXPORT_OUTPUTS                   )
    ON_RPC_PROXY_METHOD_DECL( import_outputs               , IMPORT_OUTPUTS                   )
    ON_RPC_PROXY_METHOD_DECL( export_key_images            , EXPORT_KEY_IMAGES                )
    ON_RPC_PROXY_METHOD_DECL( import_key_images            , IMPORT_KEY_IMAGES                )
    ON_RPC_PROXY_METHOD_DECL( make_uri                     , MAKE_URI                         )
    ON_RPC_PROXY_METHOD_DECL( parse_uri                    , PARSE_URI                        )
    ON_RPC_PROXY_METHOD_DECL( get_address_book             , GET_ADDRESS_BOOK_ENTRY           )
    ON_RPC_PROXY_METHOD_DECL( add_address_book             , ADD_ADDRESS_BOOK_ENTRY           )
    ON_RPC_PROXY_METHOD_DECL( edit_address_book            , EDIT_ADDRESS_BOOK_ENTRY          )
    ON_RPC_PROXY_METHOD_DECL( delete_address_book          , DELETE_ADDRESS_BOOK_ENTRY        )
    ON_RPC_PROXY_METHOD_DECL( refresh                      , REFRESH                          )
    ON_RPC_PROXY_METHOD_DECL( rescan_spent                 , RESCAN_SPENT                     )
    ON_RPC_PROXY_METHOD_DECL( start_spinning               , START_SPINNING                   )
    ON_RPC_PROXY_METHOD_DECL( stop_spinning                , STOP_SPINNING                    )
    ON_RPC_PROXY_METHOD_DECL( get_languages                , GET_LANGUAGES                    )
    ON_RPC_PROXY_METHOD_DECL( change_wallet_password       , CHANGE_WALLET_PASSWORD           )
    ON_RPC_PROXY_METHOD_DECL( is_multisig                  , IS_MULTISIG                      )
    ON_RPC_PROXY_METHOD_DECL( prepare_multisig             , PREPARE_MULTISIG                 )
    ON_RPC_PROXY_METHOD_DECL( make_multisig                , MAKE_MULTISIG                    )
    ON_RPC_PROXY_METHOD_DECL( export_multisig              , EXPORT_MULTISIG                  )
    ON_RPC_PROXY_METHOD_DECL( import_multisig              , IMPORT_MULTISIG                  )
    ON_RPC_PROXY_METHOD_DECL( finalize_multisig            , FINALIZE_MULTISIG                )
    ON_RPC_PROXY_METHOD_DECL( exchange_multisig_keys       , EXCHANGE_MULTISIG_KEYS           )
    ON_RPC_PROXY_METHOD_DECL( sign_multisig                , SIGN_MULTISIG                    )
    ON_RPC_PROXY_METHOD_DECL( submit_multisig              , SUBMIT_MULTISIG                  )
    ON_RPC_PROXY_METHOD_DECL( validate_address             , VALIDATE_ADDRESS                 )
    ON_RPC_PROXY_METHOD_DECL( set_daemon                   , SET_DAEMON                       )
    ON_RPC_PROXY_METHOD_DECL( set_log_level                , SET_LOG_LEVEL                    )
    ON_RPC_PROXY_METHOD_DECL( set_log_categories           , SET_LOG_CATEGORIES               )
    ON_RPC_PROXY_METHOD_DECL( estimate_tx_size_and_weight  , ESTIMATE_TX_SIZE_AND_WEIGHT      )
    ON_RPC_PROXY_METHOD_DECL( get_version                  , GET_VERSION                      )

    void start();
    void stop();
  };

} /* wallet */
} /* zyre */
