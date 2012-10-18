#include <fcntl.h>

#include "gtest/gtest.h"

#include "au/setup/SetupItem.h"

TEST(au_setup_SetupItem, basic) {

   au::SetupItem item( "category.name" , "default_value" , "description" , au::SetupItem_string );

   EXPECT_TRUE( item.name() == "category.name" );
   EXPECT_TRUE( item.default_value() == "default_value" );
   EXPECT_TRUE( item.value() == "default_value" );
   EXPECT_TRUE( item.description() == "description" );
   EXPECT_TRUE( item.category()  == "category" );
}

TEST(au_setup_SetupItem, chech_valid_values) {

  au::SetupItem item( "category.name" , "default_value" , "description" , au::SetupItem_string );
   EXPECT_TRUE( item.CheckValidValue("Whatever") );

   // CheckValidValue
   au::SetupItem item2( "class.name2" , "12" , "description" , au::SetupItem_uint64 );
   EXPECT_FALSE( item2.CheckValidValue("Whatever"));
   EXPECT_TRUE( item2.CheckValidValue("123456"));

}

TEST(au_setup_SetupItem, set_values) {
  
   au::SetupItem item( "class.name3" , "default_value" , "description" , au::SetupItem_string );
   EXPECT_TRUE( item.value() == "default_value" );
   item.set_value("other");
   EXPECT_TRUE( item.value() == "other" );
   EXPECT_FALSE( item.value() == "default_value" );
  item.ResetToDefaultValue();
  EXPECT_FALSE( item.value() == "other" );
  EXPECT_TRUE( item.value() == "default_value" );

  au::SetupItem item2( "class.name3" , "12" , "description" , au::SetupItem_uint64);
  EXPECT_TRUE( item2.value() == "12" );
  EXPECT_FALSE( item2.set_value("other"));
  EXPECT_FALSE( item2.value() == "other" );
  EXPECT_TRUE( item2.value() == "12" );
  
}
