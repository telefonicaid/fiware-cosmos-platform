/* ****************************************************************************
*
* FILE            operationsContainer_test.cpp
*
* AUTHOR          Javier Lois
*
* DATE            January 2012
*
* DESCRIPTION
*
* unit testing of the OperationsContainer class in the engine library
*
*/

//#include <direct.h> // for getcwd
#include <stdlib.h>// for MAX_PATH

#include "gtest/gtest.h"

#include "au/ProcessStats.h"
#include "engine/OperationsContainer.h"
#include "engine/DiskOperation.h"
#include "engine/DiskManager.h"
#include "engine/Engine.h"
#include "engine/MemoryManager.h"
#include "engine/ProcessItem.h"
#include "engine/MemoryRequest.h"

#include "xmlmarkup/xmlmarkup.h"

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

//Test void add( engine::MemoryRequest* memoryRequest );
TEST(operationsContainerTest, addMemoryRequestTest) {
    engine::Engine::init();
    engine::MemoryManager::init(1000);

    engine::OperationsContainer container;

    engine::MemoryRequest* request = new engine::MemoryRequest(10, 50.0, 1);

    container.add(request);
    container.erase(request);
    
    //TODO: how can we test this? there is no way to check from outside which memory request operations it contains

    delete request;
}

//Test void add( engine::DiskOperation *diskOperation );
TEST(operationsContainerTest, addDiskOperationTest) {
    engine::Engine::init();
    engine::OperationsContainer container;
    engine::DiskManager::init(1);
    char buffer[1024*1024];
    engine::DiskOperation* operation = engine::DiskOperation::newReadOperation( buffer , "test_filename.txt" , 3 , 5, 0 );

    
    container.add(operation);
    
    EXPECT_TRUE(container.getOperationsContainerStr().find("1 disk ops") != std::string::npos);
    EXPECT_TRUE(container.hasPendingEngineOperations());
    
    delete operation;
}

//Test void erase( engine::DiskOperation *diskOperation );
/* //Commented out for using DiskManager
TEST(operationsContainerTest, eraseDiskOperationTest) {
    engine::Engine::init();
    engine::OperationsContainer container;
    engine::DiskManager::init(1);
    char buffer[1024*1024];
    engine::DiskOperation* operation = engine::DiskOperation::newReadOperation( buffer , "test_filename.txt" , 3 , 5, 0 );

    container.add(operation);
    
    EXPECT_TRUE(container.getOperationsContainerStr().find("1 disk ops") != std::string::npos);
    EXPECT_TRUE(container.hasPendingEngineOperations());
    
    container.erase(operation);
    EXPECT_TRUE(container.getOperationsContainerStr().find("0 disk ops") != std::string::npos);
    EXPECT_TRUE(container.hasPendingEngineOperations() == false);

    delete operation;
}
*/
 
//Test void add( engine::ProcessItem *process_item );
TEST(operationsContainerTest, addProcessItemTest) {
    engine::Engine::init();
    engine::OperationsContainer container;
    

	ProcessItemExample item;
    
    container.add(&item);
    
    EXPECT_TRUE(container.getOperationsContainerStr().find("1 item ops") != std::string::npos);
    EXPECT_TRUE(container.hasPendingEngineOperations());
    
}


//Test void erase( engine::ProcessItem *process_item );
TEST(operationsContainerTest, eraseProcessItemTest) {
    engine::Engine::init();
    engine::OperationsContainer container;

	ProcessItemExample item;    
    
    container.add(&item);
    
    EXPECT_TRUE(container.getOperationsContainerStr().find("1 item ops") != std::string::npos);
    EXPECT_TRUE(container.hasPendingEngineOperations());
    
    container.erase(&item);
   
    EXPECT_TRUE(container.getOperationsContainerStr().find("0 item ops") != std::string::npos);
    EXPECT_TRUE(container.hasPendingEngineOperations() == false);
}

//Test bool hasPendingEngineOperations();
/* //Commented out because it uses  DiskOperation
TEST(operationsContainerTest, hasPendingEngineOperationsTest) {
    engine::Engine::init();
    engine::OperationsContainer container;

    EXPECT_TRUE(container.hasPendingEngineOperations() == false);
    
    engine::DiskManager::init(1);
    char buffer[1024*1024];
    engine::DiskOperation* operation = engine::DiskOperation::newReadOperation( buffer , "test_filename.txt" , 3 , 5, 0 );

	ProcessItemExample item;    
    
    container.add(operation);
    container.add(&item);
    
    EXPECT_TRUE(container.hasPendingEngineOperations());
    
    container.erase(operation);
    container.erase(&item);
   
    EXPECT_TRUE(container.hasPendingEngineOperations() == false);

    delete operation;
}
*/
    
//Test void cancelEngineOperations();
/* //Commented out because it uses  DiskOperation
TEST(operationsContainerTest, cancelEngineOperationsTest) {
    engine::Engine::init();
    engine::OperationsContainer container;
    engine::DiskManager::init(1);

    char buffer[1024*1024];
    engine::DiskOperation* operation = engine::DiskOperation::newReadOperation( buffer , "test_filename.txt" , 3 , 5, 0 );

	ProcessItemExample item;    
    
    container.add(operation);
    //container.add(&item);
    
    EXPECT_TRUE(container.hasPendingEngineOperations());

    container.cancelEngineOperations();

    //TODO: this function just sends the cancel signal but does not clear the operations list. How can we test this?
    //EXPECT_TRUE(container.hasPendingEngineOperations() == false);

    delete operation;
}
*/
    
//Test std::string getOperationsContainerStr();
/* //Commented out because it uses  DiskOperation
TEST(operationsContainerTest, getOperationsContainerStrTest) {
    engine::Engine::init();
    engine::OperationsContainer container;
    engine::DiskManager::init(1);

    char buffer[1024*1024];
    engine::DiskOperation* operation = engine::DiskOperation::newReadOperation( buffer , "test_filename.txt" , 3 , 5, 0 );

	ProcessItemExample item1;    
	ProcessItemExample item2;    
	ProcessItemExample item3;    
    
    container.add(operation);
    container.add(&item1);
    container.add(&item2);
    container.add(&item3);
    
    EXPECT_TRUE(container.getOperationsContainerStr().find("1 disk ops & 3 item ops") != std::string::npos);
    
}

*/
