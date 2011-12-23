/* ****************************************************************************
*
* FILE            engineTest.cpp
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

#include "gtest/gtest.h"

#include "engine/DiskManager.h"


// Test engine's instantiation
TEST(diskManagerTest, instantiationtest) {
    //access instance without initialise. Should return NULL.
    EXPECT_EQ(engine::DiskManager::shared(), static_cast<engine::DiskManager*>(NULL)) 
              << "Uninitialized DiskManager should be null"; //using just NULL produces compilation error
    //call init() and then shared(). Should return a valid one.
    engine::DiskManager::init(3);
    ASSERT_TRUE(engine::DiskManager::shared() != static_cast<engine::DiskManager*>(NULL)) 
                << "DiskManager instance should not be null after instantiation"; 
    EXPECT_EQ(engine::DiskManager::shared()->getNumOperations(), 3) << "Wrong number of disk operations";


}

//test void add( DiskOperation *operation )
TEST(diskManagerTest, addtest) {
    engine::DiskManager::init(1);

    char buffer[10];
    engine::DiskOperation* operation = engine::DiskOperation::newReadOperation( buffer , "test_filename.txt" , 0 , 1, 0 );
    //engine::DiskManager::shared()->add(operation);

    std::ostringstream info;
    //engine::DiskManager::shared()->getInfo( info );
    std::cout << info.str() << std::endl;
    
    

}


//test void cancel( DiskOperation *operation );

//test void run_worker();  
    
//test int getNumOperations();
    
//test void getInfo( std::ostringstream& output);
    
//test void setNumOperations( int _num_disk_operations );

//test void quitEngineService();

 
