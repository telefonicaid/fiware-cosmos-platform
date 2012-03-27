/* ****************************************************************************
*
* FILE            processItem_Test.cpp
*
* AUTHOR          Javier Lois
*
* DATE            January 2012
*
* DESCRIPTION
*
* unit testing of the ProcessItem class in the engine library
*
*/

#include "gtest/gtest.h"

#include "engine/Engine.h"
#include "engine/ProcessItem.h"
#include "engine/ProcessManager.h"

#include "au/ProcessStats.h"

//ProcessItem is pure virtual. In order to test it we need to create a derived class

class ProcessItemExample : public engine::ProcessItem
{
public:

   ProcessItemExample() : engine::ProcessItem(5)
   {
   }
   
   void run()
   {
	  // Do nothing ;)
   }

};

#include "xmlparser/xmlParser.h"


//Test std::string getStatus();
//Test std::string getDescription();
//Test bool isRunning();
TEST(processItemTest, getStatusTest) {
    engine::Engine::init();

    ProcessItemExample item;
    
    EXPECT_EQ(item.getStatus(), "Q:5:unknown") << "Error getting status";
    EXPECT_EQ(item.getDescription(), "Process Item 'unknown' Status: Q:5:unknown") << "Error in description";
    EXPECT_TRUE(item.isRunning()==false) << "Process item is not supposed to be running at this point";

}

//Test void setCanceled();
//Test bool isProcessItemCanceled();
TEST(processItemTest, isCanceledTest) {
    engine::Engine::init();

    ProcessItemExample item;;
    
    EXPECT_TRUE(item.isProcessItemCanceled() == false) << "ProcessItem should not be canceled";
    item.setCanceled();
    EXPECT_TRUE(item.isProcessItemCanceled()) << "ProcessItem should be canceled now";
    
}

//void getInfo( std::ostringstream& output);
TEST(processItemTest, getInfoTest) {
    engine::Engine::init();

    ProcessItemExample item;;
    item.addListenerId(1);
    
    std::ostringstream info;
    item.getInfo( info );

    //XML parsing
    XMLNode xMainNode=XMLNode::parseString(info.str().c_str(),"process_item");
    EXPECT_EQ(std::string(xMainNode.getChildNode("time").getClear().lpszValue), " 00:00:00") << "Error writing time tag";
    EXPECT_EQ(std::string(xMainNode.getChildNode("progress").getText()), "0") << "Error writing progress tag";
    EXPECT_EQ(std::string(xMainNode.getChildNode("state").getText()), "queued") << "Error writing state tag";
    EXPECT_EQ(std::string(xMainNode.getChildNode("priority").getText()), "5") << "Error writing priority tag";
    EXPECT_TRUE(std::string(xMainNode.getChildNode("operation_name").getText()).find("unknown") != std::string::npos ) << "Error writing operation_name tag";
}


