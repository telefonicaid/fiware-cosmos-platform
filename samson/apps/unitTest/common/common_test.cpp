/* ****************************************************************************
 *
 * FILE            common_test.cpp
 *
 * AUTHOR         Gregorio Escalada
 *
 * DATE            May 2012
 *
 * DESCRIPTION
 *
 * unit testing of the common class in the samson  library
 *
 */

#include "samson/common/MemoryCheck.h"

#include "samson/common/MessagesOperations.h"

#include "samson/common/SamsonSetup.h"
#include "samson/common/status.h"

#include "samson/common/Rate.h"

#include "samson/common/Visualitzation.h"

#include "gtest/gtest.h"



// Test void MessagesOperations();
TEST(commonTest, MessagesOperations) {
  // Better not to set /opt/samson and /var/samson, so init() can get environment variables
  // samson::SamsonSetup::init("/opt/samson", "/var/samson");

  EXPECT_EQ(samson::filterName("OTTstream.parse_logs", "OTTstream", ""), true) << "Error in filterName positive test";
  EXPECT_EQ(samson::filterName("OTTstream.parse_logs", "system", ""), false) << "Error in filterName negative test";
  EXPECT_EQ(samson::filterName("samson.OTTstream.parse_logs", "samson",
                               "logs"), true) << "Error in filterName positive test";
  EXPECT_EQ(samson::filterName("samson.OTTstream.parse_logs", "samson",
                               "log"), false) << "Error in filterName negative test";
}



#include "samson/common/KVRange.h"

// Test  KVRange;
TEST(commonTest, testKVRange) {
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
            "<kv_range><hg_begin><![CDATA[1]]></hg_begin><hg_end><![CDATA[10]]></hg_end></kv_range>") <<
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

#include "samson/common/KVInfo.h"

// Test  KVInfo;
TEST(commonTest, testKVInfo) {
  samson::KVInfo info_1;

  EXPECT_EQ(info_1.str(), "(     0 kvs in     0 bytes )") << "Error in KVInfo str() for default constructor";
  info_1.set(100, 10);
  EXPECT_EQ(info_1.str(), "(  10.0 kvs in   100 bytes )") << "Error in KVInfo str() for set";

  std::ostringstream output;
  info_1.getInfo(output);
  EXPECT_EQ(output.str(),
            "<kv_info><kvs><![CDATA[10]]></kvs><size><![CDATA[100]]></size></kv_info>") << "Error in KVInfo getInfo";


  info_1.append(1, 1);
  EXPECT_EQ(info_1.str(), "(  11.0 kvs in   101 bytes )") << "Error in KVInfo str() for append";
  info_1.remove(2, 2);
  EXPECT_EQ(info_1.str(), "(  9.00 kvs in  99.0 bytes )") << "Error in KVInfo str() for append";
  EXPECT_EQ(info_1.isEmpty(), false) << "Error in KVInfo isValid() for isEmpty() false";
  info_1.clear();
  EXPECT_EQ(info_1.isEmpty(), true) << "Error in KVInfo isValid() for isEmpty() true";

  samson::KVInfo info_2(2000, 20);
  EXPECT_EQ(info_2.str(), "(  20.0 kvs in  2.00Kbytes )") << "Error in KVInfo for initialised constructor";
  EXPECT_EQ(info_1.canAppend(info_2), true) << "Error in KVInfo str() for canAppend";
  info_1.append(info_2);
  EXPECT_EQ(info_1.str(), "(  20.0 kvs in  2.00Kbytes )") << "Error in KVInfo append from KVInfo";
}

#include "samson/common/FullKVInfo.h"
#include "samson/common/common.h"

