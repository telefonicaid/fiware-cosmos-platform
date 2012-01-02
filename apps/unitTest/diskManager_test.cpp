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

#include "xmlmarkup/xmlmarkup.h"

// Test DiskManager's instantiation
TEST(diskManagerTest, instantiationTest) {
    //access instance without initialise. Should return NULL.
    EXPECT_EQ(engine::DiskManager::shared(), static_cast<engine::DiskManager*>(NULL)) 
              << "Uninitialized DiskManager should be null"; //using just NULL produces compilation error
    //call init() and then shared(). Should return a valid one.
    engine::DiskManager::init(3);
    ASSERT_TRUE(engine::DiskManager::shared() != static_cast<engine::DiskManager*>(NULL)) 
                << "DiskManager instance should not be null after instantiation"; 
}

//test void add( DiskOperation *operation )
TEST(diskManagerTest, addTest) {
    engine::Engine::init();
    engine::DiskManager::reset();
    engine::DiskManager::init(3);
    char buffer[1024*1024];
    engine::DiskOperation* operation = engine::DiskOperation::newReadOperation( buffer , "test_filename.txt" , 0 , 1, 0 );
    engine::DiskManager::shared()->add(operation);

    std::ostringstream info;
    //engine::DiskManager::shared()->getInfo( info );
    //std::cout << info.str() << std::endl;
    
    EXPECT_EQ(engine::DiskManager::shared()->getNumOperations(), 1) << "Wrong number of disk operations";
    

}


//test void cancel( DiskOperation *operation );
TEST(diskManagerTest, cancelTest) {
    engine::Engine::init();
    engine::DiskManager::reset();
    engine::DiskManager::init(3);
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
    engine::Engine::init();
    engine::DiskManager::reset();
    engine::DiskManager::init(3);
    char buffer[1024*1024];
    engine::DiskOperation* operation = engine::DiskOperation::newReadOperation( buffer , "test_filename.txt" , 0 , 1, 0 );
    engine::DiskManager::shared()->add(operation);
    engine::DiskOperation* operation2 = engine::DiskOperation::newReadOperation( buffer , "test_filename.txt" , 0 , 1, 0 );
    engine::DiskManager::shared()->add(operation2);
    
    EXPECT_EQ(engine::DiskManager::shared()->getNumOperations(), 2) << "Error in getNumOperations()";
    
}
    
//test void getInfo( std::ostringstream& output);
TEST(diskManagerTest, getInfoTest) {
    engine::Engine::init();
    engine::DiskManager::reset();
    engine::DiskManager::init(3);
    char buffer[1024*1024];
    engine::DiskOperation* operation = engine::DiskOperation::newReadOperation( buffer , "test_filename.txt" , 0 , 1, 0 );
    engine::DiskManager::shared()->add(operation);

    std::ostringstream info;
    engine::DiskManager::shared()->getInfo( info );
    //std::cout << info.str() << std::endl;

    //XML parsing
    CMarkup xmlData( info.str() );
    xmlData.FindElem("disk_manager");
    xmlData.IntoElem();
    xmlData.FindElem("num_pending_operations");
    EXPECT_EQ(xmlData.GetData(), "1") << "Error writing pending operations tag";
    xmlData.FindElem("num_running_operations");
    EXPECT_EQ(xmlData.GetData(), "1") << "Error writing running operations tag";
    //xmlData.OutOfElem();
    xmlData.FindElem("queued");
    xmlData.IntoElem();
    xmlData.FindElem("disk_operation");
    xmlData.IntoElem();
    xmlData.FindElem("type");
    EXPECT_EQ(xmlData.GetData(), "read") << "Error writing type tag";
    xmlData.FindElem("file_name");
    EXPECT_EQ(xmlData.GetData(), "test_filename.txt") << "Error writing file_name tag";
    xmlData.FindElem("size");
    EXPECT_EQ(xmlData.GetData(), "1") << "Error writing size tag";
    xmlData.FindElem("offset");
    EXPECT_EQ(xmlData.GetData(), "0") << "Error writing offset tag";
    xmlData.OutOfElem();//disk_element
    xmlData.OutOfElem();//queued
    xmlData.FindElem("statistics");
    xmlData.IntoElem();
    xmlData.FindElem("read");
    xmlData.IntoElem();
    xmlData.FindElem("description");
    EXPECT_TRUE(xmlData.GetData().find("Currently    0 hits/s    0 B/s")) << "Error writing read statistics tag";
    xmlData.OutOfElem();//read
    xmlData.FindElem("write");
    xmlData.IntoElem();
    xmlData.FindElem("description");
    EXPECT_TRUE(xmlData.GetData().find("Currently    0 hits/s    0 B/s")) << "Error writing write statistics tag";
    xmlData.OutOfElem();//write
    xmlData.FindElem("total");
    xmlData.IntoElem();
    xmlData.FindElem("description");
    EXPECT_TRUE(xmlData.GetData().find("Currently    0 hits/s    0 B/s")) << "Error writing total statistics tag";
    xmlData.OutOfElem();//total

    
}
    
//test void setNumOperations( int _num_disk_operations );
TEST(diskManagerTest, setNumOperationsTest) {
    engine::Engine::init();
    engine::DiskManager::reset();
    engine::DiskManager::init(3);

    /*std::ostringstream info;
    engine::DiskManager::shared()->getInfo( info );
    std::cout << "BEFORE: " << info.str() << std::endl;
    */

    engine::DiskManager::shared()->setNumOperations(3);
    
    /*info.str("");
    engine::DiskManager::shared()->getInfo( info );
    std::cout  << std::endl << std::endl << "AFTER: " << info.str() << std::endl;
    */

    //EXPECT_EQ(engine::DiskManager::shared()->getNumOperations(), 3) << "Error in setNumOperations()";

    //TODO: how to get the number of threads created or number of disk manager workers
}

//test void quitEngineService();
TEST(diskManagerTest, quitEngineServiceTest) {
    engine::Engine::init();
    engine::DiskManager::reset();
    engine::DiskManager::init(3);
    char buffer[1024*1024];
    engine::DiskOperation* operation = engine::DiskOperation::newReadOperation( buffer , "test_filename.txt" , 0 , 1, 0 );
    engine::DiskManager::shared()->add(operation);

    /*std::ostringstream info;
    engine::DiskManager::shared()->getInfo( info );
    std::cout << "BEFORE: " << info.str() << std::endl;
    */
    
    engine::DiskManager::shared()->quitEngineService();
    //give time for the service to stop
    usleep(10000);
    
    //Now the number of pending operations should be 0
    //std::ostringstream info;
    /*info.str("");
    engine::DiskManager::shared()->getInfo( info );
    std::cout << "AFTER: " << info.str() << std::endl;
    */
    
    std::ostringstream info;
    engine::DiskManager::shared()->getInfo( info );
    CMarkup xmlData( info.str() );
    xmlData.FindElem("disk_manager");
    xmlData.IntoElem();
    xmlData.FindElem("num_pending_operations");
    //EXPECT_EQ(xmlData.GetData(), "0") << "DiskManager services not stopped with quitEngineService()";
    //TODO: How to test that the services have stopped?
    
        
}
 
