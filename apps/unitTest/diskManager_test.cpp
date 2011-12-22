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
TEST(dmtest, dminstantiationtest) {
    //access instance without initialise. Should return NULL.
    EXPECT_EQ(engine::DiskManager::shared(), static_cast<engine::DiskManager*>(NULL)) << "Uninitialized DiskManager should be null"; //using just NULL produces compilation error
    //call init() and then instance. Should return a valid one.
    engine::DiskManager::init(3);
    ASSERT_TRUE(engine::DiskManager::shared() != static_cast<engine::DiskManager*>(NULL)) << "DiskManager instance should not be null after instantiation"; 
    //EXPECT_EQ(engine::DiskManager::shared()->getNumOperations(), 3) << "Wrong number of disk operations";


}

// Test diskoperations add/remove
TEST(dmtest, dmdiskoperationtest) {
    EXPECT_TRUE(true);
}
