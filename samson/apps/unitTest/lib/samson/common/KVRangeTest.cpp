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
#include <vector>

#include "gtest/gtest.h"

#include "samson/common/KVRange.h"
#include "samson/common/MemoryCheck.h"
#include "samson/common/MessagesOperations.h"
#include "samson/common/Rate.h"
#include "samson/common/SamsonSetup.h"
#include "samson/common/Visualitzation.h"
#include "samson/common/samson.pb.h"
#include "samson/common/status.h"


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
  
  
  EXPECT_EQ(range_2.str(), "[00001 00010)") << "Error in KVRange str";
  EXPECT_EQ(range_2.size(), 9) << "Error in KVRange getNumHashGroups";
  
  EXPECT_EQ(range_1.IsOverlapped(range_2), true) << "Error in KVRange overlap true";
  EXPECT_EQ(range_2.IsOverlapped(range_1), true) << "Error in KVRange overlap true";
  samson::KVRange range_3(10, 30);
  EXPECT_EQ(range_3.IsOverlapped(range_1), true) << "Error in KVRange overlap true";
  EXPECT_EQ(range_3.IsOverlapped(range_2), false) << "Error in KVRange overlap false";
  
  EXPECT_EQ(range_1.Includes(range_2), true) << "Error in KVRange includes true";
  EXPECT_EQ(range_2.Includes(range_1), false) << "Error in KVRange includes false";
  EXPECT_EQ(range_1.Contains(15), true) << "Error in KVRange contains true";
  EXPECT_EQ(range_1.Contains(20), false) << "Error in KVRange contains false";
  EXPECT_EQ(range_1.Contains(range_2), true) << "Error in KVRange includes true";
  EXPECT_EQ(range_2.Contains(range_1), false) << "Error in KVRange includes false";
  
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

// -----------------------------------------------------------------------------
// KVRange::setFrom
//
static samson::KVInfo   infoV1[KVFILE_NUM_HASHGROUPS];
static samson::KVInfo   infoV2[10];
TEST(samson_common_KVRange, setFrom) {
  samson::KVRange  range;
  
  range.setFrom(&infoV1[0]);
  EXPECT_EQ(range.isValid(), true) << "Error in KVRange isValid() for setFrom(KVInfo) operation";

  infoV1[0].size = 1;
  range.setFrom(&infoV1[0]);
  EXPECT_EQ(range.isValid(), true) << "Error 2 in KVRange isValid() for setFrom(KVInfo) operation";
}

// -----------------------------------------------------------------------------
// KVRange::setFrom2
//
static samson::FullKVInfo   finfoV1[KVFILE_NUM_HASHGROUPS];
static samson::FullKVInfo   finfoV2[10];
TEST(samson_common_KVRange, setFrom2) {
  samson::KVRange  range;
  
  range.setFrom(&finfoV1[0]);
  EXPECT_EQ(range.isValid(), true) << "Error in KVRange isValid() for setFrom(FullKVInfo) operation";

  finfoV1[0].size = 1;
  range.setFrom(&finfoV1[0]);
  EXPECT_EQ(range.isValid(), true) << "Error 2 in KVRange isValid() for setFrom(FullKVInfo) operation";
}

// -----------------------------------------------------------------------------
// add
//
TEST(samson_common_KVRange, add) {
  samson::KVRange  from1(0, 10);
  samson::KVRange  from2(20, 30);
  samson::KVRange  notValid(-1, -9);
  samson::KVRange  to(5, 15);

  EXPECT_EQ(to.hg_begin_, 5);
  EXPECT_EQ(to.hg_end_, 15);

  to.add(from1);
  EXPECT_EQ(to.hg_begin_, 0);
  EXPECT_EQ(to.hg_end_, 15);

  to.add(from2);
  EXPECT_EQ(to.hg_begin_, 0);
  EXPECT_EQ(to.hg_end_, 30);

  to.add(notValid);
  EXPECT_EQ(to.hg_begin_, 0);
  EXPECT_EQ(to.hg_end_, 30);
}

// -----------------------------------------------------------------------------
// overlap
//
TEST(samson_common_KVRange, overlap) {
  samson::KVRange              range(0, 10);
  samson::KVRange              overlap(5, 15);
  samson::KVRange              notOverlap(11, 15);
  samson::KVRange              overlapFactor(5, 15);
  std::vector<samson::KVRange> rangeVector;

  EXPECT_TRUE(range.IsOverlapped(overlap));
  EXPECT_FALSE(range.IsOverlapped(notOverlap));
  EXPECT_EQ(0.50, range.GetOverlapFactor(overlapFactor));
    
  rangeVector.push_back(notOverlap);
  rangeVector.push_back(notOverlap);
  rangeVector.push_back(notOverlap);
  EXPECT_FALSE(range.IsOverlapped(rangeVector));

  rangeVector.push_back(overlap);
  EXPECT_TRUE(range.IsOverlapped(rangeVector));
}