// Test  FullKVInfo;
TEST(commonTest, testFullKVInfo) {
  samson::FullKVInfo info_1;

  EXPECT_EQ(info_1.str(), "(     0 kvs in     0 bytes )") << "Error in FullKVInfo isValid() for default constructor";
  info_1.set(static_cast<samson::uint32>(100), static_cast<samson::uint32>(10));
  EXPECT_EQ(info_1.str(), "(  10.0 kvs in   100 bytes )") << "Error in FullKVInfo isValid() for set";

  std::ostringstream output;
  info_1.getInfo(output);
  EXPECT_EQ(output.str(),
            "<kv_info><kvs><![CDATA[10]]></kvs><size><![CDATA[100]]></size></kv_info>") <<
  "Error in FullKVInfo getInfo";


  info_1.append(static_cast<samson::uint32>(1), static_cast<samson::uint32>(1));
  EXPECT_EQ(info_1.str(), "(  11.0 kvs in   101 bytes )") << "Error in FullKVInfo isValid() for append";
  info_1.remove(static_cast<samson::uint32>(2), static_cast<samson::uint32>(2));
  EXPECT_EQ(info_1.str(), "(  9.00 kvs in  99.0 bytes )") << "Error in FullKVInfo isValid() for append";
  EXPECT_EQ(info_1.isEmpty(), false) << "Error in FullKVInfo isValid() for isEmpty false";
  info_1.clear();
  EXPECT_EQ(info_1.isEmpty(), true) << "Error in FullKVInfo isValid() for isEmpty true";

  samson::FullKVInfo info_2(2000, 20);
  EXPECT_EQ(info_2.str(), "(  20.0 kvs in  2.00Kbytes )") << "Error in FullKVInfo for initialised constructor";
  info_1.append(info_2);
  EXPECT_EQ(info_1.str(), "(  20.0 kvs in  2.00Kbytes )") << "Error in FullKVInfo append from KVInfo";

  info_1.set(static_cast<samson::uint64>(1000), static_cast<samson::uint64>(100));
  EXPECT_EQ(info_1.str(), "(   100 kvs in  1.00Kbytes )") << "Error in FullKVInfo isValid() for set 64 bits";
  EXPECT_EQ(info_1.fitsInKVInfo(), true) << "Error in FullKVInfo fitsInKVInfo true";
  samson::uint64 kvs = 1024 * 1024;
  kvs *= 1024 * 8;
  samson::uint64 size = 1024 * 1024;
  size *= 1024 * 8;
  info_1.set(kvs, size);
  EXPECT_EQ(info_1.fitsInKVInfo(), false) << "Error in FullKVInfo fitsInKVInfo false";
}



#include "samson/common/KVFile.h"

/*
 *
 * //Test  KVFile;
 * TEST(commonTest, KVFile)
 * {
 *  samson::KVFile null_kvfile( NULL );
 *  EXPECT_EQ(null_kvfile.getErrorMessage(), "NULL _data") << "Error in KVFile constructor for null data";
 *
 *  // Stupid deprecated conversion from string constant to "char*" work-around
 *  char *wrong_header= strdup("data without header");
 *  samson::KVFile wrong_header_kvfile( wrong_header );
 *  EXPECT_EQ(wrong_header_kvfile.getErrorMessage(), "KVHeader error: wrong magic number") << "Error in KVFile constructor for wrong_header";
 *  free(wrong_header);
 * }
 *
 */

#include "samson/common/KVInputVector.h"

// Test  KVInputVector;
TEST(commonTest, KVInputVector) {
  samson::KVInputVector kv_empty_vector(0);
  samson::KVInputVector kv_old_vector(1);

//    samson::Operation  red_op("test_reduce_operation", samson::Operation::reduce);
//    samson::KVInputVector kv_red_op_vector(&red_op);
//
//    samson::Operation  map_op("test_map_operation", samson::Operation::map);
//    samson::KVInputVector kv_map_op_vector(&map_op);
//
//    EXPECT_EQ(kv_empty_vector.getNext(), (samson::KVSetStruct *)NULL) << "Error in getNext for empty KVInputVector";
//    EXPECT_EQ(kv_old_vector.getNext(), (samson::KVSetStruct *)NULL) << "Error in getNext for kv_old_vector KVInputVector";
//    EXPECT_EQ(kv_red_op_vector.getNext(), (samson::KVSetStruct *)NULL) << "Error in getNext for kv_red_op_vector KVInputVector";
//    EXPECT_EQ(kv_map_op_vector.getNext(), (samson::KVSetStruct *)NULL) << "Error in getNext for kv_map_op_vector KVInputVector";
//
//    kv_empty_vector.sort();
//    kv_red_op_vector.sort();
}


