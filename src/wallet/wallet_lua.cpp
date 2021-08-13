#include "lgyrowallet.h"
#include "wallet_rpc_server_commands_defs.h"

#define CONCATENATE(arg1, arg2)   CONCATENATE1(arg1, arg2)
#define CONCATENATE1(arg1, arg2)  CONCATENATE2(arg1, arg2)
#define CONCATENATE2(arg1, arg2)  arg1##arg2

#define FOR_EACH_1(T, what, x) what(T, x)
#define FOR_EACH_2(T, what, x, ...)\
  what(T, x)\
  FOR_EACH_1(T, what,  __VA_ARGS__)
#define FOR_EACH_3(T, what, x, ...)\
  what(T, x)\
  FOR_EACH_2(T, what, __VA_ARGS__)
#define FOR_EACH_4(T, what, x, ...)\
  what(T, x)\
  FOR_EACH_3(T, what,  __VA_ARGS__)
#define FOR_EACH_5(T, what, x, ...)\
  what(T, x)\
  FOR_EACH_4(T, what,  __VA_ARGS__)
#define FOR_EACH_6(T, what, x, ...)\
  what(T, x)\
  FOR_EACH_5(T, what,  __VA_ARGS__)
#define FOR_EACH_7(T, what, x, ...)\
  what(T, x)\
  FOR_EACH_6(T, what,  __VA_ARGS__)
#define FOR_EACH_8(T, what, x, ...)\
  what(T, x)\
  FOR_EACH_7(T, what,  __VA_ARGS__)
#define FOR_EACH_9(T, what, x, ...)\
  what(T, x)\
  FOR_EACH_8(T, what,  __VA_ARGS__)
#define FOR_EACH_10(T, what, x, ...)\
  what(T, x)\
  FOR_EACH_9(T, what, __VA_ARGS__)
#define FOR_EACH_11(T, what, x, ...)\
  what(T, x)\
  FOR_EACH_10(T, what,  __VA_ARGS__)
#define FOR_EACH_12(T, what, x, ...)\
  what(T, x)\
  FOR_EACH_11(T, what,  __VA_ARGS__)
#define FOR_EACH_13(T, what, x, ...)\
  what(T, x)\
  FOR_EACH_12(T, what,  __VA_ARGS__)
#define FOR_EACH_14(T, what, x, ...)\
  what(T, x)\
  FOR_EACH_13(T, what,  __VA_ARGS__)
#define FOR_EACH_15(T, what, x, ...)\
  what(T, x)\
  FOR_EACH_14(T, what,  __VA_ARGS__)
#define FOR_EACH_16(T, what, x, ...)\
  what(T, x)\
  FOR_EACH_15(T, what,  __VA_ARGS__)
#define FOR_EACH_17(T, what, x, ...)\
  what(T, x)\
  FOR_EACH_16(T, what,  __VA_ARGS__)
#define FOR_EACH_18(T, what, x, ...)\
  what(T, x)\
  FOR_EACH_17(T, what, __VA_ARGS__)
#define FOR_EACH_19(T, what, x, ...)\
  what(T, x)\
  FOR_EACH_18(T, what,  __VA_ARGS__)
#define FOR_EACH_20(T, what, x, ...)\
  what(T, x)\
  FOR_EACH_19(T, what,  __VA_ARGS__)
#define FOR_EACH_21(T, what, x, ...)\
  what(T, x)\
  FOR_EACH_20(T, what,  __VA_ARGS__)
#define FOR_EACH_22(T, what, x, ...)\
  what(T, x)\
  FOR_EACH_21(T, what,  __VA_ARGS__)
#define FOR_EACH_23(T, what, x, ...)\
  what(T, x)\
  FOR_EACH_22(T, what,  __VA_ARGS__)
#define FOR_EACH_24(T, what, x, ...)\
  what(T, x)\
  FOR_EACH_23(T, what,  __VA_ARGS__)
#define FOR_EACH_25(T, what, x, ...)\
  what(T, x)\
  FOR_EACH_24(T, what,  __VA_ARGS__)

