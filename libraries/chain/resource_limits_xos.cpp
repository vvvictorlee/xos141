#include <eosio/chain/exceptions.hpp>
#include <eosio/chain/resource_limits.hpp>
#include <eosio/chain/resource_limits_private.hpp>
#include <eosio/chain/transaction_metadata.hpp>
#include <eosio/chain/transaction.hpp>
#include <boost/tuple/tuple_io.hpp>
#include <eosio/chain/database_utils.hpp>
#include <algorithm>
#include <eosio/chain/resource_limits_private_xos.hpp>


namespace eosio { namespace chain { namespace resource_limits {

void resource_limits_manager::initialize_database_xos() {
  
  const auto &per_day_config = _db.create<resource_limits_per_day_config_object>([](resource_limits_per_day_config_object &config) {
    // see default settings in the declaration
  });
  
}

void resource_limits_manager::initialize_account_xos(const account_name& account) {

  _db.create<resource_usage_per_day_object>([&](resource_usage_per_day_object &bu) {
    bu.owner = account;
  });
}

void resource_limits_manager::set_block_parameters_xos(uint16_t free_resource_limit_per_day)
{
  const auto &per_day_config = _db.get<resource_limits_per_day_config_object>();
  _db.modify(per_day_config, [&](resource_limits_per_day_config_object &c) {
    c.free_resource_limit_per_day = free_resource_limit_per_day;
  });
}


void resource_limits_manager::add_transaction_usage_xos(const account_name& account ) {
      const auto &usage_per_day = _db.get<resource_usage_per_day_object, by_owner>(account);
    const uint64_t TX_ONE = 1;
   
    _db.modify(usage_per_day, [&](auto &bu) {
 
      bu.tx_usage.add(TX_ONE);
    });
  
}

std::tuple<int64_t, int64_t, int64_t> resource_limits_manager::check_account_limits_xos(const account_name &account, int64_t ram_bytes, int64_t net_weight, int64_t cpu_weight) const
{
  const auto &config = _db.get<resource_limits_per_day_config_object>();

  const auto &usage_per_day = _db.get<resource_usage_per_day_object, by_owner>(account);

  return std::make_tuple(ram_bytes,

                         usage_per_day.tx_usage.validate(config.free_resource_limit_per_day, net_weight),

                         usage_per_day.tx_usage.validate(config.free_resource_limit_per_day, cpu_weight));
}


void resource_limits_manager::get_account_limits_xos( const account_name& account, int64_t& ram_bytes, int64_t& net_weight, int64_t& cpu_weight ) const {
 get_account_limits(   account, ram_bytes, net_weight,  cpu_weight );

   std::tie(ram_bytes, net_weight, cpu_weight) = check_account_limits_xos(account,ram_bytes, net_weight, cpu_weight);
}

} } } /// eosio::chain::resource_limits
