#include <stdexcept>

#include "cryptonote_config.h"
#include "common/command_line.h"
#include "string_tools.h"
#include "crypto/crypto.h"
#include "crypto/hash.h"

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/filesystem.hpp>

uint64_t DURATION_TARGET =                    120;  // seconds
uint64_t DURATION_TARGET_LAG =                5;  // seconds
uint64_t LOCKED_BLOCKS_INCREEZE_INTERVAL =    100;  // blocks
uint64_t SPINNER_MOMENT_OF_INERTIA =          5;
uint64_t SPINNER_DAMPING_RATIO_DIVIDER =      2000;
uint64_t SPINNER_ENERGY_COST_MULTIPLIER =     100000; // picocoins multiplier for picojoule
uint64_t SPINNER_INTERVAL_DYNAMIC_FACTOR =    5;
uint64_t SPINNER_PVRB_BLOCK_WINDOW =          5;
uint64_t SPINNER_SPIN_BLOCK_WINDOW =          100;
uint64_t SPINNER_COUNT_PRECISION =            10;
uint64_t CRYPTONOTE_MEMPOOL_TX_LIVETIME =     (86400*3); //seconds, three days
uint64_t CRYPTONOTE_MEMPOOL_TX_FROM_ALT_BLOCK_LIVETIME = 604800; //seconds, one week
uint64_t CRYPTONOTE_MESSAGE_TX_LIVETIME =     86400; //seconds, 1 day
bool CRYPTONOTE_DANDELIONPP_ENABLE =          true;
bool CRYPTONOTE_DANDELIONPP_ENABLE_OPT =      true;
uint64_t TX_EXTRA_SMS_MAX_COUNT =             4096;
std::string GYRO_DONATION_ADDR =              "";

uint64_t EMISSION_SUBSIDY_PER_MINUTE =        20;
uint64_t FINAL_SUBSIDY_PER_MINUTE =           ((uint64_t)3000000000); // 3 * pow(10, 9)

uint32_t CRYPTONOTE_DISPLAY_DECIMAL_POINT =   12; // COIN - number of smallest units in one coin
uint64_t COIN =                               COIN_DEFAULT; //(uint64_t)1000000000000; // pow(10, 12)

std::string CRYPTONOTE_NAME =                 CRYPTONOTE_DEFAULT_NAME;

uint64_t START_AMOUNT_BLOCKS =                100;
uint64_t START_AMOUNT =                       1000 * COIN_DEFAULT;

// New constants are intended to go here
namespace config
{
  const char *sz_GENESIS_TX="013c01ff000101023fea7d719022816406a484159307f1b97780e6278cd4b790eec5ec94edf263544201974557852ad06ab901c1095bbe8cf7b7010b0aa45416f407988f73713badf31506d1e384166b71875aa98ade9d3880ba2c50ee08bc97c0911471d8fd7d6a9b2a5e";
  const char *sz_NETWORK_ID="1741eaa7-1635-420c-9f60-9c606c0d5c49";
  namespace testnet
  {
    const char *sz_GENESIS_TX="013c01ff0001010252762a81ccf8ac99b206826e394497ad95abdab98a31e16ca41f53acb59d84fc4201f555449f939fe89026be27afdac4eef06bd95daf9c88303c9734aaceef4eaa6306404b584e6afbd98a9e12713cd1d5e9bbedbddc982156f8a9e81a8abc0eeb55e5";
    const char *sz_NETWORK_ID="c18fe00a-37f8-49fa-a582-3380cc689cb9";
  }
  namespace stagenet
  {
    const char *sz_GENESIS_TX="013c01ff00010102c96d64a5d2de9f8ef61e2f03c794285fc2f2455125c076c1fdb649ab6170afaf4201fa30db8ab82231ca2d54f2c87c72d2d1ec906dfc0fa01c57c9ae6e7b0d8d4b5b066978853218838a0f1241e5ef83c6a0293f4cf26dff22240394ba782ff65d12e6";
    const char *sz_NETWORK_ID="80405031-3c35-4062-8f89-13019eb19ada";
  }

  uint64_t CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX = 17;
  uint64_t CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX = 18;
  uint64_t CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX = 41;
  uint16_t P2P_DEFAULT_PORT = 11080;
  uint16_t RPC_DEFAULT_PORT = 11081;
  uint16_t ZMQ_RPC_DEFAULT_PORT = 11082;
  boost::uuids::uuid NETWORK_ID = boost::uuids::string_generator()(sz_NETWORK_ID);
  std::string GENESIS_TX = sz_GENESIS_TX;
  uint32_t GENESIS_NONCE = 10000;

  namespace testnet
  {
    uint64_t CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX = 52;
    uint64_t CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX = 53;
    uint64_t CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX = 62;
    uint16_t P2P_DEFAULT_PORT = 21080;
    uint16_t RPC_DEFAULT_PORT = 21081;
    uint16_t ZMQ_RPC_DEFAULT_PORT = 21082;
    boost::uuids::uuid NETWORK_ID = boost::uuids::string_generator()(sz_NETWORK_ID);
    std::string GENESIS_TX = sz_GENESIS_TX;
    uint32_t GENESIS_NONCE = 10001;
  }