#define FOR_EACH_NARG(...) FOR_EACH_NARG_(__VA_ARGS__, FOR_EACH_RSEQ_N())
#define FOR_EACH_NARG_(...) FOR_EACH_ARG_N(__VA_ARGS__)
#define FOR_EACH_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, N, ...) N
#define FOR_EACH_RSEQ_N() 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define FOR_EACH_(T, N, what, ...) CONCATENATE(FOR_EACH_, N)(T, what, __VA_ARGS__)
#define FOR_EACH(T, what, ...) FOR_EACH_(T, FOR_EACH_NARG(__VA_ARGS__), what, __VA_ARGS__)

#define MEMBER(T,x) , #x, &tools::wallet_rpc::COMMAND_RPC_##T::x
#define MEMBER2(T,x) , #x, &tools::wallet_rpc::T::x

#define NEW_USERTYPE(T, ...) \
    lua.new_usertype<tools::wallet_rpc::COMMAND_RPC_##T>( \
      "GYRO_" #T \
      FOR_EACH(T, MEMBER, __VA_ARGS__) \
    );
#define NEW_USERTYPE2(T, ...) \
    lua.new_usertype<tools::wallet_rpc::T>( \
      "gyro_" #T \
      FOR_EACH(T, MEMBER2, __VA_ARGS__) \
    );

namespace lwallet
{

