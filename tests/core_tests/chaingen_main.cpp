// Copyright (c) 2012-2013 The Cryptonote developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chaingen.h"
#include "chaingen_tests_list.h"
#include "common/command_line.h"
#include "transaction_tests.h"
#include "../test_genesis_config.h"
#include "crypto/hash_options.h"

namespace po = boost::program_options;

namespace
{
  const command_line::arg_descriptor<std::string> arg_test_data_path              = {"test_data_path", "", ""};
  const command_line::arg_descriptor<bool>        arg_generate_test_data          = {"generate_test_data", ""};
  const command_line::arg_descriptor<bool>        arg_play_test_data              = {"play_test_data", ""};
  const command_line::arg_descriptor<bool>        arg_generate_and_play_test_data = {"generate_and_play_test_data", ""};
  const command_line::arg_descriptor<bool>        arg_test_transactions           = {"test_transactions", ""};
  const command_line::arg_descriptor<bool>        arg_stop_on_fail                = {"stop_on_fail", ""};
}

int main(int argc, char* argv[])
{
  TRY_ENTRY();
  
  set_test_genesis_config();
  
  epee::string_tools::set_module_name_and_folder(argv[0]);

  //set up logging options
  epee::log_space::get_set_log_detalisation_level(true, LOG_LEVEL_3);
  epee::log_space::log_singletone::add_logger(LOGGER_CONSOLE, NULL, NULL, LOG_LEVEL_2);
  
  epee::log_space::log_singletone::add_logger(LOGGER_FILE, 
    epee::log_space::log_singletone::get_default_log_file().c_str(), 
    epee::log_space::log_singletone::get_default_log_folder().c_str());

  po::options_description desc_options("Allowed options");
  command_line::add_arg(desc_options, command_line::arg_help);
  command_line::add_arg(desc_options, arg_test_data_path);
  command_line::add_arg(desc_options, arg_generate_test_data);
  command_line::add_arg(desc_options, arg_play_test_data);
  command_line::add_arg(desc_options, arg_generate_and_play_test_data);
  command_line::add_arg(desc_options, arg_test_transactions);
  command_line::add_arg(desc_options, arg_stop_on_fail);

  po::variables_map vm;
  bool r = command_line::handle_error_helper(desc_options, [&]()
  {
    po::store(po::parse_command_line(argc, argv, desc_options), vm);
    po::notify(vm);
    return true;
  });
  if (!r)
    return 1;

  if (command_line::get_arg(vm, command_line::arg_help))
  {
    std::cout << desc_options << std::endl;
    return 0;
  }

  // testing params
  g_hash_ops_small_boulderhash = true;
  cryptonote::config::do_boulderhash = true;
  cryptonote::config::no_reward_ramp = true;
  cryptonote::config::test_serialize_unserialize_block = true;
  cryptonote::config::dpos_num_delegates = 5;
  DEFAULT_FEE = 0; // add no-fee txs, lot of tests use them
  
  crypto::g_boulderhash_state = crypto::pc_malloc_state();
  
  size_t tests_count = 0;
  std::vector<std::string> failed_tests;
  std::string tests_folder = command_line::get_arg(vm, arg_test_data_path);
  if (command_line::get_arg(vm, arg_generate_test_data))
  {
    GENERATE("chain001.dat", gen_simple_chain_001);
  }
  else if (command_line::get_arg(vm, arg_play_test_data))
  {
    PLAY("chain001.dat", gen_simple_chain_001);
  }
  else if (command_line::get_arg(vm, arg_generate_and_play_test_data))
  {
    bool stop_on_fail = command_line::get_arg(vm, arg_stop_on_fail);
    
    GENERATE_AND_PLAY(gen_dpos_register);
    GENERATE_AND_PLAY(gen_dpos_register_invalid_id);
    GENERATE_AND_PLAY(gen_dpos_register_invalid_id_2);
    GENERATE_AND_PLAY(gen_dpos_register_invalid_address);
    GENERATE_AND_PLAY(gen_dpos_register_low_fee);
    GENERATE_AND_PLAY(gen_dpos_register_low_fee_2);
    
    GENERATE_AND_PLAY(gen_dpos_vote);
    
    GENERATE_AND_PLAY(gen_dpos_switch_to_dpos);
    GENERATE_AND_PLAY(gen_dpos_altchain_voting);
    GENERATE_AND_PLAY(gen_dpos_altchain_voting_invalid);
    GENERATE_AND_PLAY(gen_dpos_limit_delegates);
    GENERATE_AND_PLAY(gen_dpos_unapply_votes);
    GENERATE_AND_PLAY(gen_dpos_limit_votes);
    GENERATE_AND_PLAY(gen_dpos_change_votes);
    GENERATE_AND_PLAY(gen_dpos_spend_votes);
    GENERATE_AND_PLAY(gen_dpos_vote_tiebreaker);
    GENERATE_AND_PLAY(gen_dpos_delegate_timeout);
    GENERATE_AND_PLAY(gen_dpos_delegate_timeout_2);
    GENERATE_AND_PLAY(gen_dpos_timestamp_checks);
    GENERATE_AND_PLAY(gen_dpos_invalid_votes);
    GENERATE_AND_PLAY(gen_dpos_receives_fees);
    GENERATE_AND_PLAY(gen_dpos_altchain_voting_2);
    GENERATE_AND_PLAY(gen_dpos_altchain_voting_3);
    GENERATE_AND_PLAY(gen_dpos_altchain_voting_4);
    
    /*// Contract chain-switch
    GENERATE_AND_PLAY(gen_chainswitch_txin_to_key);
    GENERATE_AND_PLAY(gen_chainswitch_contract_create_id_1);
    GENERATE_AND_PLAY(gen_chainswitch_contract_create_id_2);
    GENERATE_AND_PLAY(gen_chainswitch_contract_create_id_3);
    GENERATE_AND_PLAY(gen_chainswitch_contract_create_id_4);
    GENERATE_AND_PLAY(gen_chainswitch_contract_create_descr_1);
    GENERATE_AND_PLAY(gen_chainswitch_contract_create_descr_2);
    GENERATE_AND_PLAY(gen_chainswitch_contract_create_descr_3);
    GENERATE_AND_PLAY(gen_chainswitch_contract_create_descr_4);
    GENERATE_AND_PLAY(gen_chainswitch_contract_create_send);
    GENERATE_AND_PLAY(gen_chainswitch_contract_grade);
    
    // Contracts
    GENERATE_AND_PLAY(gen_contracts_create);
    GENERATE_AND_PLAY(gen_contracts_create_mint);
    GENERATE_AND_PLAY(gen_contracts_grade);
    GENERATE_AND_PLAY(gen_contracts_grade_with_fee);
    GENERATE_AND_PLAY(gen_contracts_grade_spend_backing);
    GENERATE_AND_PLAY(gen_contracts_grade_spend_backing_cant_overspend_misgrade);
    GENERATE_AND_PLAY(gen_contracts_grade_spend_contract);
    GENERATE_AND_PLAY(gen_contracts_grade_send_then_spend_contract);
    GENERATE_AND_PLAY(gen_contracts_grade_cant_send_and_spend_contract_1);
    GENERATE_AND_PLAY(gen_contracts_grade_cant_send_and_spend_contract_2);
    GENERATE_AND_PLAY(gen_contracts_grade_spend_contract_cant_overspend_misgrade);
    GENERATE_AND_PLAY(gen_contracts_grade_spend_fee);
    GENERATE_AND_PLAY(gen_contracts_grade_spend_fee_cant_overspend_misgrade);
    GENERATE_AND_PLAY(gen_contracts_grade_spend_all_rounding);
    GENERATE_AND_PLAY(gen_contracts_resolve_backing_cant_change_contract);
    GENERATE_AND_PLAY(gen_contracts_resolve_backing_expired);
    GENERATE_AND_PLAY(gen_contracts_resolve_contract_cant_change_contract);
    GENERATE_AND_PLAY(gen_contracts_resolve_contract_expired);

    GENERATE_AND_PLAY(gen_contracts_extra_currency_checks);
    GENERATE_AND_PLAY(gen_contracts_create_checks);
    GENERATE_AND_PLAY(gen_contracts_mint_checks);
    GENERATE_AND_PLAY(gen_contracts_grade_checks);
    GENERATE_AND_PLAY(gen_contracts_resolve_backing_checks);
    GENERATE_AND_PLAY(gen_contracts_resolve_contract_checks);
    
    GENERATE_AND_PLAY(gen_contracts_create_mint_fuse);
    GENERATE_AND_PLAY(gen_contracts_create_mint_fuse_fee);
    GENERATE_AND_PLAY(gen_contracts_create_mint_fuse_checks);
    
    // Sub-currencies
    GENERATE_AND_PLAY(gen_chainswitch_mint);
    GENERATE_AND_PLAY(gen_chainswitch_mint_2);
    GENERATE_AND_PLAY(gen_chainswitch_mint_3);
    GENERATE_AND_PLAY(gen_chainswitch_remint);
    GENERATE_AND_PLAY(gen_chainswitch_remint_2);
    
    GENERATE_AND_PLAY(gen_currency_mint);
    GENERATE_AND_PLAY(gen_currency_mint_many);
    GENERATE_AND_PLAY(gen_currency_invalid_amount_0);
    GENERATE_AND_PLAY(gen_currency_invalid_currency_0);
    GENERATE_AND_PLAY(gen_currency_invalid_currency_70);
    GENERATE_AND_PLAY(gen_currency_invalid_currency_255);
    GENERATE_AND_PLAY(gen_currency_invalid_large_description);
    GENERATE_AND_PLAY(gen_currency_invalid_reuse_currency_1);
    GENERATE_AND_PLAY(gen_currency_invalid_reuse_currency_2);
    GENERATE_AND_PLAY(gen_currency_invalid_reuse_description_1);
    GENERATE_AND_PLAY(gen_currency_invalid_reuse_description_2);
    GENERATE_AND_PLAY(gen_currency_invalid_remint_key);
    GENERATE_AND_PLAY(gen_currency_invalid_spend_more_than_mint);
    GENERATE_AND_PLAY(gen_currency_ok_spend_less_than_mint);
    
    GENERATE_AND_PLAY(gen_currency_spend_currency);
    GENERATE_AND_PLAY(gen_currency_cant_spend_other_currency);
    GENERATE_AND_PLAY(gen_currency_spend_currency_mix);
    
    GENERATE_AND_PLAY(gen_currency_remint_valid);
    GENERATE_AND_PLAY(gen_currency_remint_invalid_amount_0);
    GENERATE_AND_PLAY(gen_currency_remint_invalid_unremintable);
    GENERATE_AND_PLAY(gen_currency_remint_invalid_currency);
    GENERATE_AND_PLAY(gen_currency_remint_invalid_new_remint_key);
    GENERATE_AND_PLAY(gen_currency_remint_invalid_signature);
    GENERATE_AND_PLAY(gen_currency_remint_invalid_spend_more_than_remint);
    GENERATE_AND_PLAY(gen_currency_remint_twice);
    GENERATE_AND_PLAY(gen_currency_remint_change_key_old_invalid);
    GENERATE_AND_PLAY(gen_currency_remint_change_key_remint);
    GENERATE_AND_PLAY(gen_currency_remint_can_remint_twice_per_block);
    GENERATE_AND_PLAY(gen_currency_remint_cant_mint_remint_same_block);
    GENERATE_AND_PLAY(gen_currency_remint_limit_uint64max);*/
    
    // Vanilla:
    GENERATE_AND_PLAY(gen_simple_chain_001);
    GENERATE_AND_PLAY(gen_simple_chain_split_1);
    GENERATE_AND_PLAY(one_block);
    GENERATE_AND_PLAY(gen_chain_switch_1);
    GENERATE_AND_PLAY(gen_chainswitch_invalid_1);
    GENERATE_AND_PLAY(gen_chainswitch_invalid_2);
    GENERATE_AND_PLAY(gen_ring_signature_1);
    GENERATE_AND_PLAY(gen_ring_signature_2);
    //GENERATE_AND_PLAY(gen_ring_signature_big); // Takes up to XXX hours (if CRYPTONOTE_MINED_MONEY_UNLOCK_WINDOW == 10)

    // Block verification tests
    GENERATE_AND_PLAY(gen_block_big_major_version);
    GENERATE_AND_PLAY(gen_block_big_minor_version);
    GENERATE_AND_PLAY(gen_block_ts_not_checked);
    GENERATE_AND_PLAY(gen_block_ts_in_past);
    GENERATE_AND_PLAY(gen_block_ts_in_future);
    GENERATE_AND_PLAY(gen_block_invalid_prev_id);
    GENERATE_AND_PLAY(gen_block_invalid_nonce);
    GENERATE_AND_PLAY(gen_block_no_miner_tx);
    GENERATE_AND_PLAY(gen_block_unlock_time_is_low);
    GENERATE_AND_PLAY(gen_block_unlock_time_is_high);
    GENERATE_AND_PLAY(gen_block_unlock_time_is_timestamp_in_past);
    GENERATE_AND_PLAY(gen_block_unlock_time_is_timestamp_in_future);
    GENERATE_AND_PLAY(gen_block_height_is_low);
    GENERATE_AND_PLAY(gen_block_height_is_high);
    GENERATE_AND_PLAY(gen_block_miner_tx_has_2_tx_gen_in);
    GENERATE_AND_PLAY(gen_block_miner_tx_has_2_in);
    GENERATE_AND_PLAY(gen_block_miner_tx_with_txin_to_key);
    GENERATE_AND_PLAY(gen_block_miner_tx_out_is_small);
    GENERATE_AND_PLAY(gen_block_miner_tx_out_is_big);
    GENERATE_AND_PLAY(gen_block_miner_tx_has_no_out);
    GENERATE_AND_PLAY(gen_block_miner_tx_has_out_to_alice);
    GENERATE_AND_PLAY(gen_block_has_invalid_tx);
    GENERATE_AND_PLAY(gen_block_is_too_big);
    //GENERATE_AND_PLAY(gen_block_invalid_binary_format); // Takes up to 3 hours, if CRYPTONOTE_MINED_MONEY_UNLOCK_WINDOW == 500, up to 30 minutes, if CRYPTONOTE_MINED_MONEY_UNLOCK_WINDOW == 10

    // Transaction verification tests
    GENERATE_AND_PLAY(gen_tx_big_version);
    GENERATE_AND_PLAY(gen_tx_unlock_time);
    GENERATE_AND_PLAY(gen_tx_input_is_not_txin_to_key);
    GENERATE_AND_PLAY(gen_tx_no_inputs_no_outputs);
    GENERATE_AND_PLAY(gen_tx_no_inputs_has_outputs);
    GENERATE_AND_PLAY(gen_tx_has_inputs_no_outputs);
    GENERATE_AND_PLAY(gen_tx_invalid_input_amount);
    GENERATE_AND_PLAY(gen_tx_input_wo_key_offsets);
    GENERATE_AND_PLAY(gen_tx_sender_key_offest_not_exist);
    GENERATE_AND_PLAY(gen_tx_key_offest_points_to_foreign_key);
    GENERATE_AND_PLAY(gen_tx_mixed_key_offest_not_exist);
    GENERATE_AND_PLAY(gen_tx_key_image_not_derive_from_tx_key);
    GENERATE_AND_PLAY(gen_tx_key_image_is_invalid);
    GENERATE_AND_PLAY(gen_tx_check_input_unlock_time);
    GENERATE_AND_PLAY(gen_tx_txout_to_key_has_invalid_key);
    GENERATE_AND_PLAY(gen_tx_output_with_zero_amount);
    GENERATE_AND_PLAY(gen_tx_output_is_not_txout_to_key);
    GENERATE_AND_PLAY(gen_tx_signatures_are_invalid);

    // Double spend
    GENERATE_AND_PLAY(gen_double_spend_in_tx<false>);
    GENERATE_AND_PLAY(gen_double_spend_in_tx<true>);
    GENERATE_AND_PLAY(gen_double_spend_in_the_same_block<false>);
    GENERATE_AND_PLAY(gen_double_spend_in_the_same_block<true>);
    GENERATE_AND_PLAY(gen_double_spend_in_different_blocks<false>);
    GENERATE_AND_PLAY(gen_double_spend_in_different_blocks<true>);
    GENERATE_AND_PLAY(gen_double_spend_in_different_chains);
    GENERATE_AND_PLAY(gen_double_spend_in_alt_chain_in_the_same_block<false>);
    GENERATE_AND_PLAY(gen_double_spend_in_alt_chain_in_the_same_block<true>);
    GENERATE_AND_PLAY(gen_double_spend_in_alt_chain_in_different_blocks<false>);
    GENERATE_AND_PLAY(gen_double_spend_in_alt_chain_in_different_blocks<true>);

    GENERATE_AND_PLAY(gen_uint_overflow_1);
    GENERATE_AND_PLAY(gen_uint_overflow_2);

    //GENERATE_AND_PLAY(gen_block_reward); // Takes a while

    std::cout << (failed_tests.empty() ? concolor::green : concolor::magenta);
    std::cout << "\nREPORT:\n";
    std::cout << "  Test run: " << tests_count << '\n';
    std::cout << "  Failures: " << failed_tests.size() << '\n';
    if (!failed_tests.empty())
    {
      std::cout << "FAILED TESTS:\n";
      BOOST_FOREACH(auto test_name, failed_tests)
      {
        std::cout << "  " << test_name << '\n';
      }
    }
    std::cout << concolor::normal << std::endl;
  }
  else if (command_line::get_arg(vm, arg_test_transactions))
  {
    CALL_TEST("TRANSACTIONS TESTS", test_transactions);
  }
  else
  {
    std::cout << concolor::magenta << "Wrong arguments" << concolor::normal << std::endl;
    std::cout << desc_options << std::endl;
    crypto::pc_free_state(crypto::g_boulderhash_state);
    return 2;
  }

  crypto::pc_free_state(crypto::g_boulderhash_state);
  return failed_tests.empty() ? 0 : 1;

  CATCH_ENTRY_L0("main", 1);
}