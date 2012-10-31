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

#ifndef _H_SAMSON_WORKER_NETWORK
#define _H_SAMSON_WORKER_NETWORK

#include <string>

#include "au/network/NetworkListener.h"

#include "samson/common/ports.h"
#include "samson/common/status.h"
#include "samson/network/CommonNetwork.h"
#include "samson/network/NetworkInterface.h"

namespace samson {
class WorkerNetwork : public CommonNetwork, public au::NetworkListenerInterface {
  public:
    WorkerNetwork(size_t worker_id, int port);
    ~WorkerNetwork();

    // Stop background threads
    void stop();

    void SendAlertToAllDelilahs(std::string type, std::string context, std::string message);
    void SendAlertToDelilah(size_t delilah_id, std::string type, std::string context, std::string message);

    // NetworkManager interface
    void newSocketConnection(au::NetworkListener *listener, au::SocketConnection *socket_connetion);

  private:
    // Worker listtener for new worker / delilah connections
    au::NetworkListener *worker_listener;
};
}

#endif  // ifndef _H_SAMSON_WORKER_NETWORK
