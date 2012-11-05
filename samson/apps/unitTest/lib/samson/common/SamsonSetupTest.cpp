
#include "gtest/gtest.h"
#include "samson/common/SamsonSetup.h"

// Test void SamsonSetup();
TEST(samson_common_SamsonSetup, SamsonSetup) {

  
  std::string samson_home_dir = au::GetRandomDirectory();
  std::string samson_working_dir = au::GetRandomDirectory();

  
  // Better not to set /opt/samson and /var/samson, so init() can get environment variables
  au::Singleton<samson::SamsonSetup>::shared()->SetWorkerDirectories(samson_home_dir,samson_working_dir);

  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->Get("isolated.timeout"),"300")
  << "Error in getValueForParameter for isolated.timeout";

  au::Singleton<samson::SamsonSetup>::shared()->AddItem("unit_test.samsonSetupTest"
                                                        , "initial"
                                                        , "dummy for testing");

  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->IsParameterDefined("unit_test.samsonSetupTest"),true)
  << "Error in isParameterDefined positive test";
  
  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->Get("unit_test.samsonSetupTest"),"initial")
  << "Error in getValueForParameter for unit_test.samsonSetupTest";

  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->Set("unit_test.samsonSetupTest","successful"),true)
  << "Error in setValueForParameter for unit_test.samsonSetupTest";
  
  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->Get("unit_test.samsonSetupTest"),"successful")
  << "Error in getValueForParameter for unit_test.samsonSetupTest";

  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->Set("isolated.timeout","1000"),true)
  << "Error in setValueForParameter for isolated.timeout";
  
  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->Get("isolated.timeout"),"1000")
  << "Error in getValueForParameter for isolated.timeout";

  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->IsParameterDefined("non_existing_entry"),false)
  << "Error in isParameterDefined for non_existing_entry";
  
  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->Get("non_existing_entry"),"")
  << "Error in getValueForParameter for non_existing_entry";

  au::Singleton<samson::SamsonSetup>::shared()->ResetToDefaultValues();

  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->IsParameterDefined("unit_test.samsonSetupTest"),true)
  << "Error in isParameterDefined negative test";
  
  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->Get("unit_test.samsonSetupTest"),"initial")
  << "Error in getValueForParameter for unit_test.samsonSetupTest";

  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->Get("isolated.timeout"),"300")
  << "Error in getValueForParameter for isolated.timeout";

  au::Singleton<samson::SamsonSetup>::shared()->ResetToDefaultValues();
  
  EXPECT_TRUE( au::Singleton<samson::SamsonSetup>::shared()->Save()) << "Error in save SamsonSetup";

  // Destroy singletons ( SamsonSetup )
  au::singleton_manager.DestroySingletons();
}
