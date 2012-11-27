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
#ifndef _H_AU_NETWORK_REST_SERVICE_COMMAND
#define _H_AU_NETWORK_REST_SERVICE_COMMAND

#include <string>

#include "au/Environment.h"
#include "au/Status.h"
#include "au/console/Console.h"
#include "au/containers/set.h"
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
class RESTServiceCommand {
public:

  RESTServiceCommand();
  ~RESTServiceCommand();

  // Read& write over a socket connection
  au::Status Read(SocketConnection *socket_connection, au::ErrorManager& error);
  au::Status Write(SocketConnection *socket_connection);

  // Command to append something at the output
  void Append(const std::string& txt);
  void AppendFormatedLiteral(const std::string& name, const std::string& value);
  void AppendFormatedElement(const std::string& name, const std::string& value);
  void AppendFormatedError(const std::string& message);
  void AppendFormatedError(int _http_state, const std::string& message);

  // Set a redicrect
  void SetRedirect(const std::string& redirect_resource);

  // Accessorts
  void set_http_state(int s) {
    http_state_ = s;
  }

  void set_format(const std::string& format) {
    format_ = format;  // Force a particular output format
  }

  int http_state() const {
    return http_state_;
  }

  std::string format() const {
    return format_;
  }

  const StringVector& path_components() const {
    return path_components_;
  }

  std::string command() const {
    return command_;
  }

  std::string resource() const {
    return resource_;
  }

  std::string path() const {
    return path_;
  }

  const char *data() const {
    return data_;
  }

  size_t data_size() const {
    return data_size_;
  }

  // Method to lock execution until it is done
  void WaitUntilFinished();
  void NotifyFinish();

private:

  // Mechanism to block main thread
  au::Token token_;
  bool finished_;

  // Command fields
  std::string command_;                 // GET, PUT, ...
  std::string resource_;                // url...
  std::string path_;                    // path...
  StringVector path_components_;        // Paths in the url
  std::string format_;                  // Extension of the resource (.xml , .json , .html )

  // First line in request
  char request_line_[1024];

  // Header environment
  au::Environment header_;

  // Body
  char *data_;
  size_t data_size_;

  // Output state
  int http_state_;

  // Output data
  std::ostringstream output_;

  // Redirect location ( if any, output is ignored )
  std::string redirect_;
};
}
}

#endif  // ifndef _H_AU_NETWORK_REST_SERVICE_COMMAND
