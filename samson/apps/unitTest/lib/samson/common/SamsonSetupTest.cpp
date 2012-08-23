
#include "gtest/gtest.h"
#include "samson/common/SamsonSetup.h"

// Test void SamsonSetup();
TEST(samson_common_SamsonSetup, SamsonSetup) {
  EXPECT_EQ(samson::createDirectory("/tmp/testSamsonSetup"), samson::OK) << "Error in createDirectory test";
  EXPECT_EQ(samson::createFullDirectory("/tmp/level1/level2/level3/testSamsonSetup"),
            samson::OK) << "Error in createFullDirectory test";
  EXPECT_EQ(samson::cannonical_path("/tmp/level1/level2/level3/"),
            "/tmp/level1/level2/level3") << "Error in cannonical_path test";

  // Better not to set /opt/samson and /var/samson, so init() can get environment variables
  // samson::SamsonSetup::init("/opt/samson", "/var/samson");
  au::Singleton<samson::SamsonSetup>::shared()->SetWorkerDirectories("/tmp/testSamsonSetup_home",
                                                                     "/tmp/testSamsonSetup_working");

  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->getValueForParameter("isolated.timeout"),
            "300") << "Error in getValueForParameter for isolated.timeout";

  au::Singleton<samson::SamsonSetup>::shared()->addItem("unit_test.samsonSetupTest", "initial", "dummy for testing",
                                                        samson::SetupItem_string);

  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->isParameterDefined("unit_test.samsonSetupTest"),
            true) << "Error in isParameterDefined positive test";
  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->getValueForParameter("unit_test.samsonSetupTest"),
            "initial") << "Error in getValueForParameter for unit_test.samsonSetupTest";


  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->setValueForParameter("unit_test.samsonSetupTest",
                                                                               "successful"),
            true) << "Error in setValueForParameter for unit_test.samsonSetupTest";
  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->getValueForParameter("unit_test.samsonSetupTest"),
            "successful") << "Error in getValueForParameter for unit_test.samsonSetupTest";

  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->setValueForParameter("isolated.timeout",
                                                                               "1000"),
            true) << "Error in setValueForParameter for isolated.timeout";
  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->getValueForParameter("isolated.timeout"),
            "1000") << "Error in getValueForParameter for isolated.timeout";

  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->isParameterDefined("non_existing_entry"),
            false) << "Error in isParameterDefined for non_existing_entry";
  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->getValueForParameter("non_existing_entry"),
            "Error") << "Error in getValueForParameter for non_existing_entry";

  au::Singleton<samson::SamsonSetup>::shared()->resetToDefaultValues();

  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->isParameterDefined("unit_test.samsonSetupTest"),
            true) << "Error in isParameterDefined negative test";
  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->getValueForParameter("unit_test.samsonSetupTest"),
            "initial") << "Error in getValueForParameter for unit_test.samsonSetupTest";

  EXPECT_EQ(au::Singleton<samson::SamsonSetup>::shared()->getValueForParameter("isolated.timeout"),
            "300") << "Error in getValueForParameter for isolated.timeout";

  au::Singleton<samson::SamsonSetup>::shared()->clearCustumValues();
  EXPECT_EQ(0, au::Singleton<samson::SamsonSetup>::shared()->save()) << "Error in save SamsonSetup";


  // Destroy singletons ( SamsonSetup )
  au::singleton_manager.DestroySingletons();
}
