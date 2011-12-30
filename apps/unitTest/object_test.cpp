/* ****************************************************************************
*
* FILE            object_Test.cpp
*
* AUTHOR          Javier Lois
*
* DATE            December 2011
*
* DESCRIPTION
*
* unit testing of the Object class in the engine library
*
*/

#include "gtest/gtest.h"

#include "engine/Engine.h"
#include "engine/Object.h"

#include "xmlmarkup/xmlmarkup.h"

// Tests size_t getEngineId();
TEST(objecttest, getEngineIdTest) {
    engine::Engine::init();
    engine::Engine::shared()->reset();
    engine::Object object1("engine");
    engine::Object object2;
    engine::Object object3;
    
    //getEngineId() should return a non zero value, different for each onject
    size_t id1 = object1.getEngineId();
    size_t id2 = object2.getEngineId();
    size_t id3 = object3.getEngineId();
    
    EXPECT_TRUE(id1 != 0);
    EXPECT_TRUE(id1 != id2);
    EXPECT_TRUE(id1 != id3);
    EXPECT_TRUE(id2 != id3);
    
}


