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
#ifndef _H_PROCESS_ITEM_ISOLATED
#define _H_PROCESS_ITEM_ISOLATED

#include <string>

#include "engine/ProcessItem.h"                // samson::ProcessItem

// samson::network
#include "samson/module/OperationController.h"  // samson::Tracer
#include "samson/module/Tracer.h"              // samson::Tracer
#include "samson/stream/SharedMemoryItem.h"    // engine::SharedMemoryItem
#include "samson/stream/WorkerTaskBase.h"

namespace samson {
class ProcessItemIsolated : public stream::WorkerTaskBase, public Tracer, public OperationController {
public:
  // Constructor
  ProcessItemIsolated(SamsonWorker *samson_worker, size_t worker_task_id, const std::string& operation,
                      const std::string& concept);

  // Destructor ( free the shared memory segment used internally )
  ~ProcessItemIsolated();

  // Virtual methods of engine::ProcessItem
  virtual void run();

  // Function to be implemented ( running on a different process )
  virtual void initProcessItemIsolated() = 0;
  virtual void runIsolated() = 0;
  virtual void finishProcessItemIsolated() = 0;

  // Function executed at this process side when a code is sent from the background process
  // The returned value is the code send to the isolated process back ( typically continue or kill )
  virtual void runCode(int c) = 0;

  // Functions used inside runIsolated
  void sendCode(int c);     // Send a code to the main process
  void trace(LogLineData *logData);     // Send a trace to the main process ( to be removed with the new log system )
  void reportProgress(double p);     // Report progress
  void reportProgress(double p, std::string status);
  void setUserError(std::string message);     // Set the error and finish the task
  void sendMessageProcessPlatform(samson::gpb::MessageProcessPlatform *message);     // Generic function to send messages from process to platform

  // Static flag to execute background processes as threads ( not forks )
  static bool isolated_process_as_tread;

private:
  // Internal function to be executed by a separate process or thread
  void runBackgroundProcessRun();

  // Exchange messages from the main process
  void runExchangeMessages();

  // Process a message from the background process ( Return true if it necessary to finish )
  bool processProcessPlatformMessage(samson::gpb::MessageProcessPlatform *message);

  // Pipes used between two process
  int pipeFdPair1[2];
  int pipeFdPair2[2];

  friend void *run_ProcessItemIsolated(void *p);
};
}

#endif  // ifndef _H_PROCESS_ITEM_ISOLATED