  namespace stagenet
  {
    uint64_t CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX = 24;
    uint64_t CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX = 25;
    uint64_t CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX = 36;
    uint16_t P2P_DEFAULT_PORT = 31080;
    uint16_t RPC_DEFAULT_PORT = 31081;
    uint16_t ZMQ_RPC_DEFAULT_PORT = 31082;
    boost::uuids::uuid NETWORK_ID = boost::uuids::string_generator()(sz_NETWORK_ID);
    std::string GENESIS_TX = sz_GENESIS_TX;
    uint32_t GENESIS_NONCE = 10002;
  }

  const command_line::arg_descriptor<uint64_t> arg_DURATION_TARGET = {
    "DURATION_TARGET", "timeout blockchain", 120 };
  const command_line::arg_descriptor<uint64_t> arg_DURATION_TARGET_LAG = {
    "DURATION_TARGET_LAG", "timeout lag blockchain", 5 };
  const command_line::arg_descriptor<uint64_t> arg_LOCKED_BLOCKS_INCREEZE_INTERVAL = {
    "LOCKED_BLOCKS_INCREEZE_INTERVAL", "base spin correction interval", 100 };
  const command_line::arg_descriptor<uint64_t> arg_SPINNER_MOMENT_OF_INERTIA = {
    "SPINNER_MOMENT_OF_INERTIA", "spinner moment of inertia", 5 };
  const command_line::arg_descriptor<uint64_t> arg_SPINNER_DAMPING_RATIO_DIVIDER = {
    "SPINNER_DAMPING_RATIO_DIVIDER", "spinner damping ratio divider", 2000 };
  const command_line::arg_descriptor<uint64_t> arg_SPINNER_ENERGY_COST_MULTIPLIER = {
    "SPINNER_ENERGY_COST_MULTIPLIER", "spinner energy cost multiplier", 1000 };
  const command_line::arg_descriptor<uint64_t> arg_SPINNER_INTERVAL_DYNAMIC_FACTOR = {
    "SPINNER_INTERVAL_DYNAMIC_FACTOR", "spinner interval dynamic factor", 5 };
  const command_line::arg_descriptor<uint64_t> arg_SPINNER_PVRB_BLOCK_WINDOW = {
    "SPINNER_PVRB_BLOCK_WINDOW", "publicly verifiable random beacon", 5 };
  const command_line::arg_descriptor<uint64_t> arg_SPINNER_SPIN_BLOCK_WINDOW = {
    "SPINNER_SPIN_BLOCK_WINDOW", "spinning block window", 100 };
  const command_line::arg_descriptor<uint64_t> arg_SPINNER_COUNT_PRECISION = {
    "SPINNER_COUNT_PRECISION", "spinner count precision", 10 };
  const command_line::arg_descriptor<uint64_t> arg_CRYPTONOTE_MEMPOOL_TX_LIVETIME = {
    "CRYPTONOTE_MEMPOOL_TX_LIVETIME", "memory pool livetime", (86400*3) };
  const command_line::arg_descriptor<uint64_t> arg_CRYPTONOTE_MEMPOOL_TX_FROM_ALT_BLOCK_LIVETIME = {
    "CRYPTONOTE_MEMPOOL_TX_FROM_ALT_BLOCK_LIVETIME", "memory pool from alt block livetime", 604800 };
  const command_line::arg_descriptor<uint64_t> arg_CRYPTONOTE_MESSAGE_TX_LIVETIME = {
    "CRYPTONOTE_MESSAGE_TX_LIVETIME", "memory pool message tx livetime", 86400 };
  const command_line::arg_descriptor<uint64_t> arg_CRYPTONOTE_DANDELIONPP_ENABLE = {
    "CRYPTONOTE_DANDELIONPP_ENABLE", "cryptonote dandelionpp enable", true };
  const command_line::arg_descriptor<uint64_t> arg_CRYPTONOTE_DANDELIONPP_ENABLE_OPT = {
    "CRYPTONOTE_DANDELIONPP_ENABLE_OPT", "cryptonote dandelionpp enable local optional", true };
  const command_line::arg_descriptor<uint64_t> arg_TX_EXTRA_SMS_MAX_COUNT = {
    "TX_EXTRA_SMS_MAX_COUNT", "message max count", 4096 };
  const command_line::arg_descriptor<std::string> arg_GYRO_DONATION_ADDR = {
    "GYRO_DONATION_ADDR", "donation", "" };
  const command_line::arg_descriptor<std::string> arg_CRYPTONOTE_NAME = {
    "CRYPTONOTE_NAME", "cryptonote name of coin", "gyro" };
  const command_line::arg_descriptor<uint32_t> arg_CRYPTONOTE_DISPLAY_DECIMAL_POINT = {
    "CRYPTONOTE_DISPLAY_DECIMAL_POINT", "Cryptonote display decimal point", 12 };
  const command_line::arg_descriptor<uint64_t> arg_EMISSION_SUBSIDY_PER_MINUTE = {
    "EMISSION_SUBSIDY_PER_MINUTE", "emission speed factor per minute", 20 };
  const command_line::arg_descriptor<uint64_t> arg_FINAL_SUBSIDY_PER_MINUTE = {
    "FINAL_SUBSIDY_PER_MINUTE", "final subsidy per minute", ((uint64_t)300000000000) }; // 3 * pow(10, 11)
  const command_line::arg_descriptor<uint64_t> arg_COIN = {
    "COIN", "COIN - number of smallest units in one coin", ((uint64_t)1000000000000) };
  const command_line::arg_descriptor<uint64_t> arg_START_AMOUNT_BLOCKS = {
    "START_AMOUNT_BLOCKS", "start amount blocks", 100 };
  const command_line::arg_descriptor<uint64_t> arg_START_AMOUNT = {
    "START_AMOUNT", "start amount coins", 1000 * COIN_DEFAULT };
  const command_line::arg_descriptor<uint64_t> arg_CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX = {
    "CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX", "", 17 };
  const command_line::arg_descriptor<uint64_t> arg_CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX = {
    "CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX", "", 18 };
  const command_line::arg_descriptor<uint64_t> arg_CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX = {
    "CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX", "", 42 };
  const command_line::arg_descriptor<uint16_t> arg_P2P_DEFAULT_PORT = {
    "P2P_DEFAULT_PORT", "", 11080 };
  const command_line::arg_descriptor<uint16_t> arg_RPC_DEFAULT_PORT = {
    "RPC_DEFAULT_PORT", "", 11081 };
  const command_line::arg_descriptor<uint16_t> arg_ZMQ_RPC_DEFAULT_PORT = {
    "ZMQ_RPC_DEFAULT_PORT", "", 11082 };
  const command_line::arg_descriptor<std::string> arg_NETWORK_ID = {
    "NETWORK_ID", "", sz_NETWORK_ID };
  const command_line::arg_descriptor<std::string> arg_GENESIS_TX = {
    "GENESIS_TX", "", sz_GENESIS_TX };
  const command_line::arg_descriptor<uint32_t> arg_GENESIS_NONCE = {
    "GENESIS_NONCE", "", 10000 };


