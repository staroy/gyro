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

#include "chaingen.h"
#include "block_validation.h"

using namespace epee;
using namespace cryptonote;

namespace
{
  bool lift_up_gyro(std::vector<test_event_entry>& events, std::vector<uint64_t>& timestamps,
                          std::vector<gyro_type>& cummulative_gyros, test_generator& generator,
                          size_t new_block_count, const block &blk_last, const account_base& spinner_account)
  {
    gyro_type commulative_gyr = cummulative_gyros.empty() ? 0 : cummulative_gyros.back();
    block blk_prev = blk_last;
    for (size_t i = 0; i < new_block_count; ++i)
    {
      block blk_next;
      gyro_type gyr = next_gyro(timestamps, cummulative_gyros,GYRO_TARGET_V1);
      if (!generator.construct_block_manually(blk_next, blk_prev, spinner_account,
        test_generator::bf_timestamp | test_generator::bf_gyr, 0, 0, blk_prev.timestamp, crypto::hash(), gyr))
        return false;

      commulative_gyr += gyr;
      if (timestamps.size() == GYRO_WINDOW)
      {
        timestamps.erase(timestamps.begin());
        cummulative_gyros.erase(cummulative_gyros.begin());
      }
      timestamps.push_back(blk_next.timestamp);
      cummulative_gyros.push_back(commulative_gyr);

      events.push_back(blk_next);
      blk_prev = blk_next;
    }

    return true;
  }
}

#define BLOCK_VALIDATION_INIT_GENERATE()                                                \
  GENERATE_ACCOUNT(spinner_account);                                                      \
  MAKE_GENESIS_BLOCK(events, blk_0, spinner_account, 1338224400);

//----------------------------------------------------------------------------------------------------------------------
// Tests

bool gen_block_big_major_version::generate(std::vector<test_event_entry>& events) const
{
  BLOCK_VALIDATION_INIT_GENERATE();

  block blk_1;
  generator.construct_block_manually(blk_1, blk_0, spinner_account, test_generator::bf_major_ver, 255);
  events.push_back(blk_1);

  DO_CALLBACK(events, "check_block_purged");

  return true;
}

bool gen_block_big_minor_version::generate(std::vector<test_event_entry>& events) const
{
  BLOCK_VALIDATION_INIT_GENERATE();

  block blk_1;
  generator.construct_block_manually(blk_1, blk_0, spinner_account, test_generator::bf_minor_ver, 0, 255);
  events.push_back(blk_1);

  DO_CALLBACK(events, "check_block_accepted");

  return true;
}

bool gen_block_ts_not_checked::generate(std::vector<test_event_entry>& events) const
{
  BLOCK_VALIDATION_INIT_GENERATE();
  REWIND_BLOCKS_N(events, blk_0r, blk_0, spinner_account, BLOCKCHAIN_TIMESTAMP_CHECK_WINDOW - 2);

  block blk_1;
  generator.construct_block_manually(blk_1, blk_0r, spinner_account, test_generator::bf_timestamp, 0, 0, blk_0.timestamp - 60 * 60);
  events.push_back(blk_1);

  DO_CALLBACK(events, "check_block_accepted");

  return true;
}

bool gen_block_ts_in_past::generate(std::vector<test_event_entry>& events) const
{
  BLOCK_VALIDATION_INIT_GENERATE();
  REWIND_BLOCKS_N(events, blk_0r, blk_0, spinner_account, BLOCKCHAIN_TIMESTAMP_CHECK_WINDOW - 1);

  uint64_t ts_below_median = boost::get<block>(events[BLOCKCHAIN_TIMESTAMP_CHECK_WINDOW / 2 - 1]).timestamp;
  block blk_1;
  generator.construct_block_manually(blk_1, blk_0r, spinner_account, test_generator::bf_timestamp, 0, 0, ts_below_median);
  events.push_back(blk_1);

  DO_CALLBACK(events, "check_block_purged");

  return true;
}

bool gen_block_ts_in_future::generate(std::vector<test_event_entry>& events) const
{
  BLOCK_VALIDATION_INIT_GENERATE();

  block blk_1;
  generator.construct_block_manually(blk_1, blk_0, spinner_account, test_generator::bf_timestamp, 0, 0, time(NULL) + 60*60 + CRYPTONOTE_BLOCK_FUTURE_TIME_LIMIT);
  events.push_back(blk_1);

  DO_CALLBACK(events, "check_block_purged");

  return true;
}

