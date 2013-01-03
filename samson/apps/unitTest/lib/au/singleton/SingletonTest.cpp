/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
#include <fcntl.h>

#include "au/singleton/Singleton.h"
#include "gtest/gtest.h"
#include "unitTest/TestClasses.h"


TEST(au_singleton_Singleton, basic) {
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
  EXPECT_EQ((size_t)1, au::singleton_manager.size());

  // Destroy the instance to get a new one
  au::Singleton<TestBase>::DestroySingleton();
  TestBase *example3 = au::Singleton<TestBase>::shared();
  EXPECT_NE(10 , example3->value());

  // Still on element in the manager
  EXPECT_EQ((size_t)1, au::singleton_manager.size());

  // Unique call to remove all singletons
  au::singleton_manager.DestroySingletons();

  // Expected no instances of this class
  EXPECT_EQ(0, TestBase::num_instances());

  // Check number of singleton in manager
  EXPECT_EQ((size_t)0, au::singleton_manager.size());
}