  // testnet
  namespace testnet
  {
  const command_line::arg_descriptor<uint64_t> arg_CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX = {
    "testnet.CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX", "", 52 };
  const command_line::arg_descriptor<uint64_t> arg_CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX = {
    "testnet.CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX", "", 53 };
  const command_line::arg_descriptor<uint64_t> arg_CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX = {
    "testnet.CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX", "", 62 };
  const command_line::arg_descriptor<uint16_t> arg_P2P_DEFAULT_PORT = {
    "testnet.P2P_DEFAULT_PORT", "", 21080 };
  const command_line::arg_descriptor<uint16_t> arg_RPC_DEFAULT_PORT = {
    "testnet.RPC_DEFAULT_PORT", "", 21081 };
  const command_line::arg_descriptor<uint16_t> arg_ZMQ_RPC_DEFAULT_PORT = {
    "testnet.ZMQ_RPC_DEFAULT_PORT", "", 21082 };
  const command_line::arg_descriptor<std::string> arg_NETWORK_ID = {
    "testnet.NETWORK_ID", "", sz_NETWORK_ID };
  const command_line::arg_descriptor<std::string> arg_GENESIS_TX = {
    "testnet.GENESIS_TX", "", sz_GENESIS_TX };
  const command_line::arg_descriptor<uint32_t> arg_GENESIS_NONCE = {
    "testnet.GENESIS_NONCE", "", 10001 };
  }

  // stagenet
  namespace stagenet
  {
  const command_line::arg_descriptor<uint64_t> arg_CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX = {
    "stagenet.CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX", "", 24 };
  const command_line::arg_descriptor<uint64_t> arg_CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX = {
    "stagenet.CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX", "", 25 };
  const command_line::arg_descriptor<uint64_t> arg_CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX = {
    "stagenet.CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX", "", 36 };
  const command_line::arg_descriptor<uint16_t> arg_P2P_DEFAULT_PORT = {
    "stagenet.P2P_DEFAULT_PORT", "", 31080 };
  const command_line::arg_descriptor<uint16_t> arg_RPC_DEFAULT_PORT = {
    "stagenet.RPC_DEFAULT_PORT", "", 31081 };
  const command_line::arg_descriptor<uint16_t> arg_ZMQ_RPC_DEFAULT_PORT = {
    "stagenet.ZMQ_RPC_DEFAULT_PORT", "", 31082 };
  const command_line::arg_descriptor<std::string> arg_NETWORK_ID = {
    "stagenet.NETWORK_ID", "", sz_NETWORK_ID };
  const command_line::arg_descriptor<std::string> arg_GENESIS_TX = {
    "stagenet.GENESIS_TX", "", sz_GENESIS_TX };
  const command_line::arg_descriptor<uint32_t> arg_GENESIS_NONCE = {
    "stagenet.GENESIS_NONCE", "", 10002 };
  }