bool gen_block_invalid_prev_id::generate(std::vector<test_event_entry>& events) const
{
  BLOCK_VALIDATION_INIT_GENERATE();

  block blk_1;
  crypto::hash prev_id = get_block_hash(blk_0);
  reinterpret_cast<char &>(prev_id) ^= 1;
  generator.construct_block_manually(blk_1, blk_0, spinner_account, test_generator::bf_prev_id, 0, 0, 0, prev_id);
  events.push_back(blk_1);

  DO_CALLBACK(events, "check_block_purged");

  return true;
}

bool gen_block_invalid_prev_id::check_block_verification_context(const cryptonote::block_verification_context& bvc, size_t event_idx, const cryptonote::block& /*blk*/)
{
  if (1 == event_idx)
    return bvc.m_marked_as_orphaned && !bvc.m_added_to_main_chain && !bvc.m_verifivation_failed;
  else
    return !bvc.m_marked_as_orphaned && bvc.m_added_to_main_chain && !bvc.m_verifivation_failed;
}

bool gen_block_invalid_nonce::generate(std::vector<test_event_entry>& events) const
{
  BLOCK_VALIDATION_INIT_GENERATE();

  std::vector<uint64_t> timestamps;
  std::vector<gyro_type> commulative_gyros;
  if (!lift_up_gyro(events, timestamps, commulative_gyros, generator, 2, blk_0, spinner_account))
    return false;

  // Create invalid nonce
  gyro_type gyr = next_gyro(timestamps, commulative_gyros,GYRO_TARGET_V1);
  assert(1 < gyr);
  const block& blk_last = boost::get<block>(events.back());
  uint64_t timestamp = blk_last.timestamp;
  block blk_3;
  do
  {
    ++timestamp;
    blk_3.spinner_tx.set_null();
    if (!generator.construct_block_manually(blk_3, blk_last, spinner_account,
      test_generator::bf_gyr | test_generator::bf_timestamp, 0, 0, timestamp, crypto::hash(), gyr))
      return false;
  }
  while (0 == blk_3.nonce);
  --blk_3.nonce;
  events.push_back(blk_3);

  return true;
}

bool gen_block_no_spinner_tx::generate(std::vector<test_event_entry>& events) const
{
  BLOCK_VALIDATION_INIT_GENERATE();

  transaction spinner_tx;
  spinner_tx.set_null();

  block blk_1;
  generator.construct_block_manually(blk_1, blk_0, spinner_account, test_generator::bf_spinner_tx, 0, 0, 0, crypto::hash(), 0, spinner_tx);
  events.push_back(blk_1);

  DO_CALLBACK(events, "check_block_purged");

  return true;
}

bool gen_block_unlock_time_is_low::generate(std::vector<test_event_entry>& events) const
{
  BLOCK_VALIDATION_INIT_GENERATE();

  MAKE_SPINNER_TX_MANUALLY(spinner_tx, blk_0);
  --spinner_tx.unlock_time;

  block blk_1;
  generator.construct_block_manually(blk_1, blk_0, spinner_account, test_generator::bf_spinner_tx, 0, 0, 0, crypto::hash(), 0, spinner_tx);
  events.push_back(blk_1);

  DO_CALLBACK(events, "check_block_purged");

  return true;
}

bool gen_block_unlock_time_is_high::generate(std::vector<test_event_entry>& events) const
{
  BLOCK_VALIDATION_INIT_GENERATE();

  MAKE_SPINNER_TX_MANUALLY(spinner_tx, blk_0);
  ++spinner_tx.unlock_time;

  block blk_1;
  generator.construct_block_manually(blk_1, blk_0, spinner_account, test_generator::bf_spinner_tx, 0, 0, 0, crypto::hash(), 0, spinner_tx);
  events.push_back(blk_1);

  DO_CALLBACK(events, "check_block_purged");

  return true;
}

