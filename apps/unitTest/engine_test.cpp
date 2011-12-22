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

#include "engine/Engine.h"
#include "engine/Notification.h"
#include "engine/NotificationElement.h"
#include "engine/Object.h"
#include "xmlmarkup/xmlmarkup.h"


// Tests engine's instantiation
TEST(enginetest, instantiationtest) {
    //engine::Engine* test_engine = NULL;
    //access instance without initialise. Should return NULL.
    EXPECT_EQ(engine::Engine::shared(), static_cast<engine::Engine*>(NULL)) << "Uninitialized engine should be null"; //using just NULL produces compilation error
    //call init() and then instance. Should return a valid one.
    engine::Engine::init();
    EXPECT_TRUE(engine::Engine::shared() != static_cast<engine::Engine*>(NULL)) << "engine instance should not be null after instantiation"; 
}

//TODO tests

//test run()
TEST(enginetest, runTest) {
    //TODO: How can we test this??
    engine::Engine::init();
    //engine::Engine::shared()->run();
}

//test get_info( std::ostringstream& output)
TEST(enginetest, getInfoTest) {
    engine::Engine::init();
    //Create dummy xml ostringstream with the expected data to compare
 /*   std::ostringstream test_info;
    au::xml_open(test_info, "engine");
    
    au::xml_simple(test_info , "loops" , counter );
   
    au::xml_simple( output , "running_element" , running_element->getDescription() );
    //au::xml_simple( output , "running_element" , "No running element" );
    
    au::xml_iterate_list( test_info , "elements" , elements );
    
    au::xml_simple( test_info , "uptime" , uptime.diffTimeInSeconds() );
    
    au::xml_close(test_info , "engine");
    */
    std::ostringstream info;
    engine::Engine::shared()->getInfo( info );
    CMarkup xmlData( info.str() );
    std::cout << info.str() << std::endl;
    //if(xmlData.FindElem("engine_element"))
    //{
        std::cout << "Data: " << xmlData.GetData() << std::endl;
    //};
    std::cout << std::endl << std::endl << xmlData.GetDoc() << std::endl;
}

//notify( Notification*  notification )
TEST(enginetest, notificationTest) {
    engine::Engine::init();
    engine::Notification* notification1 = new engine::Notification("test_notification1");
    engine::Notification* notification2 = new engine::Notification("test_notification2");
    engine::Engine::shared()->notify(notification1);
    engine::Engine::shared()->notify(notification2, 1);
    //TODO: get info and check it is right
    EXPECT_TRUE(true);
    delete notification1;
    delete notification2;
}


//void add( EngineElement *element );	
TEST(enginetest, addTest) {
    engine::Engine::init();
    engine::Notification* notification = new engine::Notification("test_notification1");
    engine::NotificationElement* notificationElement1 = new engine::NotificationElement(notification);
    //engine::Engine::shared()->add(&notificationElement1);
    //TODO: get info and check it is right
    EXPECT_TRUE(true);
    //delete notification;
    delete notificationElement1;

   
}   

 
//Object* getObjectByName( const char *name );
TEST(enginetest, getObjectByNameTest) {
    engine::Engine::init();
    engine::Object* testObject = new engine::Object("test_object");
    //Now the object should be registered in engine
    //EXPECT_EQ(engine::Engine::shared()->getObjectByName("test_object"), &testObject);
    
    delete testObject;
}   


//quitEngineService() (TODO:llamar a getEngineServiceName() )
TEST(enginetest, quitEngineServiceTest) {
    engine::Engine::init();
    engine::Engine::shared()->quitEngineService();
    //Check that it is out of the run() loop TODO:How???
    //EXPECT_EQ(engine::Engine::shared()->
    EXPECT_TRUE(true);
}

