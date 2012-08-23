#include <fcntl.h>

#include "au/Environment.h"
#include "gtest/gtest.h"
#include "unitTest/TestClasses.h"


TEST(au_Environment, basic) {
  au::Environment e;

  e.Set("param_1", "a");
  e.Set("param_2", "b");
  e.Set("param_3", "c");
  e.Set("param_4", 2.34);

  EXPECT_TRUE(e.IsSet("param_1"));
  EXPECT_FALSE(e.IsSet("param_15"));

  e.Unset("param_3");
  EXPECT_FALSE(e.IsSet("param_3"));

  EXPECT_EQ("a", e.Get("param_1", "?"));
  EXPECT_EQ("?", e.Get("param_15", "?"));
  EXPECT_EQ("2.34", e.Get("param_4", "?"));

  EXPECT_EQ("{param_1 : a param_2 : b param_4 : 2.34}", e.str());

  std::string save = e.SaveToString();

  EXPECT_EQ("param_1=a,param_2=b,param_4=2.34", save);
  e.ClearEnvironment();
  EXPECT_FALSE(e.IsSet("param_1"));

  e.RecoverFromString(save);
  EXPECT_EQ("a", e.Get("param_1", "?"));

  au::Environment e2 = e;
  EXPECT_EQ("a", e2.Get("param_1", "?"));
}