// -----------------------------------------------------------------------------
// Includes
//
TEST(samson_common_KVRange, Includes) {
  samson::KVRange range(0, 10);
  samson::KVRange included(1, 9);
  samson::KVRange notIncluded(0, 12);

  EXPECT_TRUE(range.Includes(included));
  EXPECT_FALSE(range.Includes(notIncluded));
}

// -----------------------------------------------------------------------------
// Contains
//
TEST(samson_common_KVRange, Contains) {
  samson::KVRange range(1, 10);

  EXPECT_TRUE(range.Contains(1));
  EXPECT_TRUE(range.Contains(9));
  EXPECT_FALSE(range.Contains(0));
  EXPECT_FALSE(range.Contains(10));

  samson::KVRange contained(2, 9);
  samson::KVRange notContained(0, 9);
  samson::KVRange notContained2(2, 11);

  EXPECT_TRUE(range.Contains(contained));
  EXPECT_FALSE(range.Contains(notContained));
  EXPECT_FALSE(range.Contains(notContained2));
}

// -----------------------------------------------------------------------------
// divisionForHashGroup
//
namespace samson {
  extern int divisionForHashGroup(int hg, int num_division);
}

TEST(samson_common_KVRange, divisionForHashGroup) {
  EXPECT_EQ(0, samson::divisionForHashGroup(3, 16));
}

// -----------------------------------------------------------------------------
// selectRange - 
//
namespace samson {
  extern KVInfo selectRange(KVInfo *info, KVRange range);
}

TEST(samson_common_KVRange, selectRange) {
  samson::KVInfo  info(256, 16);
  samson::KVRange range(0, 10);
  samson::KVInfo  info2 = selectRange(&info, range);

  // No EXPECT here, not until the function selectRange is fixed ...
}

// -----------------------------------------------------------------------------
// operator<
//
TEST(samson_common_KVRange, operatorLess) {
  samson::KVRange  range1(5, 10);
  samson::KVRange  range2(1, 4);
  samson::KVRange  range3(5, 11);
  
  EXPECT_FALSE(range1 < range2);
  EXPECT_TRUE(range1 < range3);
}

// -----------------------------------------------------------------------------
// operator!=
//
TEST(samson_common_KVRange, operatorNE) {
  samson::KVRange  range1(5, 10);
  samson::KVRange  range2(1, 4);

  EXPECT_TRUE(range1 != range2);
}

// -----------------------------------------------------------------------------
// operator==
//
TEST(samson_common_KVRange, operatorEQ) {
  samson::KVRange  range1(5, 10);
  samson::KVRange  range2(1, 4);
  samson::KVRange  range3(5, 11);

  EXPECT_FALSE(range1 == range2);
  EXPECT_FALSE(range1 == range3);
}

// -----------------------------------------------------------------------------
// str(vector<KVRange>)
//
TEST(samson_common_KVRange, str) {
  samson::KVRange               range1(5, 10);
  samson::KVRange               range2(11, 20);
  std::vector<samson::KVRange>  rangeV;

  rangeV.push_back(range1);
  rangeV.push_back(range2);

  std::string s = str(rangeV);
  EXPECT_STREQ(s.c_str(), "[00005 00010) [00011 00020) ");
}

// -----------------------------------------------------------------------------
// CheckCompleteKVRanges
//
TEST(samson_common_KVRange, CheckCompleteKVRanges) {
  samson::KVRange               range1(5, 10);
  samson::KVRange               range2(11, 20);
  std::vector<samson::KVRange>  rangeV;

  rangeV.push_back(range1);
  rangeV.push_back(range2);
  EXPECT_FALSE(CheckCompleteKVRanges(rangeV));
}

// -----------------------------------------------------------------------------
// Intersection
//
TEST(samson_common_KVRange, Intersection) {
  samson::KVRange range1(10, 20);
  samson::KVRange range2(0, 9);
  samson::KVRange range3(21, 30);
  samson::KVRange output  = range1.Intersection(range2);
  samson::KVRange output2 = range1.Intersection(range3);

  EXPECT_TRUE(output.isValid());
  EXPECT_TRUE(output2.isValid());
}
  
// -----------------------------------------------------------------------------
// divide
//
TEST(samson_common_KVRange, divide) {
  samson::KVRange               range(10, 20);
  std::vector<samson::KVRange>  rangeVec;

  rangeVec = range.divide(2);
  EXPECT_EQ(2, rangeVec.size());
  rangeVec = range.divide(30);
  EXPECT_EQ(10, rangeVec.size());
}

// -----------------------------------------------------------------------------
// gpbConstructor
//
TEST(samson_common_KVRange, gpbConstructor) {
  samson::gpb::KVRange  gpbRange;
  samson::KVRange       range1(1, 2);
  samson::KVRange       range2(range1);

  samson::KVRange       range(gpbRange);
  
  EXPECT_TRUE(range2.isValid());
}
