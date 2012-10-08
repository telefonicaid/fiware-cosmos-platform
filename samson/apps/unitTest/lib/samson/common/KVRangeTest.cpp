
#include "samson/common/MemoryCheck.h"
#include "samson/common/MessagesOperations.h"
#include "samson/common/SamsonSetup.h"
#include "samson/common/status.h"
#include "samson/common/Rate.h"
#include "samson/common/Visualitzation.h"
#include "samson/common/KVRange.h"

#include "gtest/gtest.h"


// Test  KVRange;
TEST(samson_common_KVRange, test1) {
  samson::KVRange range_1;
  
  EXPECT_EQ(range_1.isValid(), true) << "Error in KVRange isValid() for default constructor";
  range_1.set(0, 10);
  EXPECT_EQ(range_1.isValid(), true) << "Error in KVRange isValid() for set operation";
  range_1.set(-1, 10);
  EXPECT_EQ(range_1.isValid(), false) << "Error in KVRange isValid() for wrong set operation";
  range_1.set(0, -1);
  EXPECT_EQ(range_1.isValid(), false) << "Error in KVRange isValid() for wrong set operation";
  range_1.set(1, 0);
  EXPECT_EQ(range_1.isValid(), false) << "Error in KVRange isValid() for wrong set operation";
  range_1.set(0, 20);
  
  samson::KVRange range_2(1, 10);
  EXPECT_EQ(range_1.isValid(), true) << "Error in KVRange isValid() for initialised constructor";
  
  std::ostringstream output;
  range_2.getInfo(output);
  EXPECT_EQ(output.str(),
            "<kv_range><hg_begin>1</hg_begin><hg_end>10</hg_end></kv_range>") <<
  "Error in KVRange getInfo";
  
  EXPECT_EQ(range_2.str(), "[1 10]") << "Error in KVRange str";
  EXPECT_EQ(range_2.getNumHashGroups(), 9) << "Error in KVRange getNumHashGroups";
  
  EXPECT_EQ(range_1.IsOverlapped(range_2), true) << "Error in KVRange overlap true";
  EXPECT_EQ(range_2.IsOverlapped(range_1), true) << "Error in KVRange overlap true";
  samson::KVRange range_3(10, 30);
  EXPECT_EQ(range_3.IsOverlapped(range_1), true) << "Error in KVRange overlap true";
  EXPECT_EQ(range_3.IsOverlapped(range_2), false) << "Error in KVRange overlap false";
  
  EXPECT_EQ(range_1.includes(range_2), true) << "Error in KVRange includes true";
  EXPECT_EQ(range_2.includes(range_1), false) << "Error in KVRange includes false";
  EXPECT_EQ(range_1.contains(15), true) << "Error in KVRange contains true";
  EXPECT_EQ(range_1.contains(20), false) << "Error in KVRange contains false";
  EXPECT_EQ(range_1.contains(range_2), true) << "Error in KVRange includes true";
  EXPECT_EQ(range_2.contains(range_1), false) << "Error in KVRange includes false";
  EXPECT_EQ(range_1.isValidForNumDivisions(10), true) << "Error in KVRange isValidForNumDivisions true";
  EXPECT_EQ(range_1.isValidForNumDivisions(20000), false) << "Error in KVRange isValidForNumDivisions false";
  EXPECT_EQ(range_1.getMaxNumDivisions(), 2048) << "Error in KVRange getMaxNumDivisions true";
  EXPECT_EQ(range_2.getMaxNumDivisions(), 4096) << "Error in KVRange getMaxNumDivisions false";
  
  
  EXPECT_EQ((range_1 < range_2), false) << "Error in KVRange operator < false";
  range_1.set(11, 20);
  EXPECT_EQ((range_2 < range_1), true) << "Error in KVRange operator < true";
  
  EXPECT_EQ((range_1 == range_2), false) << "Error in KVRange operator == false";
  range_1.set(1, 10);
  EXPECT_EQ((range_2 == range_1), true) << "Error in KVRange operator == true";
  
  EXPECT_EQ((range_1 != range_2), false) << "Error in KVRange operator != false";
  range_1.set(1, 11);
  EXPECT_EQ((range_2 != range_1), true) << "Error in KVRange operator != true";
}
