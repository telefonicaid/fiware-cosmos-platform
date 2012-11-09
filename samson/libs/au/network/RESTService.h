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

#ifndef _H_AU_NETWORK_REST_SERVICE
#define _H_AU_NETWORK_REST_SERVICE

#include "au/Environment.h"

#include "au/Status.h"
#include "au/console/Console.h"
#include "au/containers/SharedPointer.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "au/network/NetworkListener.h"
#include "au/network/Service.h"
#include "au/network/SocketConnection.h"
#include "au/string/StringUtilities.h"
#include "au/tables/Table.h"
#include "au/utils.h"

namespace au {
namespace network {
class RESTServiceCommand;

class RESTServiceInterface {
public:
  virtual void process(
    au::SharedPointer<RESTServiceCommand> command) = 0;
};

class RESTService : public Service {
  // Interface to build response for the request
  RESTServiceInterface *interface;

public:

  RESTService(int port, RESTServiceInterface *_interface);
  virtual ~RESTService();

  // Virtual methods of Service
  virtual void run(SocketConnection *socket_connection,
                   bool *quit);
};
}
}  // end of namespace

#endif  // ifndef _H_AU_NETWORK_REST_SERVICE