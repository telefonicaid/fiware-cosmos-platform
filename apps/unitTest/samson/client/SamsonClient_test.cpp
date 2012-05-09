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
#include "au/ThreadManager.h"

#include "engine/Engine.h"
#include "engine/DiskManager.h"
#include "engine/ProcessManager.h"
#include "engine/MemoryManager.h"
#include "samson/module/ModulesManager.h"

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

		virtual void TearDown()
		{
		    engine::Engine::stop();                  // Stop engine
		    engine::DiskManager::stop();             // Stop disk manager
		    engine::ProcessManager::stop();          // Stop process manager

		    //au::ThreadManager::wait_all_threads();   // Wait all threads to finsih

		    engine::DiskManager::destroy();          // Destroy Disk manager
		    engine::MemoryManager::destroy();        // Destroy Memory manager
		    engine::ProcessManager::destroy();       // Destroy Process manager
		    engine::Engine::destroy();               // Destroy Engine
		}

		samson::SamsonClient *samson_client;
		std::string controller;
		int port;
		std::string user;
		std::string password;
		au::ErrorManager error;

};

/* Test void initConnection( au::ErrorManager * error
                    , std::string samson_node
                    , int port = SAMSON_WORKER_PORT
                    , std::string user = "anonymous"
                    , std::string password = "anonymous"
                    );*/
TEST_F(SamsonClientTest, initConnection)
{
	samson_client->initConnection( &error, controller , port , user , password );
	ASSERT_FALSE(error.isActivated());
}

TEST_F(SamsonClientTest, DISABLED_connection_ready)
{
	samson_client->initConnection( &error, controller , port , user , password );
	ASSERT_TRUE(samson_client->connection_ready());
}