bool gen_block_unlock_time_is_timestamp_in_past::generate(std::vector<test_event_entry>& events) const
{
  BLOCK_VALIDATION_INIT_GENERATE();

  MAKE_SPINNER_TX_MANUALLY(spinner_tx, blk_0);
  spinner_tx.unlock_time = blk_0.timestamp - 10 * 60;

  block blk_1;
  generator.construct_block_manually(blk_1, blk_0, spinner_account, test_generator::bf_spinner_tx, 0, 0, 0, crypto::hash(), 0, spinner_tx);
  events.push_back(blk_1);

  DO_CALLBACK(events, "check_block_purged");

  return true;
}

bool gen_block_unlock_time_is_timestamp_in_future::generate(std::vector<test_event_entry>& events) const
{
  BLOCK_VALIDATION_INIT_GENERATE();

  MAKE_SPINNER_TX_MANUALLY(spinner_tx, blk_0);
  spinner_tx.unlock_time = blk_0.timestamp + 3 * CRYPTONOTE_MINED_MONEY_UNLOCK_WINDOW * GYRO_BLOCKS_ESTIMATE_TIMESPAN;

  block blk_1;
  generator.construct_block_manually(blk_1, blk_0, spinner_account, test_generator::bf_spinner_tx, 0, 0, 0, crypto::hash(), 0, spinner_tx);
  events.push_back(blk_1);

  DO_CALLBACK(events, "check_block_purged");

  return true;
}

bool gen_block_height_is_low::generate(std::vector<test_event_entry>& events) const
{
  BLOCK_VALIDATION_INIT_GENERATE();

  MAKE_SPINNER_TX_MANUALLY(spinner_tx, blk_0);
  boost::get<txin_gen>(spinner_tx.vin[0]).height--;

  block blk_1;
  generator.construct_block_manually(blk_1, blk_0, spinner_account, test_generator::bf_spinner_tx, 0, 0, 0, crypto::hash(), 0, spinner_tx);
  events.push_back(blk_1);

  DO_CALLBACK(events, "check_block_purged");

  return true;
}

bool gen_block_height_is_high::generate(std::vector<test_event_entry>& events) const
{
  BLOCK_VALIDATION_INIT_GENERATE();

  MAKE_SPINNER_TX_MANUALLY(spinner_tx, blk_0);
  boost::get<txin_gen>(spinner_tx.vin[0]).height++;

  block blk_1;
  generator.construct_block_manually(blk_1, blk_0, spinner_account, test_generator::bf_spinner_tx, 0, 0, 0, crypto::hash(), 0, spinner_tx);
  events.push_back(blk_1);

  DO_CALLBACK(events, "check_block_purged");

  return true;
}

bool gen_block_spinner_tx_has_2_tx_gen_in::generate(std::vector<test_event_entry>& events) const
{
  BLOCK_VALIDATION_INIT_GENERATE();

  MAKE_SPINNER_TX_MANUALLY(spinner_tx, blk_0);

  txin_gen in;
  in.height = get_block_height(blk_0) + 1;
  spinner_tx.vin.push_back(in);

  block blk_1;
  generator.construct_block_manually(blk_1, blk_0, spinner_account, test_generator::bf_spinner_tx, 0, 0, 0, crypto::hash(), 0, spinner_tx);
  events.push_back(blk_1);

  DO_CALLBACK(events, "check_block_purged");

  return true;
}

bool gen_block_spinner_tx_has_2_in::generate(std::vector<test_event_entry>& events) const
{
  BLOCK_VALIDATION_INIT_GENERATE();
  REWIND_BLOCKS(events, blk_0r, blk_0, spinner_account);

  GENERATE_ACCOUNT(alice);

  tx_source_entry se;
  se.amount = blk_0.spinner_tx.vout[0].amount;
  se.push_output(0, boost::get<txout_to_key>(blk_0.spinner_tx.vout[0].target).key, se.amount);
  se.real_output = 0;
  se.rct = false;
  se.real_out_tx_key = get_tx_pub_key_from_extra(blk_0.spinner_tx);
  se.real_output_in_tx_index = 0;
  std::vector<tx_source_entry> sources;
  sources.push_back(se);

  tx_destination_entry de;
  de.addr = spinner_account.get_keys().m_account_address;
  de.amount = se.amount;
  std::vector<tx_destination_entry> destinations;
  destinations.push_back(de);

  transaction tmp_tx;
  if (!construct_tx(spinner_account.get_keys(), sources, destinations, boost::none, std::vector<uint8_t>(), tmp_tx, 0))
    return false;

  MAKE_SPINNER_TX_MANUALLY(spinner_tx, blk_0);
  spinner_tx.vin.push_back(tmp_tx.vin[0]);

  block blk_1;
  generator.construct_block_manually(blk_1, blk_0r, spinner_account, test_generator::bf_spinner_tx, 0, 0, 0, crypto::hash(), 0, spinner_tx);
  events.push_back(blk_1);

  DO_CALLBACK(events, "check_block_purged");

  return true;
}

