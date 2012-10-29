/* ****************************************************************************
*
* FILE            diskManagerTest.cpp
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
#include <sys/time.h>
#include <pthread.h>

#include "gtest/gtest.h"
#include "logMsg/logMsg.h"

#include "engine/DiskManager.h"
#include "engine/DiskOperation.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "unitTest/common_engine_test.h"



// -----------------------------------------------------------------------------
// Test DiskManager's instantiation
//
TEST(engine_DiskManager, instantiationTest)
{
    init_engine_test();

    // call init() and then shared(). Should return a valid one.
    ASSERT_TRUE(engine::Engine::disk_manager() != static_cast<engine::DiskManager *>(NULL))
        << "DiskManager instance should not be null after instantiation";
    
    close_engine_test();
}



// -----------------------------------------------------------------------------
// rates
//
TEST(engine_DiskManager, rates)
{
    init_engine_test();

    char buffer[1024 * 1024];

    au::SharedPointer<engine::DiskOperation> operation(engine::DiskOperation::newReadOperation(buffer, "test_filename.txt", 0, 1, 0));

    size_t rate_in = engine::Engine::disk_manager()->rate_in();
    EXPECT_EQ(0, rate_in);

    size_t rate_out = engine::Engine::disk_manager()->rate_out();
    EXPECT_EQ(0, rate_out);

    double rate_ops_in = engine::Engine::disk_manager()->rate_operations_in();
    EXPECT_EQ(0.0, rate_ops_in);

    double rate_ops_out = engine::Engine::disk_manager()->rate_operations_out();
    EXPECT_EQ(0.0, rate_ops_out);

    double on_off_activity = engine::Engine::disk_manager()->on_off_activity();
    EXPECT_EQ(0.0, on_off_activity);

    close_engine_test();
}


//
// times
//
TEST(engine_DiskManager, times)
{
    init_engine_test();

    engine::Engine::disk_manager()->on_time();
    engine::Engine::disk_manager()->off_time();

    int workers = engine::Engine::disk_manager()->num_disk_manager_workers();
    EXPECT_EQ(1, workers);
    // What can I check against ... ?

    engine::Engine::disk_manager()->set_max_num_disk_operations(3);

    close_engine_test();
}



// -----------------------------------------------------------------------------
// test void add( DiskOperation *operation )
//
TEST(engine_DiskManager, addTest) {
  init_engine_test();

  LM_M(("Starting addTest"));

  class A : public engine::NotificationListener {
    au::Token token;
  public:
    A() : token("Test A") {}

    void test() {
      LM_M(("In test"));
      au::TokenTaker tt(&token);

      LM_M(("In test"));
      char buffer[1024 * 1024];
      au::SharedPointer<engine::DiskOperation> operation(engine::DiskOperation::newReadOperation(buffer, "test_filename.txt", 0, 1, 0));

      LM_M(("Calling Add"));
      engine::Engine::disk_manager()->Add(operation);
      LM_M(("Called Add"));
      EXPECT_EQ(engine::Engine::disk_manager()->num_disk_operations(), 1) << "Wrong number of disk operations";

      LM_M(("Calling Cancel"));
      engine::Engine::disk_manager()->Cancel(operation);
      EXPECT_EQ(engine::Engine::disk_manager()->num_disk_operations(), 0) << "Wrong number of disk operations";

      LM_M(("Calling Add"));
      engine::Engine::disk_manager()->Add(operation);
      EXPECT_EQ(engine::Engine::disk_manager()->num_disk_operations(), 1) << "Wrong number of disk operations";

      // This should run as a separate thread ...
      // LM_M(("Calling run_worker"));
      // engine::Engine::disk_manager()->run_worker();
      // EXPECT_EQ(engine::Engine::disk_manager()->num_disk_operations(), 0) << "Wrong number of disk operations";

      // engine::Engine::DestroyEngine();
    }

    void notify(engine::Notification *notification) { au::TokenTaker tt(&token); }
  };

  //
  // Instantiate and test
  //
  A *a = new A();
  a->test();
  delete a;

  close_engine_test();
}



// -----------------------------------------------------------------------------
// diskOperations (and workers)
//
TEST(engine_DiskManager, diskOperations)
{
    init_engine_test();

    char buffer[1024 * 1024];


    au::SharedPointer<engine::DiskOperation> operation1(engine::DiskOperation::newReadOperation(buffer, "test_filename.txt", 0, 1, 0));
    au::SharedPointer<engine::DiskOperation> operation2(engine::DiskOperation::newReadOperation(buffer, "test_filename.txt", 0, 1, 0));
    au::SharedPointer<engine::DiskOperation> operation3(engine::DiskOperation::newReadOperation(buffer, "test_filename.txt", 0, 1, 0));
    au::SharedPointer<engine::DiskOperation> operation4(engine::DiskOperation::newReadOperation(buffer, "test_filename.txt", 0, 1, 0));
    au::SharedPointer<engine::DiskOperation> operation5(engine::DiskOperation::newReadOperation(buffer, "test_filename.txt", 0, 1, 0));

    engine::Engine::disk_manager()->Add(operation1);
    EXPECT_EQ(engine::Engine::disk_manager()->num_disk_operations(), 1) << "Wrong number of disk operations";

    engine::Engine::disk_manager()->Add(operation2);
    EXPECT_EQ(engine::Engine::disk_manager()->num_disk_operations(), 2) << "Wrong number of disk operations";

    engine::Engine::disk_manager()->Add(operation3);
    EXPECT_EQ(engine::Engine::disk_manager()->num_disk_operations(), 3) << "Wrong number of disk operations";

    engine::Engine::disk_manager()->Add(operation4);
    EXPECT_EQ(engine::Engine::disk_manager()->num_disk_operations(), 4) << "Wrong number of disk operations";

    engine::Engine::disk_manager()->Add(operation5);
    EXPECT_EQ(engine::Engine::disk_manager()->num_disk_operations(), 5) << "Wrong number of disk operations";

    // engine::Engine::disk_manager()->getNextDiskOperation();  private ...

    close_engine_test();
}




void* diskOperations(void* x)
{
    engine::DiskManager* dmP = (engine::DiskManager*) x;

    dmP->run_worker();

    return NULL;
}



// -----------------------------------------------------------------------------
// run_worker
//
TEST(engine_DiskManager, run_worker) {
  engine::DiskManager* dmP;

  init_engine_test();

  char buffer[1024 * 1024];

  dmP = engine::Engine::disk_manager();

  au::SharedPointer<engine::DiskOperation> operation1(engine::DiskOperation::newReadOperation(buffer, "test_filename.txt", 0, 1, 0));
  dmP->Add(operation1);
  EXPECT_EQ(dmP->num_disk_operations(), 1) << "Wrong number of disk operations";

#if 0  // This test doesn't work ... I need Andreu to tell me why ...
  au::SharedPointer<engine::DiskOperation> operation2(engine::DiskOperation::newReadOperation(buffer, "test_filename.txt", 0, 1, 0));
  au::SharedPointer<engine::DiskOperation> operation3(engine::DiskOperation::newReadOperation(buffer, "test_filename.txt", 0, 1, 0));
  au::SharedPointer<engine::DiskOperation> operation4(engine::DiskOperation::newReadOperation(buffer, "test_filename.txt", 0, 1, 0));
  au::SharedPointer<engine::DiskOperation> operation5(engine::DiskOperation::newReadOperation(buffer, "test_filename.txt", 0, 1, 0));

  dmP->Add(operation2);
  EXPECT_EQ(dmP->num_disk_operations(), 2) << "Wrong number of disk operations";

  dmP->Add(operation3);
  EXPECT_EQ(dmP->num_disk_operations(), 3) << "Wrong number of disk operations";

  dmP->Add(operation4);
  EXPECT_EQ(dmP->num_disk_operations(), 4) << "Wrong number of disk operations";

  dmP->Add(operation5);
  EXPECT_EQ(dmP->num_disk_operations(), 5) << "Wrong number of disk operations";

  pthread_t  pt;
  int        s;

  s = pthread_create(&pt, NULL, diskOperations, dmP);

  // await all threads to die (close_engine_test() should be enough)
  sleep(3);
  delete dmP;
#endif

  close_engine_test();
}
