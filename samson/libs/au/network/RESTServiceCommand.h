#ifndef _H_AU_NETWORK_REST_SERVICE_COMMAND
#define _H_AU_NETWORK_REST_SERVICE_COMMAND

#include "au/Status.h"
#include "au/containers/set.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "au/tables/Table.h"
#include "au/Environment.h"
#include "au/au.pb.h"
#include "au/gpb.h"
#include "au/Object.h"
#include "au/string.h"
#include "au/utils.h"
#include "au/console/Console.h"
#include "au/network/NetworkListener.h"
#include "au/network/Service.h"
#include "au/network/SocketConnection.h"

namespace au {
namespace network {
class RESTServiceCommand {
public:

  RESTServiceCommand();
  ~RESTServiceCommand();

  // Read& write over a socket connection
  au::Status Read(SocketConnection *socket_connection,
                  au::ErrorManager& error);
  au::Status Write(SocketConnection *socket_connection);

  // Command to append something at the output
  void Append(const std::string& txt);
  void AppendFormatedElement(const std::string& name,
                             const std::string& value);
  void AppendFormatedError(const std::string& message);
  void AppendFormatedError(int _http_state,
                           const std::string& message);

  // Set a redicrect
  void SetRedirect(const std::string& redirect_resource);

  // Accessorts
  void set_http_state(int s);
  int http_state();
  std::string format();
  void set_format(const std::string format);
  const StringVector& path_components();
  std::string command();

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