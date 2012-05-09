/* ****************************************************************************
*
* FILE           SamsonClient_test.cpp
*
* AUTHOR         Grant Croker
*
* DATE           May 2012
*
* DESCRIPTION
*
* Unit tests for the samson::SamsonClient class
*
*/

#include "gtest/gtest.h"

#include "samson/common/ports.h"
#include "samson/client/SamsonClient.h"
#include "au/ErrorManager.h"

class SamsonClientTest : public ::testing::Test
{
    protected:
		virtual void SetUp()
		{
			// Init SamsonClient
			size_t total_memory = 10*1024*1024; // Allocate 10MB
		    samson::SamsonClient::general_init( total_memory );
		    samson_client = new samson::SamsonClient("SamsonClientTest");

		    controller = "localhost";
		    port = SAMSON_WORKER_PORT;
		    user = "anonymous";
		    password = "anonymous";

		}

		/*
		virtual void TearDown()
		{
			// Kill SamsonClient
		}
		*/

		samson::SamsonClient *samson_client;
		std::string controller;
		int port;
		std::string user;
		std::string password;
		au::ErrorManager error;

};

//Test void getInfo( std::ostringstream& output);
TEST_F(SamsonClientTest, initConnection)
{

	samson_client->initConnection( &error, controller , port , user , password );
	ASSERT_FALSE(error.isActivated());
}


