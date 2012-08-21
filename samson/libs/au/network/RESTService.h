
#ifndef _H_AU_NETWORK_REST_SERVICE
#define _H_AU_NETWORK_REST_SERVICE

#include "au/Status.h"
#include "au/containers/SharedPointer.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "au/Environment.h"
#include "au/Object.h"
#include "au/au.pb.h"
#include "au/console/Console.h"
#include "au/gpb.h"
#include "au/string.h"
#include "au/tables/Table.h"
#include "au/utils.h"
#include "au/network/NetworkListener.h"
#include "au/network/Service.h"
#include "au/network/SocketConnection.h"

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