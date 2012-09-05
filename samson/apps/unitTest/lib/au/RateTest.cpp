

#include "au/Rate.h"
#include "gtest/gtest.h"

TEST(au_Rate, basic) {
  au::rate::Rate rate;

  rate.Push(1);
  rate.Push(2);
  rate.Push(3);

  EXPECT_EQ((size_t)6, rate.size());
  EXPECT_EQ((size_t)3, rate.hits());

  EXPECT_TRUE(rate.rate() > 0);
  EXPECT_TRUE(rate.hit_rate() > 0);
}
