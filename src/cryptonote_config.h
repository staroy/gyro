// Copyright (c) 2014-2019, The Monero Project
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// Parts of this file are originally copyright (c) 2012-2013 The Cryptonote developers

#pragma once

#include <stdexcept>
#include <string>
#include <boost/uuid/uuid.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include "crypto/hash.h"

#define CRYPTONOTE_DNS_TIMEOUT_MS                       20000

#define CRYPTONOTE_MAX_BLOCK_NUMBER                     500000000
#define CRYPTONOTE_GETBLOCKTEMPLATE_MAX_BLOCK_SIZE      196608 //size of block (bytes) that is the maximum that spinners will produce
#define CRYPTONOTE_MAX_TX_SIZE                          1000000
#define CRYPTONOTE_MAX_TX_PER_BLOCK                     0x10000000
#define CRYPTONOTE_PUBLIC_ADDRESS_TEXTBLOB_VER          0
#define CRYPTONOTE_SPINNED_MONEY_UNLOCK_WINDOW          60
#define CRYPTONOTE_SPINNED_MONEY_LOCKED_BLOCKS          CRYPTONOTE_MAX_BLOCK_NUMBER
#define CURRENT_TRANSACTION_VERSION                     2
#define CURRENT_BLOCK_MAJOR_VERSION                     1
#define CURRENT_BLOCK_MINOR_VERSION                     0
#define CRYPTONOTE_BLOCK_FUTURE_TIME_LIMIT              60*60*2
#define CRYPTONOTE_DEFAULT_TX_SPENDABLE_AGE             10

#define BLOCKCHAIN_TIMESTAMP_CHECK_WINDOW               60

// MONEY_SUPPLY - total number coins to be generated
#define MONEY_SUPPLY                                    ((uint64_t)(-1))
extern uint64_t EMISSION_SUBSIDY_PER_MINUTE;            //(18)
extern uint64_t FINAL_SUBSIDY_PER_MINUTE;               //((uint64_t)3000000000) // 3 * pow(10, 9)

#define CRYPTONOTE_REWARD_BLOCKS_WINDOW                 100
#define CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V2    60000 //size of block (bytes) after which reward for block calculated using block size
#define CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V1    20000 //size of block (bytes) after which reward for block calculated using block size - before first fork
#define CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V5    300000 //size of block (bytes) after which reward for block calculated using block size - second change, from v5
#define CRYPTONOTE_LONG_TERM_BLOCK_WEIGHT_WINDOW_SIZE   100000 // size in blocks of the long term block weight median window
#define CRYPTONOTE_SHORT_TERM_BLOCK_WEIGHT_SURGE_FACTOR 50
#define CRYPTONOTE_COINBASE_BLOB_RESERVED_SIZE          600

extern uint32_t CRYPTONOTE_DISPLAY_DECIMAL_POINT; //    12
// COIN - number of smallest units in one coin
#define COIN_DEFAULT                                    ((uint64_t)1000000000000) // pow(10, 12)
extern uint64_t COIN; //                                ((uint64_t)1000000000000) // pow(10, 12)

#define FEE_PER_KB_OLD                                  ((uint64_t)10000000000) // pow(10, 10)
#define FEE_PER_KB                                      ((uint64_t)2000000000) // 2 * pow(10, 9)
#define FEE_PER_BYTE                                    ((uint64_t)300000)
#define DYNAMIC_FEE_PER_KB_BASE_FEE                     ((uint64_t)2000000000) // 2 * pow(10,9)
#define DYNAMIC_FEE_PER_KB_BASE_BLOCK_REWARD            ((uint64_t)10000000000000) // 10 * pow(10,12)
#define DYNAMIC_FEE_PER_KB_BASE_FEE_V5                  ((uint64_t)2000000000 * (uint64_t)CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V2 / CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V5)
#define DYNAMIC_FEE_REFERENCE_TRANSACTION_WEIGHT         ((uint64_t)3000)

#define ORPHANED_BLOCKS_MAX_COUNT                       100


extern uint64_t DURATION_TARGET;                        // 120  // seconds
extern uint64_t DURATION_TARGET_LAG;                    // 5  // seconds

