
#include "samson/common/MemoryCheck.h"
#include "samson/common/MessagesOperations.h"
#include "samson/common/SamsonSetup.h"
#include "samson/common/status.h"
#include "samson/common/Rate.h"
#include "samson/common/Visualitzation.h"
#include "samson/common/KVRange.h"
#include "samson/common/KVInfo.h"
#include "samson/common/FullKVInfo.h"
#include "samson/common/common.h"

#include "gtest/gtest.h"


// Test  FullKVInfo;
TEST(samson_common_FullKVInfo, test1) {
  samson::FullKVInfo info_1;
  
  EXPECT_EQ(info_1.str(), "(     0 kvs in     0 bytes )") << "Error in FullKVInfo isValid() for default constructor";
  info_1.set(static_cast<samson::uint32>(100), static_cast<samson::uint32>(10));
  EXPECT_EQ(info_1.str(), "(  10.0 kvs in   100 bytes )") << "Error in FullKVInfo isValid() for set";
  
  std::ostringstream output;
  info_1.getInfo(output);
  EXPECT_EQ(output.str(),
            "<kv_info><kvs>10</kvs><size>100</size></kv_info>") <<
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
