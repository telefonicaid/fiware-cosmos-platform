

#include "au/Rate.h"
#include "gtest/gtest.h"

TEST(au_Rate, basic) {
  au::rate::Rate rate;

  rate.Push(1);
  rate.Push(2);
  rate.Push(3);

  EXPECT_EQ(rate.size(), 6);
  EXPECT_EQ(rate.hits(), 3);

  EXPECT_TRUE(rate.rate() > 0);
  EXPECT_TRUE(rate.hit_rate() > 0);
}
