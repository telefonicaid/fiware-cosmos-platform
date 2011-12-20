/* ****************************************************************************
*
* FILE            main_UnitTest.cpp
*
* AUTHOR          Javier Lois
*
* DATE            December 2011
*
* DESCRIPTION
*
* Main file for the automatic unit testing application
*
*/

#include "engine/MemoryManager.h"
#include "engine/Engine.h"
#include "engine/DiskManager.h"
#include "engine/ProcessManager.h"

#include "gtest/gtest.h"

//#include "main_UnitTest.h"


// Tests engine's instantiation
TEST(enginetest, instantiationtest) {
    //engine::Engine* test_engine = NULL;
    //access instance without initialise. Should return NULL.
    EXPECT_EQ(engine::Engine::shared(), static_cast<engine::Engine*>(NULL)); //using just NULL produces compilation error
    //call init() and then instance. Should return a valid one.
    engine::Engine::init();
    EXPECT_TRUE(engine::Engine::shared() != static_cast<engine::Engine*>(NULL)); 
//EXPECT_EQ (2,2);

}


/* ****************************************************************************
*
* main - 
*/

/*int main(int argC, const char *argV[])
{
    
engine::Engine* test_engine = NULL;
test_engine = engine::Engine::shared();

}*/