bool gen_block_spinner_tx_with_txin_to_key::generate(std::vector<test_event_entry>& events) const
{
  BLOCK_VALIDATION_INIT_GENERATE();

  // This block has only one output
  block blk_1;
  generator.construct_block_manually(blk_1, blk_0, spinner_account, test_generator::bf_none);
  events.push_back(blk_1);

  REWIND_BLOCKS(events, blk_1r, blk_1, spinner_account);

  tx_source_entry se;
  se.amount = blk_1.spinner_tx.vout[0].amount;
  se.push_output(0, boost::get<txout_to_key>(blk_1.spinner_tx.vout[0].target).key, se.amount);
  se.real_output = 0;
  se.rct = false;
  se.real_out_tx_key = get_tx_pub_key_from_extra(blk_1.spinner_tx);
  se.real_output_in_tx_index = 0;
  std::vector<tx_source_entry> sources;
  sources.push_back(se);

  tx_destination_entry de;
  de.addr = spinner_account.get_keys().m_account_address;
  de.amount = se.amount;
  std::vector<tx_destination_entry> destinations;
  destinations.push_back(de);

  transaction tmp_tx;
  if (!construct_tx(spinner_account.get_keys(), sources, destinations, boost::none, std::vector<uint8_t>(), tmp_tx, 0))
    return false;

  MAKE_SPINNER_TX_MANUALLY(spinner_tx, blk_1);
  spinner_tx.vin[0] = tmp_tx.vin[0];

  block blk_2;
  generator.construct_block_manually(blk_2, blk_1r, spinner_account, test_generator::bf_spinner_tx, 0, 0, 0, crypto::hash(), 0, spinner_tx);
  events.push_back(blk_2);

  DO_CALLBACK(events, "check_block_purged");

  return true;
}

bool gen_block_spinner_tx_out_is_small::generate(std::vector<test_event_entry>& events) const
{
  BLOCK_VALIDATION_INIT_GENERATE();

  MAKE_SPINNER_TX_MANUALLY(spinner_tx, blk_0);
  spinner_tx.vout[0].amount /= 2;

  block blk_1;
  generator.construct_block_manually(blk_1, blk_0, spinner_account, test_generator::bf_spinner_tx, 0, 0, 0, crypto::hash(), 0, spinner_tx);
  events.push_back(blk_1);

  DO_CALLBACK(events, "check_block_purged");

  return true;
}

bool gen_block_spinner_tx_out_is_big::generate(std::vector<test_event_entry>& events) const
{
  BLOCK_VALIDATION_INIT_GENERATE();

  MAKE_SPINNER_TX_MANUALLY(spinner_tx, blk_0);
  spinner_tx.vout[0].amount *= 2;

  block blk_1;
  generator.construct_block_manually(blk_1, blk_0, spinner_account, test_generator::bf_spinner_tx, 0, 0, 0, crypto::hash(), 0, spinner_tx);
  events.push_back(blk_1);

  DO_CALLBACK(events, "check_block_purged");

  return true;
}

bool gen_block_spinner_tx_has_no_out::generate(std::vector<test_event_entry>& events) const
{
  BLOCK_VALIDATION_INIT_GENERATE();

  MAKE_SPINNER_TX_MANUALLY(spinner_tx, blk_0);
  spinner_tx.vout.clear();

  block blk_1;
  generator.construct_block_manually(blk_1, blk_0, spinner_account, test_generator::bf_spinner_tx, 0, 0, 0, crypto::hash(), 0, spinner_tx);
  events.push_back(blk_1);

  DO_CALLBACK(events, "check_block_purged");

  return true;
}

