#include "au/containers/Dictionary.h"
#include "au/containers/SharedPointer.h"
#include "gtest/gtest.h"
#include "unitTest/test_common.h"

TEST(au_containers_Dictionary, assignation) {
  au::Dictionary<std::string, TestBase> dictionary;

  dictionary.Set("hola", new TestBase(10));
  au::SharedPointer<TestBase> p = dictionary.Get("hola");


  EXPECT_EQ(p->v(), 10) << "Error in au::Dictionary basic operations";
}

TEST(au_containers_Dictionary, duplication) {
  au::Dictionary<std::string, TestBase> dictionary;

  dictionary.Set("hola", new TestBase(10));
  dictionary.Set("adios", dictionary.Get("hola"));

  au::SharedPointer<TestBase> p = dictionary.Get("hola");
  EXPECT_EQ(p->v(), 10) << "Error in au::Dictionary basic operations";

  EXPECT_EQ(dictionary.Get("hola") == dictionary.Get("adios"),
            true) << "Error in au::Dictionary duplication";
}