#define GYRO_WINDOW                                     720 // blocks
#define GYRO_LAG                                        15  // !!!
#define GYRO_CUT                                        60  // timestamps to cut after sorting
#define GYRO_BLOCKS_COUNT                               GYRO_WINDOW + GYRO_LAG


#define CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_SECONDS      DURATION_TARGET * CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_BLOCKS
#define CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_BLOCKS       1


#define GYRO_BLOCKS_ESTIMATE_TIMESPAN                   DURATION_TARGET //just alias; used by tests
#define GYRO_NUMBER_OF_DIMENSIONALS                     256


#define BLOCKS_IDS_SYNCHRONIZING_DEFAULT_COUNT          10000  //by default, blocks ids count in synchronizing
#define BLOCKS_SYNCHRONIZING_DEFAULT_COUNT_PRE_V4       100    //by default, blocks count in blocks downloading
#define BLOCKS_SYNCHRONIZING_DEFAULT_COUNT              20     //by default, blocks count in blocks downloading
#define BLOCKS_SYNCHRONIZING_MAX_COUNT                  2048   //must be a power of 2, greater than 128, equal to SEEDHASH_EPOCH_BLOCKS

extern uint64_t CRYPTONOTE_MEMPOOL_TX_LIVETIME;                // (86400*3) //seconds, three days
extern uint64_t CRYPTONOTE_MEMPOOL_TX_FROM_ALT_BLOCK_LIVETIME; // 604800 //seconds, one week
extern uint64_t CRYPTONOTE_MESSAGE_TX_LIVETIME;                // 86400 //seconds, three days
extern uint64_t CRYPTONOTE_MESSAGE_TX_AMOUNT;                  // COIN
extern uint64_t TX_EXTRA_SMS_MAX_COUNT;      // 4096

#define CRYPTONOTE_DANDELIONPP_STEMS              2 // number of outgoing stem connections per epoch
#define CRYPTONOTE_DANDELIONPP_FLUFF_PROBABILITY 10 // out of 100
#define CRYPTONOTE_DANDELIONPP_MIN_EPOCH         10 // minutes
#define CRYPTONOTE_DANDELIONPP_EPOCH_RANGE       30 // seconds
#define CRYPTONOTE_DANDELIONPP_FLUSH_AVERAGE      5 // seconds average for poisson distributed fluff flush
#define CRYPTONOTE_DANDELIONPP_EMBARGO_AVERAGE  173 // seconds (see tx_pool.cpp for more info)

extern bool CRYPTONOTE_DANDELIONPP_ENABLE;          // true // enable dandelion protocol
extern bool CRYPTONOTE_DANDELIONPP_ENABLE_OPT;      // true // enable dandelion protocol

// see src/cryptonote_protocol/levin_notify.cpp
#define CRYPTONOTE_NOISE_MIN_EPOCH                      5      // minutes
#define CRYPTONOTE_NOISE_EPOCH_RANGE                    30     // seconds
#define CRYPTONOTE_NOISE_MIN_DELAY                      10     // seconds
#define CRYPTONOTE_NOISE_DELAY_RANGE                    5      // seconds
#define CRYPTONOTE_NOISE_BYTES                          3*1024 // 3 KiB
#define CRYPTONOTE_NOISE_CHANNELS                       2      // Max outgoing connections per zone used for noise/covert sending

#define CRYPTONOTE_MAX_FRAGMENTS                        20 // ~20 * NOISE_BYTES max payload size for covert/noise send

#define COMMAND_RPC_GET_BLOCKS_FAST_MAX_COUNT           1000

#define P2P_LOCAL_WHITE_PEERLIST_LIMIT                  1000
#define P2P_LOCAL_GRAY_PEERLIST_LIMIT                   5000

