/* ****************************************************************************
*
* FILE            endpointManager_test.cpp
*
* AUTHOR          Javier Lois
*
* DATE            January 2012
*
* DESCRIPTION
*
* unit testing of the samson/network library, EndpointManager class
*
*/

#include "gtest/gtest.h"

#include "samson/network/EndpointManager.h"

//Test constructor EndpointManager(Endpoint2::Type _type, const char* controllerIp = NULL);
TEST(endpointManagerTest, constructorTest) {
    samson::EndpointManager epm(samson::Endpoint2::Spawner);
    //Host Manager was created
    /*EXPECT_TRUE(epm.hostMgr != static_cast<samson::HostMgr*>(NULL));
    EXPECT_TRUE(epm.me != static_cast<samson::Endpoint2*>(NULL));
    EXPECT_TRUE(epm.controller != static_cast<samson::Endpoint2*>(NULL));
    EXPECT_TRUE(epm.listener != static_cast<samson::ListenerEndpoint*>(NULL));*/
    
}

TEST(endpointManagerTest, controllerConnectTest) {
    //connecting to a controller
    EXPECT_TRUE(true);
}

//Test void  initWorker();
TEST(endpointManagerTest, initWorkerTest) {
}

//Test void  initController();
TEST(endpointManagerTest, initControllerTest) {
}

//Test void  initDelilah(const char* controllerIp);
TEST(endpointManagerTest, initDelilahTest) {
}

//Test void  initSupervisor();
TEST(endpointManagerTest, initSupervisorTest) {
}

//Test void  initStarter();
TEST(endpointManagerTest, initStarterTest) {
}

//Test int   procVecSet(ProcessVector* _procVec, bool save = true);
TEST(endpointManagerTest, procVecSetTest) {
}


//Test ProcessVector*     procVecGet(void);

//Test void  timeout(void);
//Test void  periodic(void);
//Test void  workersAdd(void);
//Test void  workersConnect(void);
//Test void  controllerConnect(void);

//Test Endpoint2*  add(Endpoint2* ep);
//Test Endpoint2*  add(Endpoint2::Type type, int id, Host* host, unsigned short port, int rFd = -1, int wFd = -1);
//Test void  remove(Endpoint2* ep);
//Test Endpoint2*  get(unsigned int index);
//Test Endpoint2*  get(unsigned int index, int* rFdP);
//Test Endpoint2*  lookup(Endpoint2::Type type, const char* ip);
//Test Endpoint2*  lookup(Endpoint2::Type type, Host* host);
//Test Endpoint2*  lookup(Endpoint2::Type type, int id = 0, int* ixP = NULL);
//Test int   ixGet(Endpoint2* ep);
//Test Endpoint2*  indexedGet(unsigned int ix);
//Test void  show(const char* why, bool forced = false);

//Test int   endpointCount(Endpoint2::Type type);
//Test int   endpointCount();
//Test int   endpointCapacity();
    
//Test void  tmoSet(int secs, int usecs);  // Set timeout for select loop
//Test void  run(bool oneShot);            // Main run loop - loops forever, unless 'oneShot' is true ...
//Test void  callbackSet(CallbackId id, EpMgrCallback func, void* userParam);

//Test void  setPacketReceiver(PacketReceiverInterface* receiver);

//Test void  send(int endpointIx, Packet* packetP);
//Test int   multiSend(Endpoint2::Type type, Packet* packetP);
//Test int   multiSend(Endpoint2::Type typ, Message::MessageCode code, void* dataP = NULL, int dataLen = 0);
//Test void  pingTest(void);

