#include <boost/test/unit_test.hpp>
#include <eosio/chain/resource_limits.hpp>
#include <eosio/chain/config.hpp>
#include <eosio/testing/chainbase_fixture.hpp>

#include <algorithm>

using namespace eosio::chain::resource_limits;
using namespace eosio::testing;
using namespace eosio::chain;



class resource_limits_fixture_xos: private chainbase_fixture<512*1024>, public resource_limits_manager
{
   public:
      resource_limits_fixture_xos()
      :chainbase_fixture()
      ,resource_limits_manager(*chainbase_fixture::_db)
      {
         add_indices();
         initialize_database();
      }

      ~resource_limits_fixture_xos() {}

      chainbase::database::session start_session() {
         return chainbase_fixture::_db->start_undo_session(true);
      }
};


BOOST_AUTO_TEST_SUITE(resource_limits_test_xos)

BOOST_FIXTURE_TEST_CASE(check_block_limits_cpu, resource_limits_fixture_xos) try {
      const account_name account(1);
          const uint64_t increment = 1000;
      initialize_account(account);
      set_account_limits(account, increment, increment,  increment);
      process_account_limit_updates();

 uint16_t free_resource_limit_per_day = 100;
//     const auto& chain_config = self.get_global_properties().configuration;
      uint32_t max_virtual_mult = 1000;
      uint64_t CPU_TARGET = EOS_PERCENT(config::default_max_block_cpu_usage, config::default_target_block_cpu_usage_pct);
      set_block_parameters(
         { CPU_TARGET, config::default_max_block_cpu_usage, config::block_cpu_usage_average_window_ms / config::block_interval_ms, max_virtual_mult, {99, 100}, {1000, 999}},
         {EOS_PERCENT(config::default_max_transaction_net_usage, config::default_target_block_net_usage_pct), config::default_max_block_net_usage, config::block_size_average_window_ms / config::block_interval_ms, max_virtual_mult, {99, 100}, {1000, 999}}
      );
     set_block_parameters_xos(free_resource_limit_per_day
      );


      const uint64_t expected_iterations = config::default_max_block_cpu_usage / increment;

      for (int idx = 0; idx < expected_iterations; idx++) {
         add_transaction_usage({account}, increment, 0, 0);
      }
      int64_t r =0;
      int64_t n =0;
      int64_t c =0;
  
     std::tie(r,n,c)=   check_account_limits_xos(account,r,n,c);
      BOOST_REQUIRE_EQUAL(n, 0);
      BOOST_REQUIRE_THROW(add_transaction_usage({account}, increment, 0, 0), block_resource_exhausted);

   } FC_LOG_AND_RETHROW();

BOOST_FIXTURE_TEST_CASE(check_block_limits_cpu_lowerthan, resource_limits_fixture_xos) try {
      const account_name account(1);
          const uint64_t increment = 1000;
      initialize_account(account);
      set_account_limits(account, increment, increment,  increment);
      process_account_limit_updates();
      
 uint16_t free_resource_limit_per_day = 300;
//     const auto& chain_config = self.get_global_properties().configuration;
      uint32_t max_virtual_mult = 1000;
      uint64_t CPU_TARGET = EOS_PERCENT(config::default_max_block_cpu_usage, config::default_target_block_cpu_usage_pct);
      set_block_parameters(
         { CPU_TARGET, config::default_max_block_cpu_usage, config::block_cpu_usage_average_window_ms / config::block_interval_ms, max_virtual_mult, {99, 100}, {1000, 999}},
         {EOS_PERCENT(config::default_max_transaction_net_usage, config::default_target_block_net_usage_pct), config::default_max_block_net_usage, config::block_size_average_window_ms / config::block_interval_ms, max_virtual_mult, {99, 100}, {1000, 999}}
        );

     set_block_parameters_xos(free_resource_limit_per_day
      );

      const uint64_t expected_iterations = config::default_max_block_cpu_usage / increment;

      for (int idx = 0; idx < expected_iterations; idx++) {
         add_transaction_usage({account}, increment, 0, 0);
      }
      int64_t r =0;
      int64_t n =0;
      int64_t c =0;
  
     std::tie(r,n,c)=   check_account_limits_xos(account,r,n,c);
      BOOST_REQUIRE_EQUAL(r, 0);
      BOOST_REQUIRE_EQUAL(n, -1);
      BOOST_REQUIRE_EQUAL(c, -1);
      BOOST_REQUIRE_THROW(add_transaction_usage({account}, increment, 0, 0), block_resource_exhausted);

   } FC_LOG_AND_RETHROW();


BOOST_AUTO_TEST_SUITE_END()
