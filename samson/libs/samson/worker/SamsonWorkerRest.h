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
#ifndef _H_SAMSON_WORKER_REST
#define _H_SAMSON_WORKER_REST

/*
 *
 * REST Interface for SamsonWorker
 *
 */

#include <string>

#include "au/network/RESTService.h"
#include "au/network/RESTServiceCommand.h"
#include "engine/NotificationListener.h"
#include "samson/delilah/Delilah.h"
#include "samson/worker/SamsonWorkerSamples.h"

#define notification_samson_worker_take_sample "notification_samson_worker_take_sample"

namespace samson {
class SamsonWorkerRest : public au::network::RESTServiceInterface, engine::NotificationListener {
public:
  SamsonWorkerRest(SamsonWorker *samson_worker, int web_port);
  virtual ~SamsonWorkerRest();

  // Receive notifications to take tample
  void notify(engine::Notification *notification);

  // au::network::RESTServiceInterface
  void process(au::SharedPointer<au::network::RESTServiceCommand> command);

  void StopRestService();

private:


  // Auxiliar functions to satisfy Rest queries
  void ProcessIntern(au::SharedPointer<au::network::RESTServiceCommand> command);
  void ProcessDelilahCommand(std::string delilah_command, au::SharedPointer<au::network::RESTServiceCommand> cmd);
  void process_ilogging(au::SharedPointer<au::network::RESTServiceCommand> command);
  void ProcessLookupSynchronized(au::SharedPointer<au::network::RESTServiceCommand> command);

  // Handy method to add a collection to the output
  // The same collections are generated to be sent to delilahs
  void Append(au::SharedPointer<au::network::RESTServiceCommand> command,
              au::SharedPointer<gpb::Collection> collection);

  SamsonWorker *samson_worker_;  // Main pointer to samson worker
  Delilah *delilah_;    // Auto-client for REST interface
  au::network::RESTService *rest_service_;    // REST Service itself
  SamsonWorkerSamples samson_worker_samples_;    // Sampler for REST interface
};
}

#endif  // ifndef _H_SAMSON_WORKER_REST
