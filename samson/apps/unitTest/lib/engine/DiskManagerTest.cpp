/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
/* ****************************************************************************
 *
 * FILE            DiskManagerTest.cpp
 *
 * AUTHOR          Javier Lois
 *
 * DATE            December 2011
 *
 * DESCRIPTION
 *
 * Unit testing of the DiskManager class in the engine library
 *
 *
 * Telefonica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefonica Investigacion y Desarrollo S.A.U.
 * All rights reserved.
 */
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include "gtest/gtest.h"

#include "au/file.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "engine/DiskManager.h"
#include "engine/DiskOperation.h"
#include "logMsg/logMsg.h"
#include "unitTest/common_engine_test.h"

typedef au::SharedPointer<engine::DiskOperation>   SharedDiskOp;

// -----------------------------------------------------------------------------
// rates -
//
TEST(engine_DiskManager, rates) {
  init_engine_test();

  std::string test_filename = au::GetRandomTmpFileOrDirectory();

  char *buffer = reinterpret_cast<char *>(malloc(1024 * 1024));

  SharedDiskOp operation(engine::DiskOperation::newReadOperation(buffer, test_filename, 0, 1, 0));

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

  free(buffer);
  close_engine_test();
}

// -----------------------------------------------------------------------------
// times -
//
TEST(engine_DiskManager, times) {
  init_engine_test();

  double on_time  = engine::Engine::disk_manager()->on_time();
  EXPECT_EQ(0, on_time);

  double off_time = engine::Engine::disk_manager()->off_time();
  EXPECT_GE(1, off_time);

  int workers = engine::Engine::disk_manager()->num_disk_manager_workers();
  EXPECT_EQ(1, workers);

  engine::Engine::disk_manager()->set_max_num_disk_operations(3);
  EXPECT_EQ(3, engine::Engine::disk_manager()->max_num_disk_operations());

  close_engine_test();
}

// -----------------------------------------------------------------------------
// addTest -
//
TEST(engine_DiskManager, addTest) {
  init_engine_test();

  class A : public engine::NotificationListener {
    au::Token token;

public:
    A() : token("Test A") {
    }

    void test() {
      au::TokenTaker tt(&token);
      char *buffer = reinterpret_cast<char *>(malloc(1024 * 1024));

      std::string test_filename = au::GetRandomTmpFileOrDirectory();

      EXPECT_TRUE(buffer != NULL);
      au::SharedPointer<engine::DiskOperation> operation;
      operation = engine::DiskOperation::newReadOperation(buffer, test_filename, 0, 1, 0);

      engine::Engine::disk_manager()->Add(operation);
      EXPECT_EQ(engine::Engine::disk_manager()->num_disk_operations(), 1) << "Wrong number of disk operations";

      engine::Engine::disk_manager()->Cancel(operation);
      usleep(50000);
      EXPECT_EQ(engine::Engine::disk_manager()->num_disk_operations(), 0) << "Wrong number of disk operations";

      engine::Engine::disk_manager()->Add(operation);
      EXPECT_EQ(engine::Engine::disk_manager()->num_disk_operations(), 1) << "Wrong number of disk operations";

      free(buffer);
    }

    void notify(engine::Notification *notification) {
      au::TokenTaker tt(&token);
    }
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
// diskOperations - test Disk Operations and workers
//
TEST(engine_DiskManager, diskOperations) {
  init_engine_test();

  std::string test_filename = au::GetRandomTmpFileOrDirectory();
  char *buffer = reinterpret_cast<char *>(malloc(1024 * 1024));

  au::SharedPointer<engine::DiskOperation> operation1(
    engine::DiskOperation::newReadOperation(buffer, test_filename, 0, 1, 0));
  au::SharedPointer<engine::DiskOperation> operation2(
    engine::DiskOperation::newReadOperation(buffer, test_filename, 0, 1, 0));
  au::SharedPointer<engine::DiskOperation> operation3(
    engine::DiskOperation::newReadOperation(buffer, test_filename, 0, 1, 0));
  au::SharedPointer<engine::DiskOperation> operation4(
    engine::DiskOperation::newReadOperation(buffer, test_filename, 0, 1, 0));
  au::SharedPointer<engine::DiskOperation> operation5(
    engine::DiskOperation::newReadOperation(buffer, test_filename, 0, 1, 0));

  engine::Engine::disk_manager()->Add(operation1);
  EXPECT_EQ(engine::Engine::disk_manager()->num_disk_operations(), 1) <<
  "Wrong number of disk operations";

  engine::Engine::disk_manager()->Add(operation2);
  EXPECT_EQ(engine::Engine::disk_manager()->num_disk_operations(), 2) <<
  "Wrong number of disk operations";

  engine::Engine::disk_manager()->Add(operation3);
  EXPECT_EQ(engine::Engine::disk_manager()->num_disk_operations(), 3) <<
  "Wrong number of disk operations";

  engine::Engine::disk_manager()->Add(operation4);
  EXPECT_EQ(engine::Engine::disk_manager()->num_disk_operations(), 4) <<
  "Wrong number of disk operations";

  engine::Engine::disk_manager()->Add(operation5);
  EXPECT_EQ(engine::Engine::disk_manager()->num_disk_operations(), 5) <<
  "Wrong number of disk operations";

  // engine::Engine::disk_manager()->getNextDiskOperation();  private ...

  free(buffer);
  close_engine_test();
}

// -----------------------------------------------------------------------------
// run_worker
//
TEST(engine_DiskManager, run_worker) {
  init_engine_test();

  char         *buffer       = reinterpret_cast<char *>(malloc(1024 * 1024));
  std::string  test_filename = au::GetRandomTmpFileOrDirectory();
  int          fd            = open(test_filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0744);
  const char  *file_content  = "0123456789";
  int          content_len   = strlen(file_content);

  EXPECT_TRUE(fd != -1) << au::str("open(%s): %s", test_filename.c_str(), strerror(errno));
  if (fd != -1) {
    int nb = write(fd, file_content, content_len);
    EXPECT_EQ(strlen(file_content), nb);
  }

  engine::DiskManager *disk_manager = engine::Engine::disk_manager();

  au::SharedPointer<engine::DiskOperation> operation1(
    engine::DiskOperation::newReadOperation(buffer, test_filename, 0, content_len, 0));

  disk_manager->Add(operation1);
  au::Cronometer c;
  while (true) {
     if (disk_manager->num_disk_operations() == 0)
        break;
     if (c.seconds() > 1) {
        EXPECT_TRUE(false);
        return;
     }
  }
  buffer[content_len] = 0;

  if (operation1->error.HasErrors())
     LM_M(("operation1 error: %s", operation1->error.GetLastError().c_str()));

  EXPECT_FALSE(operation1->error.HasErrors());
  EXPECT_STREQ(file_content, buffer);

  free(buffer);

  close_engine_test();
  unlink(test_filename.c_str());
}
