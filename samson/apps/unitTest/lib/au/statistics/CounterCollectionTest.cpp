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

#include "au/statistics/CounterCollection.h"
#include "au/string/StringComponents.h"
#include "gtest/gtest.h"

TEST(au_CounterCollection, simple) {
  au::CounterCollection<std::string> counter_collection;

  ASSERT_TRUE(counter_collection.appendAndGetCounterFor("Hola") == 1);
  ASSERT_TRUE(counter_collection.appendAndGetCounterFor("Hola") == 2);
  ASSERT_TRUE(counter_collection.appendAndGetCounterFor("Hola") == 3);
  ASSERT_TRUE(counter_collection.appendAndGetCounterFor("Adios") == 1);
}