#define P2P_DEFAULT_CONNECTIONS_COUNT                   8
#define P2P_DEFAULT_HANDSHAKE_INTERVAL                  60           //secondes
#define P2P_DEFAULT_PACKET_MAX_SIZE                     50000000     //50000000 bytes maximum packet size
#define P2P_DEFAULT_PEERS_IN_HANDSHAKE                  250
#define P2P_DEFAULT_CONNECTION_TIMEOUT                  5000       //5 seconds
#define P2P_DEFAULT_SOCKS_CONNECT_TIMEOUT               45         // seconds
#define P2P_DEFAULT_PING_CONNECTION_TIMEOUT             2000       //2 seconds
#define P2P_DEFAULT_INVOKE_TIMEOUT                      60*2*1000  //2 minutes
#define P2P_DEFAULT_HANDSHAKE_INVOKE_TIMEOUT            5000       //5 seconds
#define P2P_DEFAULT_WHITELIST_CONNECTIONS_PERCENT       70
#define P2P_DEFAULT_ANCHOR_CONNECTIONS_COUNT            2
#define P2P_DEFAULT_SYNC_SEARCH_CONNECTIONS_COUNT       2
#define P2P_DEFAULT_LIMIT_RATE_UP                       2048       // kB/s
#define P2P_DEFAULT_LIMIT_RATE_DOWN                     8192       // kB/s

#define P2P_FAILED_ADDR_FORGET_SECONDS                  (60*60)     //1 hour
#define P2P_IP_BLOCKTIME                                (60*60*24)  //24 hour
#define P2P_IP_FAILS_BEFORE_BLOCK                       10
#define P2P_IDLE_CONNECTION_KILL_INTERVAL               (5*60) //5 minutes

#define P2P_SUPPORT_FLAG_FLUFFY_BLOCKS                  0x01
#define P2P_SUPPORT_FLAGS                               P2P_SUPPORT_FLAG_FLUFFY_BLOCKS

#define RPC_IP_FAILS_BEFORE_BLOCK                       3

extern std::string CRYPTONOTE_NAME;

#define CRYPTONOTE_DEFAULT_NAME                 "gyro"
#define CRYPTONOTE_POOLDATA_FILENAME            "poolstate.bin"
#define CRYPTONOTE_BLOCKCHAINDATA_FILENAME      "data.mdb"
#define CRYPTONOTE_BLOCKCHAINDATA_LOCK_FILENAME "lock.mdb"
#define P2P_NET_DATA_FILENAME                   "p2pstate.bin"
#define RPC_PAYMENTS_DATA_FILENAME              "rpcpayments.bin"
#define SPINNER_CONFIG_FILE_NAME                "spinner_conf.json"

#define THREAD_STACK_SIZE                       5 * 1024 * 1024

#define HF_VERSION_DYNAMIC_FEE                  4
#define HF_VERSION_MIN_MIXIN_4                  6
#define HF_VERSION_MIN_MIXIN_6                  7
#define HF_VERSION_MIN_MIXIN_10                 8
#define HF_VERSION_ENFORCE_RCT                  6
#define HF_VERSION_PER_BYTE_FEE                 8
#define HF_VERSION_SMALLER_BP                   10
#define HF_VERSION_LONG_TERM_BLOCK_WEIGHT       10
#define HF_VERSION_MIN_2_OUTPUTS                12
#define HF_VERSION_MIN_V2_COINBASE_TX           12
#define HF_VERSION_SAME_MIXIN                   12
#define HF_VERSION_REJECT_SIGS_IN_COINBASE      12
#define HF_VERSION_ENFORCE_MIN_AGE              12
#define HF_VERSION_EFFECTIVE_SHORT_TERM_MEDIAN_IN_PENALTY 12

#define PER_KB_FEE_QUANTIZATION_DECIMALS        8

#define HASH_OF_HASHES_STEP                     512

#define DEFAULT_TXPOOL_MAX_WEIGHT               648000000ull // 3 days at 300000, in bytes

#define BULLETPROOF_MAX_OUTPUTS                 16

#define CRYPTONOTE_PRUNING_STRIPE_SIZE          4096 // the smaller, the smoother the increase
#define CRYPTONOTE_PRUNING_LOG_STRIPES          3 // the higher, the more space saved
#define CRYPTONOTE_PRUNING_TIP_BLOCKS           5500 // the smaller, the more space saved
#define CRYPTONOTE_PRUNING_DEBUG_SPOOF_SEED

#define RPC_CREDITS_PER_HASH_SCALE ((float)(1<<24))

extern uint64_t START_AMOUNT_BLOCKS;            // 100
extern uint64_t START_AMOUNT;

