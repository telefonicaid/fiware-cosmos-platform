


#ifndef _H_SAMSON_WORKER_NETWORK
#define _H_SAMSON_WORKER_NETWORK

#include "samson/common/ports.h"
#include "samson/common/status.h"


#include "au/network/NetworkListener.h"
#include "samson/network/CommonNetwork.h"
#include "samson/network/NetworkInterface.h"

namespace samson {
class WorkerNetwork : public CommonNetwork, public au::NetworkListenerInterface {
  // Worker listtener for new worker / delilah connections
  au::NetworkListener *worker_listener;

public:

  // Constructor
  // ------------------------------------------------------------
  WorkerNetwork(size_t worker_id
                , int port
                , NetworkInterfaceReceiver *receiver
                , gpb::ClusterInfo *cluster_information
                , size_t cluster_information_version);

  ~WorkerNetwork();

  // Stop background threads
  void stop();

  // NetworkManager interface
  void newSocketConnection(au::NetworkListener *listener, au::SocketConnection *socket_connetion);
};
}

#endif // ifndef _H_SAMSON_WORKER_NETWORK
