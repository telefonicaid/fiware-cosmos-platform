
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
