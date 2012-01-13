/* ****************************************************************************
*
* FILE            processItem_Test.cpp
*
* AUTHOR          Javier Lois
*
* DATE            January 2012
*
* DESCRIPTION
*
* unit testing of the ProcessItem class in the engine library
*
*/

#include "gtest/gtest.h"

#include "engine/Engine.h"
#include "engine/ProcessItem.h"
#include "engine/ProcessManager.h"

#include "au/ProcessStats.h"

//ProcessItem is pure virtual. In order to test it we need to use DataBufferProcessItem, which is derived from it
#include "samson/worker/DataBufferProcessItem.h" //to test ProcessItem
#include "samson/worker/BufferVector.h" //to test ProcessItem
#include "samson/controller/Queue.h"

#include "xmlparser/xmlParser.h"


//Test std::string getStatus();
//Test bool isReady(){ return true; };
//Test std::string getDescription();
//Test bool isRunning();
TEST(processItemTest, getStatusTest) {
    engine::Engine::init();

    samson::network::Queue queue;
    samson::QueueuBufferVector* qbvector = new samson::QueueuBufferVector(queue, false);
    samson::DataBufferProcessItem item(qbvector);
    
    EXPECT_EQ(item.getStatus(), "Q:10:Prewrite    0 Bytes") << "Error getting status";
    EXPECT_TRUE(item.isReady()) << "Item should be ready";
    EXPECT_EQ(item.getDescription(), "Process Item 'Prewrite    0 Bytes' Status: Q:10:Prewrite    0 Bytes") << "Error in description";
    EXPECT_TRUE(item.isRunning()==false) << "Process item is not supposed to be running at this point";

}

//Test void setCanceled();
//Test bool isProcessItemCanceled();
TEST(processItemTest, isCanceledTest) {
    engine::Engine::init();

    samson::network::Queue queue;
    samson::QueueuBufferVector* qbvector = new samson::QueueuBufferVector(queue, false);
    samson::DataBufferProcessItem item(qbvector);
    
    EXPECT_TRUE(item.isProcessItemCanceled() == false) << "ProcessItem should not be canceled";
    item.setCanceled();
    EXPECT_TRUE(item.isProcessItemCanceled()) << "ProcessItem should be canceled now";
    
}

//void getInfo( std::ostringstream& output);
TEST(processItemTest, getInfoTest) {
    engine::Engine::init();

    samson::network::Queue queue;
    samson::QueueuBufferVector* qbvector = new samson::QueueuBufferVector(queue, false);
    samson::DataBufferProcessItem item(qbvector);
    
    std::ostringstream info;
    item.getInfo( info );
    //std::cout << "getInfo: " << info.str() << std::endl;
    //TODO ...
}

//Test void runInBackground();
/*TEST(processItemTest, runInBackgroundTest) {
    engine::Engine::init();
    engine::ProcessManager::init(10);

    samson::network::Queue queue;
    samson::QueueuBufferVector* qbvector = new samson::QueueuBufferVector(queue, false);
    samson::DataBufferProcessItem item(qbvector);
    
    ProcessStats pstats;
    size_t beforeThreads = pstats.get_nthreads();
    
    item.runInBackground();
    
    pstats.refresh();
    EXPECT_TRUE(pstats.get_nthreads() > beforeThreads );
    engine::ProcessManager::shared()->cancel(item);
} */
   /* 
    
    bool isRunning();
    void addListenerId( size_t _listenerId );
    
    
    // Get information for xml monitoring
    void getInfo( std::ostringstream& output);

*/