extern uint64_t SPINNER_AMOUNT_MIN_LOCK; //   COIN
extern uint64_t SPINNER_MOMENT_OF_INERTIA; //   5
extern uint64_t SPINNER_DAMPING_RATIO_DIVIDER; // 2000 (result in picoJoule)
extern uint64_t SPINNER_ENERGY_COST_MULTIPLIER; //   100000  // 100000 picocoins of 1 picoJoule
extern uint64_t SPINNER_INTERVAL_DYNAMIC_FACTOR; //   5
extern uint64_t SPINNER_PVRB_BLOCK_WINDOW; //   5 - publicly verifiable random beacon spinner block window
extern uint64_t SPINNER_SPIN_BLOCK_WINDOW; //   100
extern uint64_t SPINNER_COUNT_PRECISION; //   10

extern uint64_t TX_EXTRA_SMS_MAX_COUNT;      // 4096

extern std::string GYRO_DONATION_ADDR;



// New constants are intended to go here
namespace config
{
  uint64_t const DEFAULT_FEE_ATOMIC_XMR_PER_KB = 500; // Just a placeholder!  Change me!
  uint8_t const FEE_CALCULATION_MAX_RETRIES = 10;
  uint64_t const DEFAULT_DUST_THRESHOLD = ((uint64_t)2000000000); // 2 * pow(10, 9)
  uint64_t const BASE_REWARD_CLAMP_THRESHOLD = ((uint64_t)100000000); // pow(10, 8)

  extern uint64_t CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX; // = 17;
  extern uint64_t CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX; // = 18;
  extern uint64_t CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX; // = 41;
  extern uint16_t P2P_DEFAULT_PORT; // = 11080;
  extern uint16_t RPC_DEFAULT_PORT; // = 11081;
  extern uint16_t ZMQ_RPC_DEFAULT_PORT; // = 11082;
  extern boost::uuids::uuid NETWORK_ID; /* = { {
      0xc8, 0x4c, 0xfc, 0x07, 0x88, 0x45, 0x42, 0x83, 0x85, 0xb2, 0xbb, 0xfa, 0x9b, 0xc8, 0xf5, 0x7d
    } }; */ // Bender's nightmare
  extern std::string GENESIS_TX; // = "01003c01ff0001ffffffffffff0302747bf23f204ae2945023991a10e1e620f86c1e4c949fa4096a15e7f2440968ae2101576181835c5468c488e17fc548c19f9b77eba028da171346ece324744a6fff6c";
  extern uint32_t GENESIS_NONCE; // = 10000;

