#include <fcntl.h>

#include "gtest/gtest.h"

#include "au/setup/SetupItemCollection.h"

TEST(au_setup_SetupItemCollection, basic) {

  au::SetupItemCollection collection;

  collection.AddUInt64Item("general.memory" , "10000" , "Memory of the system" );
  collection.AddUInt64Item("general.name" , "my name" , "Name of the system" );
  
  EXPECT_TRUE( collection.GetUInt64("general.memory") == 10000 );
  EXPECT_TRUE( collection.Get("general.name") == "my name" );
  
  EXPECT_TRUE( collection.GetUInt64("general.XXX") == 0 );
  EXPECT_TRUE( collection.Get("general.XXX") == "" );

  
  collection.Set("general.memory", "2000");
  EXPECT_TRUE( collection.GetUInt64("general.memory") == 2000 );
  EXPECT_TRUE( collection.GetDefault("general.memory") == "10000" );
  
  collection.ResetToDefaultValues();
  EXPECT_TRUE( collection.GetUInt64("general.memory") == 10000 );
  
  EXPECT_TRUE( collection.IsParameterDefined("general.memory") );
  EXPECT_FALSE( collection.IsParameterDefined("general.XXX") );

  EXPECT_TRUE( collection.GetItemNames().size() == 2 );
  
  
}
