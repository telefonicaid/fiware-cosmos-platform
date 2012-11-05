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



#include "gtest/gtest.h"

#include "samson/common/KVRangeDivision.h"

TEST(samson_common_KVRangeDivision, test1) {

  samson::KVRangeDivision range_division;
  EXPECT_TRUE( range_division.ranges().size() == 1 );
  EXPECT_TRUE( range_division.ranges()[0] == samson::KVRange( 0 , KVFILE_NUM_HASHGROUPS ) );

  samson::KVRangeDivision range_division2(2);
  EXPECT_TRUE( range_division2.ranges().size() == 2 );

  samson::KVRangeDivision range_division10(10);
  EXPECT_TRUE( range_division10.ranges().size() == 10 );
  
  samson::KVRangeDivision range_divisionX;
  range_divisionX.AddDivision(1024);
  EXPECT_TRUE( range_divisionX.ranges()[0] == samson::KVRange(0,1024) );
  EXPECT_TRUE( range_divisionX.ranges()[1] == samson::KVRange(1024, KVFILE_NUM_HASHGROUPS ) );

  samson::KVRangeDivision range_divisionY;
  range_divisionY.AddDivision( samson::KVRange( 1024, 2048) );
  EXPECT_TRUE( range_divisionY.ranges()[0] == samson::KVRange(0,1024) );
  EXPECT_TRUE( range_divisionY.ranges()[1] == samson::KVRange(1024, 2048 ) );
  EXPECT_TRUE( range_divisionY.ranges()[2] == samson::KVRange(2048, KVFILE_NUM_HASHGROUPS ) );

}