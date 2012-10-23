

#include "au/statistics/Cronometer.h"
#include "gtest/gtest.h"

TEST(au_Cronometer, basic) {
  
   au::Cronometer cronometer;;
   sleep(1);
   EXPECT_TRUE( cronometer.seconds() > 0.5  );
   cronometer.Reset();
   EXPECT_TRUE( cronometer.seconds() < 0.5  );


}