  // Hash domain separators
  const char HASH_KEY_BULLETPROOF_EXPONENT[] = "bulletproof";
  const char HASH_KEY_RINGDB[] = "ringdsb";
  const char HASH_KEY_SUBADDRESS[] = "SubAddr";
  const unsigned char HASH_KEY_ENCRYPTED_PAYMENT_ID = 0x8d;
  const unsigned char HASH_KEY_WALLET = 0x8c;
  const unsigned char HASH_KEY_WALLET_CACHE = 0x8d;
  const unsigned char HASH_KEY_RPC_PAYMENT_NONCE = 0x58;
  const unsigned char HASH_KEY_MEMORY = 'k';
  const unsigned char HASH_KEY_MULTISIG[] = {'M', 'u', 'l', 't' , 'i', 's', 'i', 'g', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

  namespace testnet
  {
    extern uint64_t CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX; // = 52;
    extern uint64_t CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX; // = 53;
    extern uint64_t CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX; // = 62;
    extern uint16_t P2P_DEFAULT_PORT; // = 21080;
    extern uint16_t RPC_DEFAULT_PORT; // = 21081;
    extern uint16_t ZMQ_RPC_DEFAULT_PORT; // = 21082;
    extern boost::uuids::uuid NETWORK_ID; /* = { {
        0x4b, 0x6b, 0xd2, 0xa3, 0xe5, 0x6f, 0x4d, 0x4b, 0xbb, 0x83, 0x4a, 0xf6, 0x14, 0x2c, 0x28, 0x02
      } }; */ // Bender's daydream
    extern std::string GENESIS_TX; // = "01003c01ff0001ffffffffffff030232bbe8c4a21f1f869f9eb1c841a244244af5372c692ab0bbf6c6fca0c106f05121012d6d9465b206bc1db2af37204643c7964f9c5710d37c28dc60597f911a59ca06";
    extern uint32_t GENESIS_NONCE; // = 10001;
  }

  namespace stagenet
  {
    extern uint64_t CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX; // = 24;
    extern uint64_t CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX; // = 25;
    extern uint64_t CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX; // = 36;
    extern uint16_t P2P_DEFAULT_PORT; // = 31080;
    extern uint16_t RPC_DEFAULT_PORT; // = 31081;
    extern uint16_t ZMQ_RPC_DEFAULT_PORT; // = 31082;
    extern boost::uuids::uuid NETWORK_ID; /* = { {
        0x3a, 0x70, 0xb7, 0xc1, 0xd0, 0x14, 0x4f, 0x6c, 0xa6, 0xdd, 0xb6, 0xf8, 0x52, 0x84, 0xd0, 0x41
      } }; */ // Bender's daydream
    extern std::string GENESIS_TX; // = "01003c01ff0001ffffffffffff03023b590a0f062f3d549ef67498ff32bda325d4909c627a53eed07e032ee66d9a8f21018f9367d7371077c9dcf4447ed6b88cdd8b1f24efb5204f260b77c0637958e656";
    extern uint32_t GENESIS_NONCE; // = 10002;
  }
  void init_options(boost::program_options::options_description & option_spec);
  void init(boost::program_options::variables_map const & vm);
  void new_network_id();
  void new_testnet_network_id();
  void new_stagenet_network_id();
  crypto::hash get_cryptonote_config_hash();
  crypto::hash get_testnet_cryptonote_config_hash();
  crypto::hash get_stagenet_cryptonote_config_hash();
  bool load_config_file(const std::string& config);
}

namespace cryptonote
{
  enum network_type : uint8_t
  {
    MAINNET = 0,
    TESTNET,
    STAGENET,
    FAKECHAIN,
    UNDEFINED = 255
  };
  struct config_t
  {
    uint64_t const CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX;
    uint64_t const CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX;
    uint64_t const CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX;
    uint16_t const P2P_DEFAULT_PORT;
    uint16_t const RPC_DEFAULT_PORT;
    uint16_t const ZMQ_RPC_DEFAULT_PORT;
    boost::uuids::uuid const NETWORK_ID;
    std::string const GENESIS_TX;
    uint32_t const GENESIS_NONCE;
  };
  inline const config_t& get_config(network_type nettype)
  {
    static const config_t mainnet = {
      ::config::CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX,
      ::config::CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX,
      ::config::CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX,
      ::config::P2P_DEFAULT_PORT,
      ::config::RPC_DEFAULT_PORT,
      ::config::ZMQ_RPC_DEFAULT_PORT,
      ::config::NETWORK_ID,
      ::config::GENESIS_TX,
      ::config::GENESIS_NONCE
    };
    static const config_t testnet = {
      ::config::testnet::CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX,
      ::config::testnet::CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX,
      ::config::testnet::CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX,
      ::config::testnet::P2P_DEFAULT_PORT,
      ::config::testnet::RPC_DEFAULT_PORT,
      ::config::testnet::ZMQ_RPC_DEFAULT_PORT,
      ::config::testnet::NETWORK_ID,
      ::config::testnet::GENESIS_TX,
      ::config::testnet::GENESIS_NONCE
    };
    static const config_t stagenet = {
      ::config::stagenet::CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX,
      ::config::stagenet::CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX,
      ::config::stagenet::CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX,
      ::config::stagenet::P2P_DEFAULT_PORT,
      ::config::stagenet::RPC_DEFAULT_PORT,
      ::config::stagenet::ZMQ_RPC_DEFAULT_PORT,
      ::config::stagenet::NETWORK_ID,
      ::config::stagenet::GENESIS_TX,
      ::config::stagenet::GENESIS_NONCE
    };
    switch (nettype)
    {
      case MAINNET: return mainnet;
      case TESTNET: return testnet;
      case STAGENET: return stagenet;
      case FAKECHAIN: return mainnet;
      default: throw std::runtime_error("Invalid network type");
    }
  }
}
