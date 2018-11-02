/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once
#include <fc/uint128.hpp>
#include <fc/array.hpp>

#include <eosio/chain/types.hpp>
#include <eosio/chain/block_timestamp.hpp>
#include <eosio/chain/chain_config.hpp>
#include <eosio/chain/producer_schedule.hpp>
#include <eosio/chain/incremental_merkle.hpp>
#include <chainbase/chainbase.hpp>
#include "multi_index_includes.hpp"
#include <eosio/chain/global_property_object_xos.hpp>
#include <boost/algorithm/string.hpp>

namespace eosio { namespace chain {

using account_name_list = flat_set<account_name>;

static const  vector<string> config_list_type_string= {
              "actor_blacklist",
              "contract_blacklist",
              "resource_greylist"};

      enum  config_list_type:uint16_t {
              actor_blacklist_type=0, 
              contract_blacklist_type,
              resource_greylist_type,  	
               LIST_TYPE_COUNT ///< Sentry value which contains the number of different List types  
              // actor_whitelist,
              // contract_whitelist,
              // action_blacklist,
              // key_blacklist,
              // trusted_producers,
                };

         struct chain_list_config {
            // flat_set<account_name>   actor_whitelist;
            // flat_set<account_name>   actor_blacklist;
            // flat_set<account_name>   contract_whitelist;
            // flat_set<account_name>   contract_blacklist;
            // flat_set< pair<account_name, action_name> > action_blacklist;
            // flat_set<public_key_type> key_blacklist;
            
            // flat_set<account_name>   resource_greylist;
            // flat_set<account_name>   trusted_producers;
            flat_set<account_name>   list;
         };

 class global_property_list_object : public chainbase::object<global_property_list_object_type, global_property_list_object>
   {
       OBJECT_CTOR(global_property_list_object, (list_configuration))
      id_type                           id;
      uint16_t list_type;
      shared_string  list_configuration;
 
 void add_list( const chain_list_config& a ) {
  const chain_list_config& b = get_list();
   chain_list_config c;
set_union(a.list.begin(),a.list.end(),b.list.begin(),b.list.end(),inserter(c.list,c.list.begin()));
          set_list(c);
      }
 void remove_list( const chain_list_config& b ) {
  const chain_list_config& a = get_list();
   chain_list_config c;
set_difference(a.list.begin(),a.list.end(),b.list.begin(),b.list.end(),inserter(c.list,c.list.begin()));
          set_list(c);
      }

void add_string_list( const std::string& list ) {
   chain_list_config a;
  a.list== parse_name_string(list);
          add_list(a);
      }

void remove_string_list( const std::string& list ) {
   chain_list_config a;
  a.list== parse_name_string(list);
          remove_list(a);
      }



account_name_list parse_name_string(string items)
{
  account_name_list item_list;
vector<string> itemlist;
      boost::split(itemlist, items, boost::is_any_of(","));
      for (string item: itemlist) {
 
         item_list.insert(string_to_name(item.c_str()));
      }

      return item_list;
}

      void set_list( const chain_list_config& a ) {
         list_configuration.resize( fc::raw::pack_size( a ) );
         fc::datastream<char*> ds( list_configuration.data(), list_configuration.size() );
         fc::raw::pack( ds, a );
      }

      chain_list_config get_list()const {
         chain_list_config a;
        //  EOS_ASSERT( list_configuration.size() != 0, abi_not_found_exception, "No ABI set on account ${n}", ("n",name) );

         fc::datastream<const char*> ds( list_configuration.data(), list_configuration.size() );
         fc::raw::unpack( ds, a );
         return a;
      }
   };


 using global_property_list_multi_index = chainbase::shared_multi_index_container<
      global_property_list_object,
      indexed_by<
         ordered_unique<tag<by_id>,
            BOOST_MULTI_INDEX_MEMBER(global_property_list_object, global_property_list_object::id_type, id),
        ordered_unique<tag<by_type>,
            BOOST_MULTI_INDEX_MEMBER(global_property_list_object, global_property_list_object::uint16_t, list_type)
         >
      >
   >;
  

   class global_property_object_ex : public chainbase::object<global_property_object_ex_type, global_property_object_ex>
   {
       OBJECT_CTOR(global_property_object_ex)
      id_type                           id;
      uint16_t free_resource_limit_per_day;
   };


 using global_property_list_multi_index = chainbase::shared_multi_index_container<
      global_property_object_ex,
      indexed_by<
         ordered_unique<tag<by_id>,
            BOOST_MULTI_INDEX_MEMBER(global_property_object_ex, global_property_object_ex::id_type, id)
         >
      >
   >;


}}


CHAINBASE_SET_INDEX_TYPE(eosio::chain::global_property_list_object, eosio::chain::global_property_list_multi_index)
CHAINBASE_SET_INDEX_TYPE(eosio::chain::global_property_object_ex, eosio::chain::global_property_ex_multi_index)
FC_REFLECT(eosio::chain::global_property_list_object,
           (list_type)(list_configuration)
          )
FC_REFLECT(eosio::chain::global_property_object_ex,
           (free_resource_limit_per_day)
          )

FC_REFLECT(eosio::chain::chain_list_config,
           (list)
          )