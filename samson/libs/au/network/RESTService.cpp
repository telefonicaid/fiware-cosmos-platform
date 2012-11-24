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

#include "RESTService.h"  // Own interface
#include "RESTServiceCommand.h"
#include "au/au.pb.h"
#include "au/gpb.h"
#include "au/string/xml.h"


namespace au {
namespace network {
RESTService::RESTService(int port, RESTServiceInterface *interface) : Service(port) {
  interface_ = interface;    // Keep a pointer to the interface to get REST answers
}

RESTService::~RESTService() {
}

void RESTService::run(SocketConnection *socket_connection, bool *quit) {
  if (*quit) {
    return;
  }

  au::ErrorManager error;

  // Read HTTP packet
  au::SharedPointer<RESTServiceCommand> command(new RESTServiceCommand());
  au::Status s = command->Read(socket_connection, error);

  if (s != au::OK) {
    LOG_SW(("Error in REST interface ( %s / %s )", status(s), error.GetMessage().c_str()));
    socket_connection->Close();
    return;
  }

  // Get anser from the service
  interface_->process(command);

  // Return anser for this request
  s = command->Write(socket_connection);

  if (s != au::OK) {
    LOG_SW(("Error in REST interface ( %s / %s )", status(s), error.GetMessage().c_str()));
    socket_connection->Close();
    return;
  }

  // Close socket connection in all cases
  socket_connection->Close();
}
}
}
