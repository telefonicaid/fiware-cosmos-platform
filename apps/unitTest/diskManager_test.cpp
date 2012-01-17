/* ****************************************************************************
*
* FILE            diskManagerTest.cpp
*
* AUTHOR          Javier Lois
*
* DATE            December 2011
*
* DESCRIPTION
*
* unit testing of the engine library
*
*/

#include <sys/time.h>

#include "gtest/gtest.h"

#include "engine/DiskManager.h"

#include "au/ProcessStats.h"

#include "xmlparser/xmlParser.h"
  
// Test DiskManager's instantiation
TEST(diskManagerTest, instantiationTest) {
    ProcessStats pstats;
    unsigned long threadsBefore =  pstats.get_nthreads();

    //access instance without initialise. Should return NULL.
    EXPECT_EQ(engine::DiskManager::shared(), static_cast<engine::DiskManager*>(NULL)) 
              << "Uninitialized DiskManager should be null"; //using just NULL produces compilation error

    //call init() and then shared(). Should return a valid one.
    engine::DiskManager::init(10);
    ASSERT_TRUE(engine::DiskManager::shared() != static_cast<engine::DiskManager*>(NULL)) 
                << "DiskManager instance should not be null after instantiation"; 

    //check that new threads have been created
    pstats.refresh();
    unsigned long threadsAfter =  pstats.get_nthreads();
    EXPECT_TRUE(threadsAfter > threadsBefore);
}

//test void getInfo( std::ostringstream& output);
TEST(diskManagerTest, getInfoTest) {
    //engine::Engine::init();
    engine::DiskManager::init(10);
    char buffer[1024*1024];
    engine::DiskOperation* operation = engine::DiskOperation::newReadOperation( buffer , "test_filename.txt" , 0 , 1, 0 );
    engine::DiskManager::shared()->add(operation);

    std::ostringstream info;
    engine::DiskManager::shared()->getInfo( info );
    //std::cout << std::endl << info.str() << std::endl;

    //XML parsing
    XMLNode xMainNode=XMLNode::parseString(info.str().c_str(),"disk_manager");
    //Check that pending operations and running operations have a value
    EXPECT_TRUE(xMainNode.getChildNode("num_pending_operations").getClear().lpszValue != (char*)NULL) << "Error writing pending operations tag";
    EXPECT_TRUE(xMainNode.getChildNode("num_running_operations").getClear().lpszValue != NULL) << "Error writing running operations tag";
    //If pending operations is set to 1, the operation is in <queued>, otherwise it is in <running>
    XMLNode tmpNode;
    if(strcmp(xMainNode.getChildNode("num_pending_operations").getClear().lpszValue, "1") == 0)
    {
        tmpNode = xMainNode.getChildNode("queued");
    }
    else
    {
        tmpNode = xMainNode.getChildNode("running");
    }
    
    ASSERT_TRUE(!tmpNode.isEmpty());
    XMLNode diskOperationNode = tmpNode.getChildNode("disk_operation");
    ASSERT_TRUE(!diskOperationNode.isEmpty());
    EXPECT_EQ(std::string(diskOperationNode.getChildNode("type").getText()), "read") << "Error writing type tag";
    EXPECT_EQ(std::string(diskOperationNode.getChildNode("file_name").getText()), "test_filename.txt") << "Error writing file_name tag";
    EXPECT_EQ(std::string(diskOperationNode.getChildNode("size").getText()), "1") << "Error writing size tag";
    EXPECT_EQ(std::string(diskOperationNode.getChildNode("offset").getText()), "0") << "Error writing offset tag";
    
    XMLNode statisticsNode = xMainNode.getChildNode("statistics");
    XMLNode readNode = statisticsNode.getChildNode("read");
    EXPECT_TRUE(std::string(readNode.getChildNode("description").getText()).find("Currently    0 hits/s    0 B/s") 
        != std::string::npos) << "Error writing read statistics tag";
    EXPECT_EQ(std::string(readNode.getChildNode("rate").getClear().lpszValue), "0") << "Error writing read rate tag";
    XMLNode writeNode = statisticsNode.getChildNode("read");
    EXPECT_TRUE(std::string(writeNode.getChildNode("description").getText()).find("Currently    0 hits/s    0 B/s") 
        != std::string::npos) << "Error writing write statistics tag";
    EXPECT_EQ(std::string(writeNode.getChildNode("rate").getClear().lpszValue), "0") << "Error writing write rate tag";
    XMLNode totalNode = statisticsNode.getChildNode("read");
    EXPECT_TRUE(std::string(totalNode.getChildNode("description").getText()).find("Currently    0 hits/s    0 B/s") 
        != std::string::npos) << "Error writing total statistics tag";
    EXPECT_EQ(std::string(totalNode.getChildNode("rate").getClear().lpszValue), "0") << "Error writing total rate tag";
    
}

