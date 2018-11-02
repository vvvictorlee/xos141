/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include <eosio/testing/tester.hpp>
#include <eosio/chain/global_property_object.hpp>
#include <fc/crypto/digest.hpp>
#include <boost/container/flat_set.hpp>
#include <eosio/chain/types.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#ifdef NON_VALIDATING_TEST
#define TESTER tester
#else
#define TESTER validating_tester
#endif


using namespace eosio::chain;
using namespace eosio::testing;
namespace bfs = boost::filesystem;

BOOST_AUTO_TEST_SUITE(database_tests_xos)

flat_set<account_name> parse_list_string(string items)
{
  flat_set<account_name> item_list;
vector<string> itemlist;
      boost::split(itemlist, items, boost::is_any_of(","));
      for (string item: itemlist) {
      
        //  try {
        //     node = boost::lexical_cast<uint16_t,string>(node_number);
        //  }
        //  catch(boost::bad_lexical_cast &) {
        //     // This exception will be handled below
        //  }
        //  if (node < 0 || node > 99) {
        //     cerr << "Bad node number found in node number list: " << node_number << endl;
        //     exit(-1);
        //  }
         item_list.insert(string_to_name(item.c_str()));
      }

      return item_list;
}


    // Simple tests of undo infrastructure
   BOOST_AUTO_TEST_CASE(list_config_parse_test) {
      try {
         TESTER test;
      //  // Bypass read-only restriction on state DB access for this unit test which really needs to mutate the DB to properly conduct its test.
      //    eosio::chain::database& db = const_cast<eosio::chain::database&>( test.control->db() );

      //    auto ses = db.start_undo_session(true);

      //    // Create an account
      //    db.create<global_property_list_object>([](global_property_list_object &a) {
      //            chain_list_config config;
      //            config.resource_whitelist={1};
      //      a.set_list(config);
      //    });

      //    // Make sure we can retrieve that account by name
      //    const global_property_list_object& ptr = db.get<global_property_list_object>();
      //   //   BOOST_TEST(a.list_configuration.size > 0);
      //    chain_list_config a  = ptr.get_list();
       
      //    uint64_t v = 0;
      //    if(a.resource_whitelist.size() > 0)
      //    {
      //          v =  *(a.resource_whitelist.begin());
      //    }
      string str= "alice,bob,tom";
      flat_set<account_name> list = parse_list_string(str);
         BOOST_TEST(list.size() > 0);
         account_name n =N(a);
         if(list.size()>0)
         {
            n = *(list.begin());
         }

         BOOST_TEST(n !=N(a));
         BOOST_TEST(n ==N(a));
          BOOST_TEST(n ==N(alice));
        //  BOOST_TEST(v == 1);
         // Undo creation of the account
        //  ses.undo();

        //  // Make sure we can no longer find the account
        //  ptr = db.find<global_property_list_object, by_name, std::string>("billy");
        //  BOOST_TEST(ptr == nullptr);
      } FC_LOG_AND_RETHROW()
   }

 // Simple tests of undo infrastructure
   BOOST_AUTO_TEST_CASE(list_config_test) {
      try {
         TESTER test;
       // Bypass read-only restriction on state DB access for this unit test which really needs to mutate the DB to properly conduct its test.
         eosio::chain::database& db = const_cast<eosio::chain::database&>( test.control->db() );

         auto ses = db.start_undo_session(true);

         // Create an account
         db.create<global_property_list_object>([](global_property_list_object &a) {
                 chain_list_config config;
                 config.list={N(1)};
           a.set_list(config);
         });

        
         // Make sure we can retrieve that account by name
         const global_property_list_object& ptr = db.get<global_property_list_object>();
        //   BOOST_TEST(a.list_configuration.size > 0);
         chain_list_config a  = ptr.get_list();
       
         uint64_t v = 0;
         if(a.list.size() > 0)
         {
               v =  *(a.list.begin());
         }
         BOOST_TEST(v > 0);
         BOOST_TEST(v == N(1));
         // Undo creation of the account
         ses.undo();

        //  // Make sure we can no longer find the account
        //  ptr = db.find<global_property_list_object, by_name, std::string>("billy");
        //  BOOST_TEST(ptr == nullptr);
      } FC_LOG_AND_RETHROW()
   }
   
  

 // Simple tests of undo infrastructure
   BOOST_AUTO_TEST_CASE(add_list_config_test) {
      try {
         TESTER test;
       // Bypass read-only restriction on state DB access for this unit test which really needs to mutate the DB to properly conduct its test.
         eosio::chain::database& db = const_cast<eosio::chain::database&>( test.control->db() );

         auto ses = db.start_undo_session(true);

 chain_list_config config;
                 config.list={N(1)};

uint16_t type = actor_blacklist_type;
        test.control->add_config_list(type,config.list);

        
         // Make sure we can retrieve that account by name
         const global_property_list_object& ptr = db.get<global_property_list_object,by_type>(type);
        //   BOOST_TEST(a.list_configuration.size > 0);
         chain_list_config a  = ptr.get_list();
       
         uint64_t v = 0;
         if(a.list.size() > 0)
         {
               v =  *(a.list.begin());
         }
         BOOST_TEST(v > 0);
         BOOST_TEST(v == N(1));
         // Undo creation of the account
         ses.undo();

      } FC_LOG_AND_RETHROW()
   }
   


    // Simple tests of undo infrastructure
   BOOST_AUTO_TEST_CASE(set_string_list_test) {
      try {
         TESTER test;

      string str= "alice,bob,tom";
      string action="add";
      string type = "actor_blacklist";//_type;
        test.control->set_config_string_list(type,action,str);


uint16_t type = actor_blacklist_type;
            
         // Make sure we can retrieve that account by name
         const global_property_list_object& ptr = db.get<global_property_list_object,by_type>(type);
        //   BOOST_TEST(a.list_configuration.size > 0);
         chain_list_config a  = ptr.get_list();
       
         uint64_t v = 0;
         if(a.list.size() > 0)
         {
               v =  *(a.list.begin());
         }
         BOOST_TEST(v > 0);
       
          BOOST_TEST(v ==N(alice));
    
      } FC_LOG_AND_RETHROW()
   }



BOOST_AUTO_TEST_SUITE_END()