bool gen_block_spinner_tx_has_out_to_alice::generate(std::vector<test_event_entry>& events) const
{
  BLOCK_VALIDATION_INIT_GENERATE();

  GENERATE_ACCOUNT(alice);

  keypair txkey;
  MAKE_SPINNER_TX_AND_KEY_MANUALLY(spinner_tx, blk_0, &txkey);

  crypto::key_derivation derivation;
  crypto::public_key out_eph_public_key;
  crypto::generate_key_derivation(alice.get_keys().m_account_address.m_view_public_key, txkey.sec, derivation);
  crypto::derive_public_key(derivation, 1, alice.get_keys().m_account_address.m_spend_public_key, out_eph_public_key);

  tx_out out_to_alice;
  out_to_alice.amount = spinner_tx.vout[0].amount / 2;
  spinner_tx.vout[0].amount -= out_to_alice.amount;
  out_to_alice.target = txout_to_key(out_eph_public_key);
  spinner_tx.vout.push_back(out_to_alice);

  block blk_1;
  generator.construct_block_manually(blk_1, blk_0, spinner_account, test_generator::bf_spinner_tx, 0, 0, 0, crypto::hash(), 0, spinner_tx);
  events.push_back(blk_1);

  DO_CALLBACK(events, "check_block_accepted");

  return true;
}

bool gen_block_has_invalid_tx::generate(std::vector<test_event_entry>& events) const
{
  BLOCK_VALIDATION_INIT_GENERATE();

  std::vector<crypto::hash> tx_hashes;
  tx_hashes.push_back(crypto::hash());

  block blk_1;
  generator.construct_block_manually_tx(blk_1, blk_0, spinner_account, tx_hashes, 0);
  events.push_back(blk_1);

  DO_CALLBACK(events, "check_block_purged");

  return true;
}

bool gen_block_is_too_big::generate(std::vector<test_event_entry>& events) const
{
  BLOCK_VALIDATION_INIT_GENERATE();

  // Creating a huge spinner_tx, it will have a lot of outs
  MAKE_SPINNER_TX_MANUALLY(spinner_tx, blk_0);
  static const size_t tx_out_count = CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V1 / 2;
  uint64_t amount = get_outs_money_amount(spinner_tx);
  uint64_t portion = amount / tx_out_count;
  uint64_t remainder = amount % tx_out_count;
  txout_target_v target = spinner_tx.vout[0].target;
  spinner_tx.vout.clear();
  for (size_t i = 0; i < tx_out_count; ++i)
  {
    tx_out o;
    o.amount = portion;
    o.target = target;
    spinner_tx.vout.push_back(o);
  }
  if (0 < remainder)
  {
    tx_out o;
    o.amount = remainder;
    o.target = target;
    spinner_tx.vout.push_back(o);
  }

  // Block reward will be incorrect, as it must be reduced if cumulative block size is very big,
  // but in this test it doesn't matter
  block blk_1;
  if (!generator.construct_block_manually(blk_1, blk_0, spinner_account, test_generator::bf_spinner_tx, 0, 0, 0, crypto::hash(), 0, spinner_tx))
    return false;

  events.push_back(blk_1);

  DO_CALLBACK(events, "check_block_purged");

  return true;
}

gen_block_invalid_binary_format::gen_block_invalid_binary_format()
  : m_corrupt_blocks_begin_idx(0)
{
  REGISTER_CALLBACK("check_all_blocks_purged", gen_block_invalid_binary_format::check_all_blocks_purged);
  REGISTER_CALLBACK("corrupt_blocks_boundary", gen_block_invalid_binary_format::corrupt_blocks_boundary);
}