  void init_options(boost::program_options::options_description & option_spec)
  {
      command_line::add_arg(option_spec, arg_CRYPTONOTE_NAME);
      command_line::add_arg(option_spec, arg_CRYPTONOTE_DISPLAY_DECIMAL_POINT);
      command_line::add_arg(option_spec, arg_EMISSION_SUBSIDY_PER_MINUTE);
      command_line::add_arg(option_spec, arg_FINAL_SUBSIDY_PER_MINUTE);
      command_line::add_arg(option_spec, arg_COIN);
      command_line::add_arg(option_spec, arg_DURATION_TARGET);
      command_line::add_arg(option_spec, arg_DURATION_TARGET_LAG);
      command_line::add_arg(option_spec, arg_START_AMOUNT_BLOCKS);
      command_line::add_arg(option_spec, arg_START_AMOUNT);
      command_line::add_arg(option_spec, arg_LOCKED_BLOCKS_INCREEZE_INTERVAL);
      command_line::add_arg(option_spec, arg_SPINNER_MOMENT_OF_INERTIA);
      command_line::add_arg(option_spec, arg_SPINNER_DAMPING_RATIO_DIVIDER);
      command_line::add_arg(option_spec, arg_SPINNER_ENERGY_COST_MULTIPLIER);
      command_line::add_arg(option_spec, arg_SPINNER_INTERVAL_DYNAMIC_FACTOR);
      command_line::add_arg(option_spec, arg_SPINNER_PVRB_BLOCK_WINDOW);
      command_line::add_arg(option_spec, arg_SPINNER_SPIN_BLOCK_WINDOW);
      command_line::add_arg(option_spec, arg_SPINNER_COUNT_PRECISION);
      command_line::add_arg(option_spec, arg_CRYPTONOTE_MEMPOOL_TX_LIVETIME);
      command_line::add_arg(option_spec, arg_CRYPTONOTE_MEMPOOL_TX_FROM_ALT_BLOCK_LIVETIME);
      command_line::add_arg(option_spec, arg_CRYPTONOTE_MESSAGE_TX_LIVETIME);
      command_line::add_arg(option_spec, arg_CRYPTONOTE_DANDELIONPP_ENABLE);
      command_line::add_arg(option_spec, arg_CRYPTONOTE_DANDELIONPP_ENABLE_OPT);
      command_line::add_arg(option_spec, arg_TX_EXTRA_SMS_MAX_COUNT);
      command_line::add_arg(option_spec, arg_GYRO_DONATION_ADDR);

      // main
      command_line::add_arg(option_spec, arg_CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX);
      command_line::add_arg(option_spec, arg_CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX);
      command_line::add_arg(option_spec, arg_CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX);
      command_line::add_arg(option_spec, arg_P2P_DEFAULT_PORT);
      command_line::add_arg(option_spec, arg_RPC_DEFAULT_PORT);
      command_line::add_arg(option_spec, arg_ZMQ_RPC_DEFAULT_PORT);
      command_line::add_arg(option_spec, arg_NETWORK_ID);
      command_line::add_arg(option_spec, arg_GENESIS_TX);
      command_line::add_arg(option_spec, arg_GENESIS_NONCE);

      // testnet
      command_line::add_arg(option_spec, testnet::arg_CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX);
      command_line::add_arg(option_spec, testnet::arg_CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX);
      command_line::add_arg(option_spec, testnet::arg_CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX);
      command_line::add_arg(option_spec, testnet::arg_P2P_DEFAULT_PORT);
      command_line::add_arg(option_spec, testnet::arg_RPC_DEFAULT_PORT);
      command_line::add_arg(option_spec, testnet::arg_ZMQ_RPC_DEFAULT_PORT);
      command_line::add_arg(option_spec, testnet::arg_NETWORK_ID);
      command_line::add_arg(option_spec, testnet::arg_GENESIS_TX);
      command_line::add_arg(option_spec, testnet::arg_GENESIS_NONCE);

      // stagenet
      command_line::add_arg(option_spec, stagenet::arg_CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX);
      command_line::add_arg(option_spec, stagenet::arg_CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX);
      command_line::add_arg(option_spec, stagenet::arg_CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX);
      command_line::add_arg(option_spec, stagenet::arg_P2P_DEFAULT_PORT);
      command_line::add_arg(option_spec, stagenet::arg_RPC_DEFAULT_PORT);
      command_line::add_arg(option_spec, stagenet::arg_ZMQ_RPC_DEFAULT_PORT);
      command_line::add_arg(option_spec, stagenet::arg_NETWORK_ID);
      command_line::add_arg(option_spec, stagenet::arg_GENESIS_TX);
      command_line::add_arg(option_spec, stagenet::arg_GENESIS_NONCE);
  }