  bool reg(sol::state_view& lua)
  {
    NEW_USERTYPE(GET_BALANCE::request,account_index,address_indices,all_accounts,strict)
    NEW_USERTYPE(GET_BALANCE::per_subaddress_info,account_index,address_index,address,balance,unlocked_balance,label,num_unspent_outputs,blocks_to_unlock,time_to_unlock)
    NEW_USERTYPE(GET_BALANCE::txid_spinner_lock,txid,amount,height)
    NEW_USERTYPE(GET_BALANCE::response,balance,unlocked_balance,multisig_import_needed,per_subaddress,blocks_to_unlock,time_to_unlock,txid_locks)
    NEW_USERTYPE(GET_ADDRESS_INDEX::request_t,address)
    NEW_USERTYPE(GET_ADDRESS_INDEX::response_t,index)
    NEW_USERTYPE(CREATE_ADDRESS::request_t,account_index,count,label)
    NEW_USERTYPE(CREATE_ADDRESS::response_t,address, address_index, addresses, address_indices)
    NEW_USERTYPE(LABEL_ADDRESS::request_t,index, label)
    //NEW_USERTYPE(LABEL_ADDRESS::response_t)
    NEW_USERTYPE(GET_ACCOUNTS::request_t,tag, strict_balances)
    NEW_USERTYPE(GET_ACCOUNTS::subaddress_account_info,account_index, base_address, balance, unlocked_balance, label, tag)
    NEW_USERTYPE(GET_ACCOUNTS::response_t,total_balance, total_unlocked_balance, subaddress_accounts)
    NEW_USERTYPE(CREATE_ACCOUNT::request_t,label)
    NEW_USERTYPE(CREATE_ACCOUNT::response_t,account_index,address)
    NEW_USERTYPE(LABEL_ACCOUNT::request_t,account_index,label)
    //NEW_USERTYPE(LABEL_ACCOUNT::response_t)
    //NEW_USERTYPE(GET_ACCOUNT_TAGS::request_t)
    NEW_USERTYPE(GET_ACCOUNT_TAGS::account_tag_info,tag,label,accounts)
    NEW_USERTYPE(GET_ACCOUNT_TAGS::response_t,account_tags)
    NEW_USERTYPE(TAG_ACCOUNTS::request_t,tag,accounts)
    //NEW_USERTYPE(TAG_ACCOUNTS::response_t)
    NEW_USERTYPE(UNTAG_ACCOUNTS::request_t,accounts)
    //NEW_USERTYPE(UNTAG_ACCOUNTS::::response_t)
    NEW_USERTYPE(SET_ACCOUNT_TAG_DESCRIPTION::request_t,tag,description)
    //NEW_USERTYPE(SET_ACCOUNT_TAG_DESCRIPTION::::response_t)
    //NEW_USERTYPE(GET_HEIGHT::request_t)
    NEW_USERTYPE(GET_HEIGHT::response_t,height)
    NEW_USERTYPE2(transfer_destination,amount,address)
    NEW_USERTYPE(TRANSFER::request_t,destinations,account_index,subaddr_indices,priority,ring_size,unlock_time,payment_id,get_tx_key,do_not_relay,get_tx_hex,get_tx_metadata)
    NEW_USERTYPE(TRANSFER::response_t,tx_hash,tx_key,amount,fee,weight,tx_blob,tx_metadata,multisig_txset,unsigned_txset)
    NEW_USERTYPE(TRANSFER_SPLIT::request_t,destinations,account_index,subaddr_indices,priority,ring_size,unlock_time,payment_id,get_tx_keys,do_not_relay,get_tx_hex,get_tx_metadata)
    NEW_USERTYPE(TRANSFER_SPLIT::key_list,keys)
    NEW_USERTYPE(TRANSFER_SPLIT::response_t,tx_hash_list,tx_key_list,amount_list,fee_list,weight_list,tx_blob_list,tx_metadata_list,multisig_txset,unsigned_txset)
    NEW_USERTYPE(DESCRIBE_TRANSFER::recipient,address,amount)
    NEW_USERTYPE(DESCRIBE_TRANSFER::transfer_description,amount_in,amount_out,ring_size,unlock_time,recipients,payment_id,change_amount,change_address,fee,dummy_outputs,extra)
    NEW_USERTYPE(DESCRIBE_TRANSFER::request_t,unsigned_txset,multisig_txset)
    NEW_USERTYPE(DESCRIBE_TRANSFER::response_t,desc)
    NEW_USERTYPE(SIGN_TRANSFER::request_t,unsigned_txset,export_raw,get_tx_keys)
    NEW_USERTYPE(SIGN_TRANSFER::response_t,signed_txset,tx_hash_list,tx_raw_list,tx_key_list)
    NEW_USERTYPE(SUBMIT_TRANSFER::request_t,tx_data_hex)
    NEW_USERTYPE(SUBMIT_TRANSFER::response_t,tx_hash_list)
    NEW_USERTYPE(SWEEP_DUST::request_t,get_tx_keys,do_not_relay,get_tx_hex,get_tx_metadata)
    NEW_USERTYPE(SWEEP_DUST::key_list,keys)
    NEW_USERTYPE(SWEEP_DUST::response_t,tx_hash_list,tx_key_list,amount_list,fee_list,weight_list,tx_blob_list,tx_metadata_list,multisig_txset,unsigned_txset)
    NEW_USERTYPE(SWEEP_ALL::request_t,address,account_index,subaddr_indices,subaddr_indices_all,priority,ring_size,outputs,unlock_time,payment_id,get_tx_keys,below_amount,do_not_relay,get_tx_hex,get_tx_metadata)
    NEW_USERTYPE(SWEEP_ALL::key_list,keys)
    NEW_USERTYPE(SWEEP_ALL::response_t,tx_hash_list,tx_key_list,amount_list,fee_list,weight_list,tx_blob_list,tx_metadata_list,multisig_txset,unsigned_txset)
    NEW_USERTYPE(SWEEP_SINGLE::request_t,address,priority,ring_size,outputs,unlock_time,payment_id,get_tx_key,key_image,do_not_relay,get_tx_hex,get_tx_metadata)
    NEW_USERTYPE(SWEEP_SINGLE::response_t,tx_hash,tx_key,amount,fee,weight,tx_blob,tx_metadata,multisig_txset,unsigned_txset)
    NEW_USERTYPE(RELAY_TX::request_t,hex)
    NEW_USERTYPE(RELAY_TX::response_t,tx_hash)
    //NEW_USERTYPE(STORE::request_t)
    //NEW_USERTYPE(STORE::response_t)
    NEW_USERTYPE2(payment_details,payment_id,tx_hash,amount,block_height,unlock_time,locked,subaddr_index,address)
    NEW_USERTYPE(GET_PAYMENTS::request_t,payment_id)
    NEW_USERTYPE(GET_PAYMENTS::response_t,payments)
    NEW_USERTYPE(GET_BULK_PAYMENTS::request_t,payment_ids,min_block_height)
    NEW_USERTYPE(GET_PAYMENTS::response_t,payments)
    NEW_USERTYPE2(transfer_details,amount,spent,global_index,tx_hash,subaddr_index,key_image,block_height,frozen,unlocked)
    NEW_USERTYPE(INCOMING_TRANSFERS::request_t,transfer_type,account_index,subaddr_indices)
    NEW_USERTYPE(INCOMING_TRANSFERS::response_t,transfers)
    NEW_USERTYPE(QUERY_KEY::request_t,key_type)
    NEW_USERTYPE(QUERY_KEY::response_t,key)
    NEW_USERTYPE(MAKE_INTEGRATED_ADDRESS::request_t,standard_address,payment_id)
    NEW_USERTYPE(MAKE_INTEGRATED_ADDRESS::response_t,integrated_address,payment_id)
    NEW_USERTYPE(SPLIT_INTEGRATED_ADDRESS::request_t,integrated_address)
    NEW_USERTYPE(SPLIT_INTEGRATED_ADDRESS::response_t,standard_address,payment_id,is_subaddress)
    //NEW_USERTYPE(STOP_WALLET::request_t)
    //NEW_USERTYPE(STOP_WALLET::response_t)
    NEW_USERTYPE(RESCAN_BLOCKCHAIN::request_t,hard)
    //NEW_USERTYPE(RESCAN_BLOCKCHAIN::response_t)
    NEW_USERTYPE(SET_TX_NOTES::request_t,txids,notes)
    //NEW_USERTYPE(SET_TX_NOTES::response_t)
    NEW_USERTYPE(GET_TX_NOTES::request_t,txids)
    NEW_USERTYPE(GET_TX_NOTES::response_t,notes)
    NEW_USERTYPE(SET_ATTRIBUTE::request_t,key,value)
    //NEW_USERTYPE(SET_ATTRIBUTE::response_t)
    NEW_USERTYPE(GET_ATTRIBUTE::request_t,key)
    NEW_USERTYPE(GET_ATTRIBUTE::response_t,value)
    NEW_USERTYPE(GET_TX_KEY::request_t,txid)
    NEW_USERTYPE(GET_TX_KEY::response_t,tx_key)
    NEW_USERTYPE(CHECK_TX_KEY::request_t,txid,tx_key,address)
    NEW_USERTYPE(CHECK_TX_KEY::response_t,received,in_pool,confirmations)
    NEW_USERTYPE(GET_TX_PROOF::request_t,txid,address,message)
    NEW_USERTYPE(GET_TX_PROOF::response_t,signature)
    NEW_USERTYPE(CHECK_TX_PROOF::request_t,txid,address,message,signature)
    NEW_USERTYPE(CHECK_TX_PROOF::response_t,good,received,in_pool,confirmations)
    NEW_USERTYPE2(transfer_entry,txid,payment_id,height,timestamp,amount,amounts,fee,note,destinations,type,unlock_time,locked,subaddr_index,subaddr_indices,address,double_spend_seen,confirmations,suggested_confirmations_threshold)
    NEW_USERTYPE(GET_SPEND_PROOF::request_t,txid,message)
    NEW_USERTYPE(GET_SPEND_PROOF::response_t,signature)
    NEW_USERTYPE(CHECK_SPEND_PROOF::request_t,txid,message,signature)
    NEW_USERTYPE(CHECK_SPEND_PROOF::response_t,good)
    NEW_USERTYPE(GET_RESERVE_PROOF::request_t,all,account_index,amount,message)
    NEW_USERTYPE(GET_RESERVE_PROOF::response_t,signature)
    NEW_USERTYPE(CHECK_RESERVE_PROOF::request_t,address,message,signature)
    NEW_USERTYPE(CHECK_RESERVE_PROOF::response_t,good,total,spent)
    NEW_USERTYPE(GET_TRANSFERS::request_t,in,out,pending,failed,pool,filter_by_height,min_height,max_height,account_index,subaddr_indices,all_accounts)
    NEW_USERTYPE(GET_TRANSFERS::response_t,in,out,pending,failed,pool)
    NEW_USERTYPE(GET_TRANSFER_BY_TXID::request_t,txid,account_index)
    NEW_USERTYPE(GET_TRANSFER_BY_TXID::response_t,transfer,transfers)
    NEW_USERTYPE(SIGN::request_t,data,account_index,address_index)
    NEW_USERTYPE(SIGN::response_t,signature)
    NEW_USERTYPE(VERIFY::request_t,data,address,signature)
    NEW_USERTYPE(VERIFY::response_t,good)
    NEW_USERTYPE(EXPORT_OUTPUTS::request_t,all)
    NEW_USERTYPE(EXPORT_OUTPUTS::response_t,outputs_data_hex)
    NEW_USERTYPE(IMPORT_OUTPUTS::request_t,outputs_data_hex)
    NEW_USERTYPE(IMPORT_OUTPUTS::response_t,num_imported)
    NEW_USERTYPE(EXPORT_KEY_IMAGES::request_t,all)
    NEW_USERTYPE(EXPORT_KEY_IMAGES::signed_key_image,key_image,signature)
    NEW_USERTYPE(EXPORT_KEY_IMAGES::response_t,offset,signed_key_images)
    NEW_USERTYPE(IMPORT_KEY_IMAGES::signed_key_image,key_image,signature)
    NEW_USERTYPE(IMPORT_KEY_IMAGES::request_t,offset,signed_key_images)
    NEW_USERTYPE(IMPORT_KEY_IMAGES::response_t,height,spent,unspent)
    NEW_USERTYPE2(uri_spec,address,payment_id,amount,tx_description,recipient_name)
    NEW_USERTYPE(MAKE_URI::request_t,address,payment_id,amount,tx_description,recipient_name)
    NEW_USERTYPE(MAKE_URI::response_t,uri)
    NEW_USERTYPE(PARSE_URI::request_t,uri)
    NEW_USERTYPE(PARSE_URI::response_t,uri,unknown_parameters)
    NEW_USERTYPE(ADD_ADDRESS_BOOK_ENTRY::request_t,address,description,label,viewsec)
    NEW_USERTYPE(ADD_ADDRESS_BOOK_ENTRY::response_t,index)
    NEW_USERTYPE(EDIT_ADDRESS_BOOK_ENTRY::request_t,index,set_address,address,set_description,description,set_label,label,set_viewsec,viewsec)
    //NEW_USERTYPE(EDIT_ADDRESS_BOOK_ENTRY::response_t)
    NEW_USERTYPE(GET_ADDRESS_BOOK_ENTRY::request_t,entries)
    NEW_USERTYPE(GET_ADDRESS_BOOK_ENTRY::entry,index,address,description,label,viewsec)
    NEW_USERTYPE(GET_ADDRESS_BOOK_ENTRY::response_t,entries)
    NEW_USERTYPE(DELETE_ADDRESS_BOOK_ENTRY::request_t,index)
    //NEW_USERTYPE(DELETE_ADDRESS_BOOK_ENTRY::response_t)
    //NEW_USERTYPE(RESCAN_SPENT::request_t)
    //NEW_USERTYPE(RESCAN_SPENT::response_t)
    NEW_USERTYPE(REFRESH::request_t,start_height)
    NEW_USERTYPE(REFRESH::response_t,blocks_fetched,received_money)
    NEW_USERTYPE(AUTO_REFRESH::request_t,enable,period)
    //NEW_USERTYPE(AUTO_REFRESH::response_t)
    NEW_USERTYPE(START_SPINNING::request_t,reward_address,txid,threads_count,do_background_spinning,ignore_battery)        
    //NEW_USERTYPE(START_SPINNING::response_t)
    //NEW_USERTYPE(STOP_SPINNING::request_t)
    //NEW_USERTYPE(STOP_SPINNING::response_t)
    //NEW_USERTYPE(GET_LANGUAGES::request_t)
    NEW_USERTYPE(GET_LANGUAGES::response_t,languages,languages_local)
    NEW_USERTYPE(CREATE_WALLET::request_t,filename,password,language)
    //NEW_USERTYPE(CREATE_WALLET::response_t)
    NEW_USERTYPE(OPEN_WALLET::request_t,filename,password,autosave_current)
    //NEW_USERTYPE(OPEN_WALLET::response_t)
    NEW_USERTYPE(CLOSE_WALLET::request_t,autosave_current)
    //NEW_USERTYPE(CLOSE_WALLET::response_t)
    NEW_USERTYPE(CHANGE_WALLET_PASSWORD::request_t,old_password,new_password)
    //NEW_USERTYPE(CHANGE_WALLET_PASSWORD::response_t)
    NEW_USERTYPE(GENERATE_FROM_KEYS::request,restore_height,filename,address,spendkey,viewkey,password,autosave_current)
    NEW_USERTYPE(GENERATE_FROM_KEYS::response,address,info)
    NEW_USERTYPE(RESTORE_DETERMINISTIC_WALLET::request_t,restore_height,filename,seed,seed_offset,password,language,autosave_current)
    NEW_USERTYPE(RESTORE_DETERMINISTIC_WALLET::response_t,address,seed,info,was_deprecated)
    //NEW_USERTYPE(IS_MULTISIG::request_t)
    NEW_USERTYPE(IS_MULTISIG::response_t,multisig,ready,threshold,total)
    //NEW_USERTYPE(PREPARE_MULTISIG::request_t)
    NEW_USERTYPE(PREPARE_MULTISIG::response_t,multisig_info)
    NEW_USERTYPE(MAKE_MULTISIG::request_t,multisig_info,threshold,password)
    NEW_USERTYPE(MAKE_MULTISIG::response_t,address,multisig_info)
    //NEW_USERTYPE(EXPORT_MULTISIG::request_t)
    NEW_USERTYPE(EXPORT_MULTISIG::response_t,info)
    NEW_USERTYPE(IMPORT_MULTISIG::request_t,info)
    NEW_USERTYPE(IMPORT_MULTISIG::response_t,n_outputs)
    NEW_USERTYPE(FINALIZE_MULTISIG::request_t,password,multisig_info)
    NEW_USERTYPE(FINALIZE_MULTISIG::response_t,address)
    NEW_USERTYPE(EXCHANGE_MULTISIG_KEYS::request_t,password,multisig_info)
    NEW_USERTYPE(EXCHANGE_MULTISIG_KEYS::response_t,address,multisig_info)
    NEW_USERTYPE(SIGN_MULTISIG::request_t,tx_data_hex)
    NEW_USERTYPE(SIGN_MULTISIG::response_t,tx_data_hex,tx_hash_list)
    NEW_USERTYPE(SUBMIT_MULTISIG::request_t,tx_data_hex)
    NEW_USERTYPE(SUBMIT_MULTISIG::response_t,tx_hash_list)
    //NEW_USERTYPE(GET_VERSION::request_t)
    NEW_USERTYPE(GET_VERSION::response_t,version,release)
    NEW_USERTYPE(VALIDATE_ADDRESS::request_t,address,any_net_type,allow_openalias)
    NEW_USERTYPE(VALIDATE_ADDRESS::response_t,valid,integrated,subaddress,nettype,openalias_address)
    NEW_USERTYPE(SET_DAEMON::request_t,address,trusted,ssl_support,ssl_private_key_path,ssl_certificate_path,ssl_ca_file,ssl_allowed_fingerprints,ssl_allow_any_cert)
    //NEW_USERTYPE(SET_DAEMON::response_t)
    NEW_USERTYPE(SET_LOG_LEVEL::request_t,level)
    //NEW_USERTYPE(SET_LOG_LEVEL::response_t)
    NEW_USERTYPE(SET_LOG_CATEGORIES::request_t,categories)
    NEW_USERTYPE(SET_LOG_CATEGORIES::response_t,categories)
    NEW_USERTYPE(ESTIMATE_TX_SIZE_AND_WEIGHT::request_t,n_inputs,n_outputs,ring_size,rct)
    NEW_USERTYPE(ESTIMATE_TX_SIZE_AND_WEIGHT::response_t,size,weight)
    
    return true;
  }

} /* lwallet */
