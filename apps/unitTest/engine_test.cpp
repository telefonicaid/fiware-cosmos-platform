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

#include "au/ProcessStats.h"

#include "xmlparser/xmlParser.h"


// Tests engine's instantiation
TEST(engineTest, instantiationTest) {
    //get initial number of threads
    ProcessStats pstats;
    unsigned long beforeThreads = pstats.get_nthreads();
    //access instance without initialise. Should return NULL.
    EXPECT_EQ(engine::Engine::shared(), static_cast<engine::Engine*>(NULL)) << "Uninitialized engine should be null"; //using just NULL produces compilation error
    //call init() and then instance. Should return a valid one.
    engine::Engine::init();
    EXPECT_TRUE(engine::Engine::shared() != static_cast<engine::Engine*>(NULL)) << "engine instance should not be null after instantiation"; 
    //get number of threads now. Should be bigger. This tests run()
    pstats.refresh();
    unsigned long afterThreads = pstats.get_nthreads();
    EXPECT_TRUE(afterThreads > beforeThreads);
}

//test run()
//we can consider that if it passed the threads test in instantiationTest, that means that the run() thread is running

//test get_info( std::ostringstream& output)
TEST(engineTest, getInfoTest) {
    engine::Engine::init();

    std::ostringstream info;
    engine::Engine::shared()->getInfo( info );
    //std::cout << info.str() << std::endl;
    
    //XML parsing class
    XMLNode xMainNode=XMLNode::parseString(info.str().c_str(),"engine");
    //Loops value is not testable because it varies with time, but we can at least check that it is set
    EXPECT_TRUE(!xMainNode.getChildNode("loops").isEmpty()) << "Error writing loops tag";
    EXPECT_EQ(std::string(xMainNode.getChildNode("running_element").getClear().lpszValue), "No running element") << "Error writing running element tag";
    XMLNode elementNode = xMainNode.getChildNode("elements").getChildNode("engine_element");
    ASSERT_TRUE(!elementNode.isEmpty());
    std::string tmpString = std::string(elementNode.getChildNode("short_description").getClear().lpszValue);
    EXPECT_TRUE( tmpString.find("[ EngineElement in ") != std::string::npos
                 && tmpString.find(" secs ( repetition count:0 delay:10 ) ] Not:[ Not: alive]") != std::string::npos )
        << "Error writing short_description tag";
    tmpString = std::string(elementNode.getChildNode("description").getClear().lpszValue);
    EXPECT_TRUE( tmpString.find("[ Notification [ Notification alive Targets: () () ] repeated count:0 time:") != std::string::npos
                 && tmpString.find(" delay:10 ] Not:[ Not: alive]") != std::string::npos )
        << "Error writing description tag";
    //Uptime value is not testable because it varies with time, but we can at least check that it is set
    EXPECT_TRUE(!xMainNode.getChildNode("uptime").isEmpty()) << "Error writing uptime tag";
    
}

//notify( Notification*  notification )
TEST(engineTest, notificationTest) {
    engine::Engine::init();
    engine::Notification* notification1 = new engine::Notification("test_notification1");
    engine::Notification* notification2 = new engine::Notification("test_notification2");
    engine::Engine::shared()->notify(notification1);
    engine::Engine::shared()->notify(notification2, 3);
    //get xml info and check it there
    std::ostringstream info;
    engine::Engine::shared()->getInfo( info );
    //std::cout << info.str() << std::endl;

    XMLNode xMainNode=XMLNode::parseString(info.str().c_str(),"engine");
    XMLNode element1Node = xMainNode.getChildNode("elements").getChildNode("engine_element",0);
    XMLNode element2Node = xMainNode.getChildNode("elements").getChildNode("engine_element",1);
    ASSERT_TRUE(!element1Node.isEmpty() && !element2Node.isEmpty());
    //If the order was not right, swap them
    if(std::string(element1Node.getChildNode("description").getClear().lpszValue).find("test_notification1") == false)
    {
        XMLNode tmp = element1Node;
        element1Node = element2Node;
        element2Node = tmp; 
    }


    //First engine_element will be notification test_notification1
    EXPECT_TRUE(std::string(element1Node.getChildNode("description").getClear().lpszValue).find("test_notification1")) 
        << "notification not registered" ;
    EXPECT_TRUE(std::string(element2Node.getChildNode("description").getClear().lpszValue).find("test_notification2")) 
        << "notification not registered" ;
    
    //delete notification1;
    //delete notification2;
}


//void add( EngineElement *element );	
TEST(engineTest, addTest) {
    engine::Engine::init();
    engine::EngineElementSleepTest* testElement = new engine::EngineElementSleepTest();
    engine::Engine::shared()->add(testElement);
    
    //get info and check that the element was added
    std::ostringstream info;
    engine::Engine::shared()->getInfo( info );

    XMLNode xMainNode=XMLNode::parseString(info.str().c_str(),"engine");
 
    //Find the corresponding element in the list of elements in the engine
    bool found = false;
    int i = 0;
    XMLNode elementNode = xMainNode.getChildNode("elements").getChildNode("engine_element",i);
    while(!elementNode.isEmpty() && !found)
    {
        found = std::string(elementNode.getChildNode("description").getClear().lpszValue).find("Sleep element just to sleep 10 seconds");
        i++;
        if (!found) elementNode = xMainNode.getChildNode("elements").getChildNode("engine_element",i);
    }
    
    EXPECT_TRUE( found ) << "EngineElement not added" ;

    //delete testElement;
}   


//Object* getObjectByName( const char *name );
TEST(engineTest, getObjectByNameTest) {
     engine::Engine::init();
    engine::Object* testObject = new engine::Object("test_object");
    //Now the object should be registered in engine
    EXPECT_EQ(engine::Engine::shared()->getObjectByName("test_object"), testObject);
    
    //delete testObject;

}   


//quitEngineService()
TEST(engineTest, quitEngineServiceTest) {
    engine::Engine::init();

    //Check the number of threads. After the call to quitEngineService the number of threads should decrease
    ProcessStats pstats;
    unsigned long beforeThreads = pstats.get_nthreads();
    
    engine::Engine::shared()->quitEngineService();

    pstats.refresh();
    unsigned long afterThreads = pstats.get_nthreads();

    EXPECT_TRUE(afterThreads < beforeThreads);
}