  void init(boost::program_options::variables_map const & vm)
  {
      CRYPTONOTE_NAME                                       = command_line::get_arg(vm, arg_CRYPTONOTE_NAME);
      CRYPTONOTE_DISPLAY_DECIMAL_POINT                      = command_line::get_arg(vm, arg_CRYPTONOTE_DISPLAY_DECIMAL_POINT);
      EMISSION_SUBSIDY_PER_MINUTE                      = command_line::get_arg(vm, arg_EMISSION_SUBSIDY_PER_MINUTE);
      FINAL_SUBSIDY_PER_MINUTE                              = command_line::get_arg(vm, arg_FINAL_SUBSIDY_PER_MINUTE);
      COIN                                                  = command_line::get_arg(vm, arg_COIN);
      DURATION_TARGET                                       = command_line::get_arg(vm, arg_DURATION_TARGET);
      DURATION_TARGET_LAG                                   = command_line::get_arg(vm, arg_DURATION_TARGET_LAG);
      START_AMOUNT_BLOCKS                                   = command_line::get_arg(vm, arg_START_AMOUNT_BLOCKS);
      START_AMOUNT                                          = command_line::get_arg(vm, arg_START_AMOUNT);
      LOCKED_BLOCKS_INCREEZE_INTERVAL                       = command_line::get_arg(vm, arg_LOCKED_BLOCKS_INCREEZE_INTERVAL);
      SPINNER_MOMENT_OF_INERTIA                             = command_line::get_arg(vm, arg_SPINNER_MOMENT_OF_INERTIA);
      SPINNER_DAMPING_RATIO_DIVIDER                         = command_line::get_arg(vm, arg_SPINNER_DAMPING_RATIO_DIVIDER);
      SPINNER_ENERGY_COST_MULTIPLIER                        = command_line::get_arg(vm, arg_SPINNER_ENERGY_COST_MULTIPLIER);
      SPINNER_INTERVAL_DYNAMIC_FACTOR                       = command_line::get_arg(vm, arg_SPINNER_INTERVAL_DYNAMIC_FACTOR);
      SPINNER_PVRB_BLOCK_WINDOW                             = command_line::get_arg(vm, arg_SPINNER_PVRB_BLOCK_WINDOW);
      SPINNER_SPIN_BLOCK_WINDOW                             = command_line::get_arg(vm, arg_SPINNER_SPIN_BLOCK_WINDOW);
      SPINNER_COUNT_PRECISION                               = command_line::get_arg(vm, arg_SPINNER_COUNT_PRECISION);
      CRYPTONOTE_MEMPOOL_TX_LIVETIME                        = command_line::get_arg(vm, arg_CRYPTONOTE_MEMPOOL_TX_LIVETIME);
      CRYPTONOTE_MEMPOOL_TX_FROM_ALT_BLOCK_LIVETIME         = command_line::get_arg(vm, arg_CRYPTONOTE_MEMPOOL_TX_FROM_ALT_BLOCK_LIVETIME);
      CRYPTONOTE_MESSAGE_TX_LIVETIME                        = command_line::get_arg(vm, arg_CRYPTONOTE_MESSAGE_TX_LIVETIME);
      CRYPTONOTE_DANDELIONPP_ENABLE                         = command_line::get_arg(vm, arg_CRYPTONOTE_DANDELIONPP_ENABLE);
      CRYPTONOTE_DANDELIONPP_ENABLE_OPT                     = command_line::get_arg(vm, arg_CRYPTONOTE_DANDELIONPP_ENABLE_OPT);
      TX_EXTRA_SMS_MAX_COUNT                                = command_line::get_arg(vm, arg_TX_EXTRA_SMS_MAX_COUNT);
      GYRO_DONATION_ADDR                                    = command_line::get_arg(vm, arg_GYRO_DONATION_ADDR);

      CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX               = command_line::get_arg(vm, arg_CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX);
      CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX            = command_line::get_arg(vm, arg_CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX);
      CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX    = command_line::get_arg(vm, arg_CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX);
      P2P_DEFAULT_PORT                                      = command_line::get_arg(vm, arg_P2P_DEFAULT_PORT);
      RPC_DEFAULT_PORT                                      = command_line::get_arg(vm, arg_RPC_DEFAULT_PORT);
      ZMQ_RPC_DEFAULT_PORT                                  = command_line::get_arg(vm, arg_ZMQ_RPC_DEFAULT_PORT);
      NETWORK_ID                                            = boost::uuids::string_generator()(command_line::get_arg(vm, arg_NETWORK_ID));
      GENESIS_TX                                            = command_line::get_arg(vm, arg_GENESIS_TX);
      GENESIS_NONCE                                         = command_line::get_arg(vm, arg_GENESIS_NONCE);

      testnet::CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX               = command_line::get_arg(vm, testnet::arg_CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX);
      testnet::CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX            = command_line::get_arg(vm, testnet::arg_CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX);
      testnet::CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX    = command_line::get_arg(vm, testnet::arg_CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX);
      testnet::P2P_DEFAULT_PORT                                      = command_line::get_arg(vm, testnet::arg_P2P_DEFAULT_PORT);
      testnet::RPC_DEFAULT_PORT                                      = command_line::get_arg(vm, testnet::arg_RPC_DEFAULT_PORT);
      testnet::ZMQ_RPC_DEFAULT_PORT                                  = command_line::get_arg(vm, testnet::arg_ZMQ_RPC_DEFAULT_PORT);
      testnet::NETWORK_ID                                            = boost::uuids::string_generator()(command_line::get_arg(vm, testnet::arg_NETWORK_ID));
      testnet::GENESIS_TX                                            = command_line::get_arg(vm, testnet::arg_GENESIS_TX);
      testnet::GENESIS_NONCE                                         = command_line::get_arg(vm, testnet::arg_GENESIS_NONCE);

      stagenet::CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX               = command_line::get_arg(vm, stagenet::arg_CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX);
      stagenet::CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX            = command_line::get_arg(vm, stagenet::arg_CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX);
      stagenet::CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX    = command_line::get_arg(vm, stagenet::arg_CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX);
      stagenet::P2P_DEFAULT_PORT                                      = command_line::get_arg(vm, stagenet::arg_P2P_DEFAULT_PORT);
      stagenet::RPC_DEFAULT_PORT                                      = command_line::get_arg(vm, stagenet::arg_RPC_DEFAULT_PORT);
      stagenet::ZMQ_RPC_DEFAULT_PORT                                  = command_line::get_arg(vm, stagenet::arg_ZMQ_RPC_DEFAULT_PORT);
      stagenet::NETWORK_ID                                            = boost::uuids::string_generator()(command_line::get_arg(vm, stagenet::arg_NETWORK_ID));
      stagenet::GENESIS_TX                                            = command_line::get_arg(vm, stagenet::arg_GENESIS_TX);
      stagenet::GENESIS_NONCE                                         = command_line::get_arg(vm, stagenet::arg_GENESIS_NONCE);
  }

