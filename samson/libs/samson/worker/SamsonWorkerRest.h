#ifndef _H_SAMSON_WORKER_REST
#define _H_SAMSON_WORKER_REST

/*
 *
 * REST Interface for SamsonWorker
 *
 */

#include "au/network/RESTService.h"
#include "au/network/RESTServiceCommand.h"
#include "engine/Object.h"
#include "samson/delilah/Delilah.h"
#include "samson/worker/SamsonWorkerSamples.h"

#define notification_samson_worker_take_sample "notification_samson_worker_take_sample"

namespace samson {
class SamsonWorkerRest : public au::network::RESTServiceInterface, engine::Object {
public:

  SamsonWorkerRest(SamsonWorker *samson_worker, int web_port);
  virtual ~SamsonWorkerRest() {
  }

  // Receive notifications to take tample
  void notify(engine::Notification *notification);

  // au::network::RESTServiceInterface
  void process(au::SharedPointer< au::network::RESTServiceCommand> command);

  // Stop the rest waiting for all connections to finish ( even the thread for the listener )
  void Stop();

private:

  // Main pointer to samson worker
  SamsonWorker *samson_worker_;

  // Auxiliar functions to satisfy Rest queries
  void process_intern(au::SharedPointer< au::network::RESTServiceCommand> command);
  void process_delilah_command(std::string delilah_command, au::SharedPointer<au::network::RESTServiceCommand> command);
  void process_node(au::SharedPointer<au::network::RESTServiceCommand> command);
  void process_ilogging(au::SharedPointer<au::network::RESTServiceCommand> command);
  void process_logging(au::SharedPointer<au::network::RESTServiceCommand> command);

  // Auto-client for REST interface
  Delilah *delilah;

  // REST Service itself
  au::network::RESTService *rest_service;

  // Sampler for REST interface
  SamsonWorkerSamples samson_worker_samples;
};
}

#endif // ifndef _H_SAMSON_WORKER_REST
