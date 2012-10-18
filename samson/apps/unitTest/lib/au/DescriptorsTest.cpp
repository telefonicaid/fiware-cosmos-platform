#include <fcntl.h>

#include "au/string/Descriptors.h"
#include "gtest/gtest.h"


TEST(au_Descriptors, basic) {
  au::Descriptors descriptors;

  descriptors.Add("Hola");
  descriptors.Add("Hola");
  descriptors.Add("Adios");
  descriptors.Add("Adios");
  descriptors.Add("Adios");

  EXPECT_EQ((size_t)2, descriptors.size());
  EXPECT_EQ("3 X Adios 2 X Hola", descriptors.str());
}