  void new_network_id()
  {
      boost::uuids::random_generator g;
      NETWORK_ID = g();
  }

  void new_testnet_network_id()
  {
      boost::uuids::random_generator g;
      testnet::NETWORK_ID = g();
  }

  void new_stagenet_network_id()
  {
      boost::uuids::random_generator g;
      stagenet::NETWORK_ID = g();
  }

  crypto::hash get_cryptonote_config_hash()
  {
      crypto::hash hash = crypto::null_hash;

      crypto::cn_fast_hash(CRYPTONOTE_NAME.c_str(), CRYPTONOTE_NAME.length(), hash.data);
      crypto::cn_fast_hash(&CRYPTONOTE_DISPLAY_DECIMAL_POINT, sizeof(CRYPTONOTE_DISPLAY_DECIMAL_POINT), hash.data);
      crypto::cn_fast_hash(&COIN, sizeof(COIN), hash.data);
      crypto::cn_fast_hash(&DURATION_TARGET, sizeof(DURATION_TARGET), hash.data);
      crypto::cn_fast_hash(&DURATION_TARGET_LAG, sizeof(DURATION_TARGET_LAG), hash.data);
      crypto::cn_fast_hash(&START_AMOUNT_BLOCKS, sizeof(START_AMOUNT_BLOCKS), hash.data);
      crypto::cn_fast_hash(&START_AMOUNT, sizeof(START_AMOUNT), hash.data);
      crypto::cn_fast_hash(&LOCKED_BLOCKS_INCREEZE_INTERVAL, sizeof(LOCKED_BLOCKS_INCREEZE_INTERVAL), hash.data);
      crypto::cn_fast_hash(&SPINNER_MOMENT_OF_INERTIA, sizeof(SPINNER_MOMENT_OF_INERTIA), hash.data);
      crypto::cn_fast_hash(&SPINNER_DAMPING_RATIO_DIVIDER, sizeof(SPINNER_DAMPING_RATIO_DIVIDER), hash.data);
      crypto::cn_fast_hash(&SPINNER_ENERGY_COST_MULTIPLIER, sizeof(SPINNER_ENERGY_COST_MULTIPLIER), hash.data);
      crypto::cn_fast_hash(&SPINNER_INTERVAL_DYNAMIC_FACTOR, sizeof(SPINNER_INTERVAL_DYNAMIC_FACTOR), hash.data);
      crypto::cn_fast_hash(&SPINNER_PVRB_BLOCK_WINDOW, sizeof(SPINNER_PVRB_BLOCK_WINDOW), hash.data);
      crypto::cn_fast_hash(&SPINNER_SPIN_BLOCK_WINDOW, sizeof(SPINNER_SPIN_BLOCK_WINDOW), hash.data);
      crypto::cn_fast_hash(&SPINNER_COUNT_PRECISION, sizeof(SPINNER_COUNT_PRECISION), hash.data);
      crypto::cn_fast_hash(&CRYPTONOTE_MEMPOOL_TX_LIVETIME, sizeof(CRYPTONOTE_MEMPOOL_TX_LIVETIME), hash.data);
      crypto::cn_fast_hash(&CRYPTONOTE_MEMPOOL_TX_FROM_ALT_BLOCK_LIVETIME, sizeof(CRYPTONOTE_MEMPOOL_TX_FROM_ALT_BLOCK_LIVETIME), hash.data);
      crypto::cn_fast_hash(&CRYPTONOTE_MESSAGE_TX_LIVETIME, sizeof(CRYPTONOTE_MESSAGE_TX_LIVETIME), hash.data);
      crypto::cn_fast_hash(&CRYPTONOTE_DANDELIONPP_ENABLE, sizeof(CRYPTONOTE_DANDELIONPP_ENABLE), hash.data);
      crypto::cn_fast_hash(&TX_EXTRA_SMS_MAX_COUNT, sizeof(TX_EXTRA_SMS_MAX_COUNT), hash.data);
      crypto::cn_fast_hash(&GYRO_DONATION_ADDR, sizeof(GYRO_DONATION_ADDR), hash.data);
      crypto::cn_fast_hash(&EMISSION_SUBSIDY_PER_MINUTE, sizeof(EMISSION_SUBSIDY_PER_MINUTE), hash.data);
      crypto::cn_fast_hash(&FINAL_SUBSIDY_PER_MINUTE, sizeof(FINAL_SUBSIDY_PER_MINUTE), hash.data);
      crypto::cn_fast_hash(&CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX, sizeof(CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX), hash.data);
      crypto::cn_fast_hash(&CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX, sizeof(CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX), hash.data);
      crypto::cn_fast_hash(&CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX, sizeof(CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX), hash.data);
      crypto::cn_fast_hash(&GENESIS_NONCE, sizeof(GENESIS_NONCE), hash.data);
      crypto::cn_fast_hash(NETWORK_ID.data, NETWORK_ID.size(), hash.data);

      return hash;
  }
  crypto::hash get_testnet_cryptonote_config_hash()
  {
      crypto::hash hash = crypto::null_hash;

      crypto::cn_fast_hash(CRYPTONOTE_NAME.c_str(), CRYPTONOTE_NAME.length(), hash.data);
      crypto::cn_fast_hash(&CRYPTONOTE_DISPLAY_DECIMAL_POINT, sizeof(CRYPTONOTE_DISPLAY_DECIMAL_POINT), hash.data);
      crypto::cn_fast_hash(&COIN, sizeof(COIN), hash.data);
      crypto::cn_fast_hash(&EMISSION_SUBSIDY_PER_MINUTE, sizeof(EMISSION_SUBSIDY_PER_MINUTE), hash.data);
      crypto::cn_fast_hash(&FINAL_SUBSIDY_PER_MINUTE, sizeof(FINAL_SUBSIDY_PER_MINUTE), hash.data);
      crypto::cn_fast_hash(&DURATION_TARGET, sizeof(DURATION_TARGET), hash.data);
      crypto::cn_fast_hash(&DURATION_TARGET_LAG, sizeof(DURATION_TARGET_LAG), hash.data);
      crypto::cn_fast_hash(&START_AMOUNT_BLOCKS, sizeof(START_AMOUNT_BLOCKS), hash.data);
      crypto::cn_fast_hash(&START_AMOUNT, sizeof(START_AMOUNT), hash.data);
      crypto::cn_fast_hash(&LOCKED_BLOCKS_INCREEZE_INTERVAL, sizeof(LOCKED_BLOCKS_INCREEZE_INTERVAL), hash.data);
      crypto::cn_fast_hash(&SPINNER_MOMENT_OF_INERTIA, sizeof(SPINNER_MOMENT_OF_INERTIA), hash.data);
      crypto::cn_fast_hash(&SPINNER_DAMPING_RATIO_DIVIDER, sizeof(SPINNER_DAMPING_RATIO_DIVIDER), hash.data);
      crypto::cn_fast_hash(&SPINNER_ENERGY_COST_MULTIPLIER, sizeof(SPINNER_ENERGY_COST_MULTIPLIER), hash.data);
      crypto::cn_fast_hash(&SPINNER_INTERVAL_DYNAMIC_FACTOR, sizeof(SPINNER_INTERVAL_DYNAMIC_FACTOR), hash.data);
      crypto::cn_fast_hash(&SPINNER_PVRB_BLOCK_WINDOW, sizeof(SPINNER_PVRB_BLOCK_WINDOW), hash.data);
      crypto::cn_fast_hash(&SPINNER_SPIN_BLOCK_WINDOW, sizeof(SPINNER_SPIN_BLOCK_WINDOW), hash.data);
      crypto::cn_fast_hash(&SPINNER_COUNT_PRECISION, sizeof(SPINNER_COUNT_PRECISION), hash.data);
      crypto::cn_fast_hash(&CRYPTONOTE_MEMPOOL_TX_LIVETIME, sizeof(CRYPTONOTE_MEMPOOL_TX_LIVETIME), hash.data);
      crypto::cn_fast_hash(&CRYPTONOTE_MEMPOOL_TX_FROM_ALT_BLOCK_LIVETIME, sizeof(CRYPTONOTE_MEMPOOL_TX_FROM_ALT_BLOCK_LIVETIME), hash.data);
      crypto::cn_fast_hash(&CRYPTONOTE_MESSAGE_TX_LIVETIME, sizeof(CRYPTONOTE_MESSAGE_TX_LIVETIME), hash.data);
      crypto::cn_fast_hash(&CRYPTONOTE_DANDELIONPP_ENABLE, sizeof(CRYPTONOTE_DANDELIONPP_ENABLE), hash.data);
      crypto::cn_fast_hash(&TX_EXTRA_SMS_MAX_COUNT, sizeof(TX_EXTRA_SMS_MAX_COUNT), hash.data);
      crypto::cn_fast_hash(&GYRO_DONATION_ADDR, sizeof(GYRO_DONATION_ADDR), hash.data);
      crypto::cn_fast_hash(&testnet::CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX, sizeof(testnet::CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX), hash.data);
      crypto::cn_fast_hash(&testnet::CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX, sizeof(testnet::CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX), hash.data);
      crypto::cn_fast_hash(&testnet::CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX, sizeof(testnet::CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX), hash.data);
      crypto::cn_fast_hash(&testnet::GENESIS_NONCE, sizeof(testnet::GENESIS_NONCE), hash.data);
      crypto::cn_fast_hash(testnet::NETWORK_ID.data, testnet::NETWORK_ID.size(), hash.data);

      return hash;
  }

