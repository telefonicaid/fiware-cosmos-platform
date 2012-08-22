
#include "au/S.h"
#include "gtest/gtest.h"


TEST(au_S, basic) {
  std::string s = au::S() << "int " << 4 << " double " << 0.6;

  EXPECT_EQ("int 4 double 0.6", s);
}
