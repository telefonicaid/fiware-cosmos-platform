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

#include "engine/Engine.h"

#include "gtest/gtest.h"

// Tests engine's instantiation
TEST(enginetest, instantiationtest) {
    //engine::Engine* test_engine = NULL;
    //access instance without initialise. Should return NULL.
    EXPECT_EQ(engine::Engine::shared(), static_cast<engine::Engine*>(NULL)) << "Uninitialized engine should be null"; //using just NULL produces compilation error
    //call init() and then instance. Should return a valid one.
    engine::Engine::init();
    EXPECT_TRUE(engine::Engine::shared() != static_cast<engine::Engine*>(NULL)) << "engine instance should not be null after instantiation"; 


}


