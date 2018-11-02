#pragma once
#include <eosio/chain/types.hpp>
#include <eosio/chain/config.hpp>
#include <eosio/chain/exceptions.hpp>

#include "multi_index_includes.hpp"


namespace eosio { namespace chain { namespace resource_limits {

   namespace impl {
      

struct resource_usage_per_day_accumulator
{

        resource_usage_per_day_accumulator()
            : current_time(0),current_day(0),usage_per_day(0)
        {
        }
        int64_t current_time = 0;   // current time
        uint32_t current_day = 0;   // current day 0 hour 0 minute 0 second
        uint64_t usage_per_day = 0; //  usage per day
        void add(uint64_t units)
        {
                static constexpr uint32_t seconds_of_day = 24 * 60 * 60;
                int64_t nowtime = fc::time_point::now().time_since_epoch().count();
                EOS_ASSERT(nowtime > current_time, resource_limit_exception, "new time cannot be less than the previous time");
                current_time = nowtime;
                uint32_t now = fc::time_point::now().sec_since_epoch();
                uint32_t currentday = now / seconds_of_day * seconds_of_day;

                if (current_day != currentday)
                {
                        current_day = currentday;
                        usage_per_day = 0;
                }

                usage_per_day += units;
        }

        int64_t validate(uint64_t threshold, int64_t weight) const
        {
                return usage_per_day <= threshold ? -1 : weight;
        }
};

} // namespace impl

using usage_per_day_accumulator = impl::resource_usage_per_day_accumulator;

struct resource_usage_per_day_object : public chainbase::object<resource_usage_per_day_object_type, resource_usage_per_day_object>
{
        OBJECT_CTOR(resource_usage_per_day_object)

        id_type id;
        account_name owner;

        // usage_per_day_accumulator net_usage;
        // usage_per_day_accumulator cpu_usage;
        // usage_per_day_accumulator ram_usage;

        usage_per_day_accumulator tx_usage;
};

using resource_usage_per_day_index = chainbase::shared_multi_index_container<
    resource_usage_per_day_object,
    indexed_by<
        ordered_unique<tag<by_id>, member<resource_usage_per_day_object, resource_usage_per_day_object::id_type, &resource_usage_per_day_object::id>>,
        ordered_unique<tag<by_owner>, member<resource_usage_per_day_object, account_name, &resource_usage_per_day_object::owner>>>>;
   
class resource_limits_per_day_config_object : public chainbase::object<resource_limits_per_day_config_object_type, resource_limits_per_day_config_object>
{
        OBJECT_CTOR(resource_limits_per_day_config_object);
        id_type id;

        //       static_assert( config::block_interval_ms > 0, "config::block_interval_ms must be positive" );
        // elastic_per_day_limit_parameters cpu_limit_parameters = {{100, 10}, {200, 20}, {300, 50}};
        // elastic_per_day_limit_parameters net_limit_parameters = {{1024, 100}, {2048, 1000}, {4096, 2000}};
        // elastic_per_day_limit_parameters ram_limit_parameters = {{1024, 100}, {2048, 1000}, {4096, 2000}};
        uint16_t free_resource_limit_per_day = config::default_free_resource_limit_per_day;
};

using resource_limits_per_day_config_index = chainbase::shared_multi_index_container<
    resource_limits_per_day_config_object,
    indexed_by<
        ordered_unique<tag<by_id>, member<resource_limits_per_day_config_object, resource_limits_per_day_config_object::id_type, &resource_limits_per_day_config_object::id>>>>;

} } } /// eosio::chain::resource_limits

CHAINBASE_SET_INDEX_TYPE(eosio::chain::resource_limits::resource_usage_per_day_object, eosio::chain::resource_limits::resource_usage_per_day_index)
CHAINBASE_SET_INDEX_TYPE(eosio::chain::resource_limits::resource_limits_per_day_config_object, eosio::chain::resource_limits::resource_limits_per_day_config_index)

FC_REFLECT(eosio::chain::resource_limits::usage_per_day_accumulator, (current_time)(current_day)(usage_per_day))
FC_REFLECT(eosio::chain::resource_limits::resource_usage_per_day_object,  (owner)(tx_usage))
FC_REFLECT(eosio::chain::resource_limits::resource_limits_per_day_config_object, (free_resource_limit_per_day))
