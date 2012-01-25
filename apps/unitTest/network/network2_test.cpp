/* ****************************************************************************
*
* FILE            network2Test.cpp
*
* AUTHOR          Javier Lois
*
* DATE            December 2011
*
* DESCRIPTION
*
* unit testing of the samson/network library
*
*/

#include "gtest/gtest.h"

#include "samson/network/Network2.h"

// Test constructor
// Network(Endpoint::Type type, const char* alias, unsigned short port = 0, int endpoints = 80, int workers = 10);
TEST(networkTest, constructorTest) {
    samson::Network2 network_default(samson::Endpoint2::Spawner);
    //EXPECT_EQ(network_default.type, samson::Endpoint2::Worker);
    EXPECT_TRUE(network_default.epMgr != static_cast<samson::EndpointManager*>(NULL));
    //samson::Network2 network2(samson::Endpoint2::Controller, "192.168.1.1");
    //EXPECT_EQ(network_default.epMgr, static_cast<char*>(NULL));
    //EXPECT_TRUE(true);
}
