/* ****************************************************************************
*
* FILE            processManager_test.cpp
*
* AUTHOR          Javier Lois
*
* DATE            January 2012
*
* DESCRIPTION
*
* unit testing of the ProcessManager class in the engine library
*
*/

//#include <direct.h> // for getcwd
#include <stdlib.h>// for MAX_PATH

#include "gtest/gtest.h"

#include "engine/ProcessManager.h"
#include "engine/ProcessItem.h"
#include "engine/Notification.h"

#include "samson/worker/DataBufferProcessItem.h" //to test ProcessItem
#include "samson/worker/BufferVector.h" //to test ProcessItem
#include "samson/controller/Queue.h"

#include "xmlmarkup/xmlmarkup.h"

//TODO: calling init in the ProcessManager makes it run in the background and therefore the test application never exits. How can we kill it?


// Tests ProcessManager's instantiation
TEST(processManagerTest, instantiationTest) {
    //access instance without initialise. Should return NULL.
    EXPECT_EQ(engine::ProcessManager::shared(), static_cast<engine::ProcessManager*>(NULL)) << "Uninitialized ProcessManager should be null";
    //call init() and then instance. Should return a valid one.
    engine::ProcessManager::init(3);
    EXPECT_TRUE(engine::ProcessManager::shared() != static_cast<engine::ProcessManager*>(NULL)) << "ProcessManager instance should not be null after instantiation"; 
    
        exit(0);
   
}

//Test static int getNumCores();
TEST(processManagerTest, getNumCoresTest) {
    engine::ProcessManager::init(3);
    EXPECT_EQ(engine::ProcessManager::shared()->getNumCores(), 3);

}

//Test static int getNumUsedCores();
TEST(processManagerTest, getNumUsedCoresTest) {
    engine::ProcessManager::init(3);
    EXPECT_EQ(engine::ProcessManager::shared()->getNumUsedCores(), 0);

    samson::network::Queue queue;
    samson::QueueuBufferVector* qbvector = new samson::QueueuBufferVector(queue, false);
    samson::DataBufferProcessItem item(qbvector);

    engine::ProcessManager::shared()->add(&item, 1);

    //TODO: How to have a running process?? They get in the queue
    //EXPECT_EQ(engine::ProcessManager::shared()->getNumUsedCores(), 1);

}

//Test void notify( Notification* notification );
//TEST(processManagerTest, notifyTest) {
//This function just writes an error message saying that you shouldn't do the notification here
//}

//Test void add( ProcessItem *item , size_t listenerId );                          
TEST(processManagerTest, addTest) {
    engine::ProcessManager::init(3);
    
    EXPECT_EQ(engine::ProcessManager::shared()->public_num_proccesses, 0);

    samson::network::Queue queue;
    samson::QueueuBufferVector* qbvector =  new samson::QueueuBufferVector(queue, false);
    samson::DataBufferProcessItem* item = new samson::DataBufferProcessItem(qbvector);

    //engine::ProcessManager::shared()->add(item, 1);
    
    std::ostringstream info;
    engine::ProcessManager::shared()->getInfo( info );
    std::cout << info.str() << std::endl;


    EXPECT_EQ(engine::ProcessManager::shared()->public_num_proccesses, 1);

    //engine::ProcessManager::shared()->finishProcessItem(item);

    
}


    

    // Function to cancel a Process. 
//Test void cancel( ProcessItem *item );                   
    
    // Publics but only called from SAMSON platform ( background process notifying ... )
    
    // Notification that this ProcessItem has finished
//Test void finishProcessItem( ProcessItem *item );		
    // Notification that this ProcessItem is halted ( blocked until output memory is ready ) 
//Test void haltProcessItem( ProcessItem *item );			
    
    // Get information for monitoring
//Test void getInfo( std::ostringstream& output);



