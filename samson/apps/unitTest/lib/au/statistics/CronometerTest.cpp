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


#include "au/statistics/Cronometer.h"
#include "gtest/gtest.h"

TEST(au_Cronometer, basic) {
  
   au::Cronometer cronometer;;
   sleep(1);
   EXPECT_TRUE( cronometer.seconds() > 0.5  );
   cronometer.Reset();
   EXPECT_TRUE( cronometer.seconds() < 0.5  );


}
