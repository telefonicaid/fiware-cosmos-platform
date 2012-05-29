/* ****************************************************************************
*
* FILE           SharedMemoryManager_test.cpp
*
* AUTHOR         Grant Croker
*
* DATE           May 2012
*
* DESCRIPTION
*
* Unit tests for the engine::SharedMemoryManager class
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
#include "samson/isolated/SharedMemoryManager.h"

class SharedMemoryManagerTest : public ::testing::Test
{
    protected:
		virtual void SetUp()
		{
	        std::string samson_home    = SAMSON_HOME_DEFAULT;
	        std::string samson_working = SAMSON_WORKING_DEFAULT;

	        char *env_samson_working = getenv("SAMSON_WORKING");
	        char *env_samson_home = getenv("SAMSON_HOME");

	        if( env_samson_working )
	            samson_working = env_samson_working;

	        if( env_samson_home )
	            samson_home = env_samson_home;

	        // Init SamsonSetup
	        samson::SamsonSetup::init( samson_home , samson_working );

	        // Init Engine
	        engine::Engine::init();
		}

		virtual void TearDown()
		{
		    engine::Engine::stop();                  // Stop engine
		    engine::Engine::destroy();               // Destroy Engine
		}


};


// void SharedMemoryManager::init(
// 					int _shared_memory_num_buffers , size_t _shared_memory_size_per_buffer )

TEST_F(SharedMemoryManagerTest, init)
{
	engine::SharedMemoryManager*  sharedMemoryManager = NULL;

	engine::SharedMemoryManager::init( 1, 67108864); //Allocate a single 64MB buffer
	sharedMemoryManager = engine::SharedMemoryManager::shared();

	EXPECT_TRUE(sharedMemoryManager != NULL) << "Error initializing the shared memory segment";
}
