#include <fcntl.h>

#include "au/Singleton.h"
#include "gtest/gtest.h"
#include "unitTest/TestClasses.h"


TEST(au_Singleton, basic) {
  // Destroy all previous singletons
  au::singleton_manager.DestroySingletons();

  TestBase *example = au::Singleton<TestBase>::shared();
  example->set_value(10);

  // Hopefully pointing to the same variable
  TestBase *example2 = au::Singleton<TestBase>::shared();
  EXPECT_EQ(10, example2->value());

  // Only one instance of the singleton
  EXPECT_EQ(1, TestBase::num_instances());

  // Check number of singleton in manager
  EXPECT_EQ(1, au::singleton_manager.size());

  // Unique call to remove all singletons
  au::singleton_manager.DestroySingletons();

  // Expected no instances of this class
  EXPECT_EQ(0, TestBase::num_instances());

  // Check number of singleton in manager
  EXPECT_EQ(0, au::singleton_manager.size());
}
