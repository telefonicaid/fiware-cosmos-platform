/* ****************************************************************************
*
* FILE            objectsManager_Test.cpp
*
* AUTHOR          Javier Lois
*
* DATE            December 2011
*
* DESCRIPTION
*
* unit testing of the ObjectManager class in the engine library
*
*/

#include "gtest/gtest.h"

#include "engine/Engine.h"
#include "engine/Object.h"

#include "xmlmarkup/xmlmarkup.h"

//Test void add( Object* o );
//Test void remove( Object* o );
TEST(objectsmanagertest, addRemoveGetIdTest) {
    engine::Engine::init();
    engine::Engine::shared()->reset();

    engine::ObjectsManager om;
    
    engine::Object object1("object1");
    engine::Object object2("object2");

    om.add(&object1);
    om.add(&object2, "channel1");
    
    EXPECT_EQ(&object1, om.getObjectByName("object1")); 
    //object2 is in the channel, so getObjectByName must return NULL
    EXPECT_EQ(om.getObjectByName("object2"), static_cast<engine::Object*>(NULL));    
    
    om.remove(&object1);
    om.remove(&object2, "channel1");
    
    EXPECT_EQ(om.getObjectByName("object1"), static_cast<engine::Object*>(NULL)); 
    
}