  crypto::hash get_stagenet_cryptonote_config_hash()
  {
      crypto::hash hash = crypto::null_hash;

      crypto::cn_fast_hash(CRYPTONOTE_NAME.c_str(), CRYPTONOTE_NAME.length(), hash.data);
      crypto::cn_fast_hash(&CRYPTONOTE_DISPLAY_DECIMAL_POINT, sizeof(CRYPTONOTE_DISPLAY_DECIMAL_POINT), hash.data);
      crypto::cn_fast_hash(&COIN, sizeof(COIN), hash.data);
      crypto::cn_fast_hash(&EMISSION_SUBSIDY_PER_MINUTE, sizeof(EMISSION_SUBSIDY_PER_MINUTE), hash.data);
      crypto::cn_fast_hash(&FINAL_SUBSIDY_PER_MINUTE, sizeof(FINAL_SUBSIDY_PER_MINUTE), hash.data);
      crypto::cn_fast_hash(&DURATION_TARGET, sizeof(DURATION_TARGET), hash.data);
      crypto::cn_fast_hash(&DURATION_TARGET_LAG, sizeof(DURATION_TARGET_LAG), hash.data);
      crypto::cn_fast_hash(&START_AMOUNT_BLOCKS, sizeof(START_AMOUNT_BLOCKS), hash.data);
      crypto::cn_fast_hash(&START_AMOUNT, sizeof(START_AMOUNT), hash.data);
      crypto::cn_fast_hash(&LOCKED_BLOCKS_INCREEZE_INTERVAL, sizeof(LOCKED_BLOCKS_INCREEZE_INTERVAL), hash.data);
      crypto::cn_fast_hash(&SPINNER_MOMENT_OF_INERTIA, sizeof(SPINNER_MOMENT_OF_INERTIA), hash.data);
      crypto::cn_fast_hash(&SPINNER_DAMPING_RATIO_DIVIDER, sizeof(SPINNER_DAMPING_RATIO_DIVIDER), hash.data);
      crypto::cn_fast_hash(&SPINNER_ENERGY_COST_MULTIPLIER, sizeof(SPINNER_ENERGY_COST_MULTIPLIER), hash.data);
      crypto::cn_fast_hash(&SPINNER_INTERVAL_DYNAMIC_FACTOR, sizeof(SPINNER_INTERVAL_DYNAMIC_FACTOR), hash.data);
      crypto::cn_fast_hash(&SPINNER_PVRB_BLOCK_WINDOW, sizeof(SPINNER_PVRB_BLOCK_WINDOW), hash.data);
      crypto::cn_fast_hash(&SPINNER_SPIN_BLOCK_WINDOW, sizeof(SPINNER_SPIN_BLOCK_WINDOW), hash.data);
      crypto::cn_fast_hash(&SPINNER_COUNT_PRECISION, sizeof(SPINNER_COUNT_PRECISION), hash.data);
      crypto::cn_fast_hash(&CRYPTONOTE_MEMPOOL_TX_LIVETIME, sizeof(CRYPTONOTE_MEMPOOL_TX_LIVETIME), hash.data);
      crypto::cn_fast_hash(&CRYPTONOTE_MEMPOOL_TX_FROM_ALT_BLOCK_LIVETIME, sizeof(CRYPTONOTE_MEMPOOL_TX_FROM_ALT_BLOCK_LIVETIME), hash.data);
      crypto::cn_fast_hash(&CRYPTONOTE_MESSAGE_TX_LIVETIME, sizeof(CRYPTONOTE_MESSAGE_TX_LIVETIME), hash.data);
      crypto::cn_fast_hash(&CRYPTONOTE_DANDELIONPP_ENABLE, sizeof(CRYPTONOTE_DANDELIONPP_ENABLE), hash.data);
      crypto::cn_fast_hash(&TX_EXTRA_SMS_MAX_COUNT, sizeof(TX_EXTRA_SMS_MAX_COUNT), hash.data);
      crypto::cn_fast_hash(&GYRO_DONATION_ADDR, sizeof(GYRO_DONATION_ADDR), hash.data);
      crypto::cn_fast_hash(&stagenet::CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX, sizeof(stagenet::CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX), hash.data);
      crypto::cn_fast_hash(&stagenet::CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX, sizeof(stagenet::CRYPTONOTE_PUBLIC_SUBADDRESS_BASE58_PREFIX), hash.data);
      crypto::cn_fast_hash(&stagenet::CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX, sizeof(stagenet::CRYPTONOTE_PUBLIC_INTEGRATED_ADDRESS_BASE58_PREFIX), hash.data);
      crypto::cn_fast_hash(&stagenet::GENESIS_NONCE, sizeof(stagenet::GENESIS_NONCE), hash.data);
      crypto::cn_fast_hash(stagenet::NETWORK_ID.data, stagenet::NETWORK_ID.size(), hash.data);

      return hash;
  }
  bool load_config_file(const std::string& config)
  {
    boost::program_options::options_description desc_params;
    boost::program_options::variables_map vm;
    boost::filesystem::path config_path(config);
    boost::system::error_code ec;
    if (boost::filesystem::exists(config_path, ec))
    {
      init_options(desc_params);
      boost::program_options::store(boost::program_options::parse_config_file<char>(config_path.string<std::string>().c_str(), desc_params), vm);
    }
    else
    {
      return false;
    }
    init(vm);
    return true;
  }

}
