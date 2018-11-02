#include <eosio/chain/controller.hpp>
#include <eosio/chain/transaction_context.hpp>

#include <eosio/chain/block_log.hpp>
#include <eosio/chain/fork_database.hpp>
#include <eosio/chain/exceptions.hpp>

#include <eosio/chain/account_object.hpp>
#include <eosio/chain/block_summary_object.hpp>
#include <eosio/chain/global_property_object.hpp>
#include <eosio/chain/contract_table_objects.hpp>
#include <eosio/chain/generated_transaction_object.hpp>
#include <eosio/chain/transaction_object.hpp>
#include <eosio/chain/reversible_block_object.hpp>

#include <eosio/chain/authorization_manager.hpp>
#include <eosio/chain/resource_limits.hpp>
#include <eosio/chain/chain_snapshot.hpp>

#include <chainbase/chainbase.hpp>
#include <fc/io/json.hpp>
#include <fc/scoped_exit.hpp>

#include <fc/variant_object.hpp>

#include <eosio/chain/eosio_contract.hpp>

namespace eosio { namespace chain {

using resource_limits::resource_limits_manager;

const global_property_list_object& controller::get_global_properties_list(uint16_t type)const {
    const auto &property_list = my->db.get<global_property_list_object, by_type>(type);
    return property_list;
}

const global_property_object_ex& controller::get_global_properties_ex()const {
  return my->db.get<global_property_object_ex>();
}


 void controller::add_config_list(uint16_t type,const account_name_list& list) {
      my->db.add_list_xos(type,list);

 }

// #define STR2ENUM(s)  ((##s)_type)
 void controller::set_config_string_list(string& type,string& action,const string& list) {
   
     uint16_t listtype=STR2ENUM(type);
     const string actiontype="add";
     if(action.compare(actiontype)==0)
     {
       my->db.add_string_list_xos(listtype,list);
     }
     else
     {
       my->db.remove_string_list_xos(listtype,list);
     }
 }


} } /// eosio::chain
