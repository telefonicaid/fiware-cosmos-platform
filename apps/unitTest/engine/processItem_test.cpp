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
//Test bool isReady(){ return true; };
//Test std::string getDescription();
//Test bool isRunning();
TEST(processItemTest, getStatusTest) {
    engine::Engine::init();

    ProcessItemExample item;
    
    EXPECT_EQ(item.getStatus(), "Q:5:unknown") << "Error getting status";
    EXPECT_TRUE(item.isReady()) << "Item should be ready";
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


//Test bool isRunning();
TEST(processItemTest, isRunningTest) {
    engine::Engine::init();

    ProcessItemExample item;;
    
    EXPECT_TRUE(!item.isRunning());
    item.unHalt();
    EXPECT_TRUE(!item.isRunning());
    
}

//Test void runInBackground();
/*TEST(processItemTest, runInBackgroundTest) {
    engine::Engine::init();
    engine::ProcessManager::init(10);
    engine::MemoryManager::init(1000);

    ProcessItemExample item;;
    
    ProcessStats pstats;
    size_t beforeThreads = pstats.get_nthreads();
    
    Todo: causes segmentation fault. Why?
    item.runInBackground();
    
    pstats.refresh();
    EXPECT_TRUE(pstats.get_nthreads() > beforeThreads );
    EXPECT_TRUE(item.isRunning());
    //engine::ProcessManager::shared()->cancel(item);
} */

   /* 
    
    void addListenerId( size_t _listenerId );
    
    
    // Get information for xml monitoring

*/

