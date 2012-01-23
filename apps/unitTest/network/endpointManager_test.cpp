/* ****************************************************************************
*
* FILE            endpointManager_test.cpp
*
* AUTHOR          Javier Lois
*
* DATE            January 2012
*
* DESCRIPTION
*
* unit testing of the samson/network library, EndpointManager class
*
*/

#include "gtest/gtest.h"

#include "samson/network/EndpointManager.h"

//Test constructor EndpointManager(Endpoint2::Type _type, const char* controllerIp = NULL);
TEST(endpointManagerTest, constructorTest) {
    samson::EndpointManager epm(samson::Endpoint2::Worker);
    //Host Manager was created
    EXPECT_TRUE(epm.hostMgr != static_cast<samson::HostMgr*>(NULL));
    EXPECT_TRUE(epm.me != static_cast<samson::Endpoint2*>(NULL));
    EXPECT_TRUE(epm.controller != static_cast<samson::Endpoint2*>(NULL));
    EXPECT_TRUE(epm.listener != static_cast<samson::ListenerEndpoint*>(NULL));
    
}
TEST(endpointManagerTest, constructor2Test) {
    EXPECT_TRUE(true);
}
