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
#include "engine/EngineService.h"
#include "engine/Notification.h"
#include "engine/NotificationElement.h"
#include "engine/Object.h"

#include "xmlmarkup/xmlmarkup.h"


// Tests engine's instantiation
TEST(enginetest, instantiationtest) {
    //access instance without initialise. Should return NULL.
    EXPECT_EQ(engine::Engine::shared(), static_cast<engine::Engine*>(NULL)) << "Uninitialized engine should be null"; //using just NULL produces compilation error
    //call init() and then instance. Should return a valid one.
    engine::Engine::init();
    EXPECT_TRUE(engine::Engine::shared() != static_cast<engine::Engine*>(NULL)) << "engine instance should not be null after instantiation"; 
    
}

//test run()
TEST(enginetest, runTest) {
    //TODO: How can we test this??
    engine::Engine::shared()->reset();
    engine::Engine::init();
    //engine::Engine::shared()->run();
}

//test get_info( std::ostringstream& output)
TEST(enginetest, getInfoTest) {
    engine::Engine::shared()->reset();
    engine::Engine::init();

    std::ostringstream info;
    engine::Engine::shared()->getInfo( info );
    //std::cout << info.str() << std::endl;
    
    //XML parsing class
    CMarkup xmlData( info.str() );
    xmlData.FindElem();
    xmlData.IntoElem();
    //Loops is not testable because it can vary
    //xmlData.FindElem("loops");
    //EXPECT_EQ(xmlData.GetData(), "1") << "Error writing loops tag";
    xmlData.FindElem("running_element");
    //std::cout << "running_element: " << xmlData.GetData() << std::endl;
    EXPECT_EQ(xmlData.GetData(), "No running element") << "Error writing running element tag";
    xmlData.FindElem("elements");
    xmlData.IntoElem(); 
    xmlData.FindElem("engine_element");
    xmlData.IntoElem(); //engine_element 
    xmlData.FindElem("short_description");
    //std::cout << "short description: " << xmlData.GetData() << std::endl;
    EXPECT_EQ(xmlData.GetData(), "[ EngineElement in 10.00 secs ( repetition count:0 delay:10 ) ] Not:[ Not: alive]") << \
                      "Error writing short_description tag";
    xmlData.FindElem("description");
    //std::cout << "description: " << xmlData.GetData() << std::endl;
    EXPECT_EQ(xmlData.GetData(), 
              "[ Notification [ Notification alive Targets: () () ] repeated count:0 time:10.00 delay:10 ] Not:[ Not: alive]")
               << "Error writing description tag";
    xmlData.OutOfElem();
    xmlData.OutOfElem();
    xmlData.FindElem("uptime");
    //std::cout << "uptime: " << xmlData.GetData() << std::endl;
    EXPECT_EQ(xmlData.GetData(), "0") << "Error writing uptime tag";
    
    //std::cout << std::endl << std::endl << xmlData.GetDoc() << std::endl;
}

//notify( Notification*  notification )
TEST(enginetest, notificationTest) {
    engine::Engine::shared()->reset();
    engine::Engine::init();
    engine::Notification* notification1 = new engine::Notification("test_notification1");
    engine::Notification* notification2 = new engine::Notification("test_notification2");
    engine::Engine::shared()->notify(notification1);
    engine::Engine::shared()->notify(notification2, 3);
    //get info and check it is right
    std::ostringstream info;
    engine::Engine::shared()->getInfo( info );
    //std::cout << info.str() << std::endl;

    CMarkup xmlData( info.str() );
    xmlData.FindElem();
    xmlData.IntoElem();
 
    //First engine_element will be notification test_notification1
    xmlData.FindElem("elements");
    xmlData.IntoElem(); 
    xmlData.FindElem("engine_element");
    xmlData.IntoElem(); //engine_element 
    xmlData.FindElem("description");
    EXPECT_TRUE(xmlData.GetData().find("test_notification1") != std::string::npos ) << "notification not registered" ;
    xmlData.OutOfElem();
    //Second engine_element will be notification test_notification2
    xmlData.FindElem("engine_element");
    xmlData.IntoElem(); //engine_element 
    xmlData.FindElem("description");
    EXPECT_TRUE(xmlData.GetData().find("test_notification2") != std::string::npos ) << "notification not registered" ;
    
    //delete notification1;
    //delete notification2;
}


//void add( EngineElement *element );	
TEST(enginetest, addTest) {
    engine::Engine::shared()->reset();
    engine::Engine::init();
    engine::EngineElementSleepTest* testElement = new engine::EngineElementSleepTest();
    //engine::Notification* notification = new engine::Notification("test_notification");
    //engine::NotificationElement* notificationElement1 = new engine::NotificationElement(notification);
    engine::Engine::shared()->add(testElement);
    
    //get info and check that the element was added
    std::ostringstream info;
    engine::Engine::shared()->getInfo( info );

    CMarkup xmlData( info.str() );
    xmlData.FindElem();
    xmlData.IntoElem();
 
    //testelement is the second element in the engine, so we look for it
    xmlData.FindElem("elements");
    xmlData.IntoElem(); 
    xmlData.FindElem("engine_element");
    xmlData.FindElem("engine_element");
    xmlData.IntoElem(); 
    xmlData.FindElem("description");
    EXPECT_TRUE(xmlData.GetData().find("Sleep element just to sleep 10 seconds") != std::string::npos ) << "EngineElement not added" ;

    //std::cout << "Probando add(): " << info.str() << std::endl;
    
    EXPECT_TRUE(true);
    //delete testElement;

   
}   


//Object* getObjectByName( const char *name );
TEST(enginetest, getObjectByNameTest) {
    engine::Engine::shared()->reset();
     engine::Engine::init();
    engine::Object* testObject = new engine::Object("test_object");
    //Now the object should be registered in engine
    EXPECT_EQ(engine::Engine::shared()->getObjectByName("test_object"), testObject);
    
    //delete testObject;

}   


//quitEngineService()
TEST(enginetest, quitEngineServiceTest) {
    engine::Engine::shared()->reset();
    engine::Engine::init();

    std::ostringstream info;
    engine::Engine::shared()->getInfo( info );
    //std::cout << info.str() << std::endl << std::endl;
    
    engine::Engine::shared()->quitEngineService();

    engine::Engine::shared()->getInfo( info );
    //std::cout << std::endl << std::endl << info.str() << std::endl;

    //Check that it is out of the run() loop TODO:How???
    //EXPECT_EQ(engine::Engine::shared()->
    //EXPECT_TRUE(true);


}