bool gen_block_invalid_binary_format::generate(std::vector<test_event_entry>& events) const
{
  BLOCK_VALIDATION_INIT_GENERATE();

  std::vector<uint64_t> timestamps;
  std::vector<gyro_type> cummulative_gyros;
  gyro_type cummulative_diff = 1;

  // Unlock blk_0 outputs
  block blk_last = blk_0;
  assert(CRYPTONOTE_MINED_MONEY_UNLOCK_WINDOW < GYRO_WINDOW);
  for (size_t i = 0; i < CRYPTONOTE_MINED_MONEY_UNLOCK_WINDOW; ++i)
  {
    MAKE_NEXT_BLOCK(events, blk_curr, blk_last, spinner_account);
    timestamps.push_back(blk_curr.timestamp);
    cummulative_gyros.push_back(++cummulative_diff);
    blk_last = blk_curr;
  }

  // Lifting up takes a while
  gyro_type gyr;
  do
  {
    blk_last = boost::get<block>(events.back());
    gyr = next_gyro(timestamps, cummulative_gyros,GYRO_TARGET_V1);
    if (!lift_up_gyro(events, timestamps, cummulative_gyros, generator, 1, blk_last, spinner_account))
      return false;
    std::cout << "Block #" << events.size() << ", gyro: " << gyr << std::endl;
  }
  while (gyr < 1500);

  blk_last = boost::get<block>(events.back());
  MAKE_TX(events, tx_0, spinner_account, spinner_account, MK_COINS(30), boost::get<block>(events[1]));
  DO_CALLBACK(events, "corrupt_blocks_boundary");

  block blk_test;
  std::vector<crypto::hash> tx_hashes;
  tx_hashes.push_back(get_transaction_hash(tx_0));
  size_t txs_weight = get_transaction_weight(tx_0);
  gyr = next_gyro(timestamps, cummulative_gyros,DURATION_TARGET);
  if (!generator.construct_block_manually(blk_test, blk_last, spinner_account,
    test_generator::bf_gyr | test_generator::bf_timestamp | test_generator::bf_tx_hashes, 0, 0, blk_last.timestamp,
    crypto::hash(), gyr, transaction(), tx_hashes, txs_weight))
    return false;

  blobdata blob = t_serializable_object_to_blob(blk_test);
  for (size_t i = 0; i < blob.size(); ++i)
  {
    for (size_t bit_idx = 0; bit_idx < sizeof(blobdata::value_type) * 8; ++bit_idx)
    {
      serialized_block sr_block(blob);
      blobdata::value_type& ch = sr_block.data[i];
      ch ^= 1 << bit_idx;

      events.push_back(sr_block);
    }
  }

  DO_CALLBACK(events, "check_all_blocks_purged");

  return true;
}

bool gen_block_invalid_binary_format::check_block_verification_context(const cryptonote::block_verification_context& bvc,
                                                                       size_t event_idx, const cryptonote::block& blk)
{
  if (0 == m_corrupt_blocks_begin_idx || event_idx < m_corrupt_blocks_begin_idx)
  {
    return bvc.m_added_to_main_chain;
  }
  else
  {
    return (!bvc.m_added_to_main_chain && (bvc.m_already_exists || bvc.m_marked_as_orphaned || bvc.m_verifivation_failed))
      || (bvc.m_added_to_main_chain && bvc.m_partial_block_reward);
  }
}

bool gen_block_invalid_binary_format::corrupt_blocks_boundary(cryptonote::core& c, size_t ev_index, const std::vector<test_event_entry>& events)
{
  m_corrupt_blocks_begin_idx = ev_index + 1;
  return true;
}

bool gen_block_invalid_binary_format::check_all_blocks_purged(cryptonote::core& c, size_t ev_index, const std::vector<test_event_entry>& events)
{
  DEFINE_TESTS_ERROR_CONTEXT("gen_block_invalid_binary_format::check_all_blocks_purged");

  CHECK_EQ(1, c.get_pool_transactions_count());
  CHECK_EQ(m_corrupt_blocks_begin_idx - 2, c.get_current_blockchain_height());

  return true;
}

bool gen_block_late_v1_coinbase_tx::generate(std::vector<test_event_entry>& events) const
{
  BLOCK_VALIDATION_INIT_GENERATE();

  block blk_1;
  generator.construct_block_manually(blk_1, blk_0, spinner_account,
      test_generator::bf_major_ver | test_generator::bf_minor_ver,
      HF_VERSION_MIN_V2_COINBASE_TX, HF_VERSION_MIN_V2_COINBASE_TX);
  events.push_back(blk_1);

  DO_CALLBACK(events, "check_block_purged");

  return true;
}