//test void add( DiskOperation *operation )
TEST(diskManagerTest, addTest) {
    //engine::Engine::init();
    engine::DiskManager::init(10);
    char buffer[1024*1024];
    engine::DiskOperation* operation = engine::DiskOperation::newReadOperation( buffer , "test_filename.txt" , 0 , 1, 0 );
    engine::DiskManager::shared()->add(operation);

    EXPECT_EQ(engine::DiskManager::shared()->getNumOperations(), 1) << "Wrong number of disk operations";
    

}


//test void cancel( DiskOperation *operation );
TEST(diskManagerTest, cancelTest) {
    //engine::Engine::init();
    engine::DiskManager::init(10);
    char buffer[1024*1024];
    engine::DiskOperation* operation = engine::DiskOperation::newReadOperation( buffer , "test_filename.txt" , 0 , 1, 0 );
    engine::DiskManager::shared()->add(operation);
    EXPECT_EQ(engine::DiskManager::shared()->getNumOperations(), 1) << "Error adding operation";
    engine::DiskManager::shared()->cancel(operation);
    usleep(50000);
    EXPECT_EQ(engine::DiskManager::shared()->getNumOperations(), 0) << "Operation was not cancelled";

}

//test void run_worker();  (TODO: How??)
    
//test int getNumOperations();
TEST(diskManagerTest, getNumOperationsTest) {
    //add two operations and check that it returns 2
    //engine::Engine::init();
    engine::DiskManager::init(10);
    char buffer[1024*1024];
    engine::DiskOperation* operation = engine::DiskOperation::newReadOperation( buffer , "test_filename.txt" , 0 , 1, 0 );
    engine::DiskManager::shared()->add(operation);
    engine::DiskOperation* operation2 = engine::DiskOperation::newReadOperation( buffer , "test_filename.txt" , 0 , 1, 0 );
    engine::DiskManager::shared()->add(operation2);
    
    EXPECT_EQ(engine::DiskManager::shared()->getNumOperations(), 2) << "Error in getNumOperations()";
    
}
    
//test void setNumOperations( int _num_disk_operations );
TEST(diskManagerTest, setNumOperationsTest) {
    //engine::Engine::init();
    engine::DiskManager::init(10);

    ProcessStats pstats;
    long threadsBefore = pstats.get_nthreads();

    engine::DiskManager::shared()->setNumOperations(14);

    pstats.refresh();
    long threadsAfter = pstats.get_nthreads(); //should be threadsBefore+4, since we extended the number of operations from 10 to 14

    EXPECT_EQ(threadsAfter - threadsBefore, 4) << "Error in setNumOperations()";
    
}

//test void quitEngineService();
TEST(diskManagerTest, quitEngineServiceTest) {
    //engine::Engine::init();
    engine::DiskManager::init(10);
    char buffer[1024*1024];
    engine::DiskOperation* operation = engine::DiskOperation::newReadOperation( buffer , "test_filename.txt" , 0 , 1, 0 );
    engine::DiskManager::shared()->add(operation);

    ProcessStats pstats;
    unsigned long threadsBefore = pstats.get_nthreads();
    engine::DiskManager::shared()->quitEngineService();
    pstats.refresh();
    unsigned long threadsAfter = pstats.get_nthreads();
    
    //Now the number of threads should have been reduced
    EXPECT_TRUE(threadsAfter < threadsBefore);
}


 
