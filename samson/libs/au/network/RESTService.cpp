
#include "RESTService.h"  // Own interface
#include "RESTServiceCommand.h"
#include "au/xml.h"


namespace au {
namespace network {
RESTService::RESTService(int port,
                         RESTServiceInterface *_interface) :
  Service(port) {
  // Keep a pointer to the interface to get REST answers
  interface = _interface;
}

RESTService::~RESTService() {
}

void RESTService::run(SocketConnection *socket_connection,
                      bool *quit) {
  if (*quit)
    return;

  au::ErrorManager error;

  // Read HTTP packet
  au::SharedPointer<RESTServiceCommand> command(
    new RESTServiceCommand());
  au::Status s = command->Read(socket_connection, error);

  if (s != au::OK) {
    LM_W(("Error in REST interface ( %s / %s )", status(s),
          error.GetMessage().c_str()));
    socket_connection->Close();
    return;
  }

  // Get anser from the service
  interface->process(command);

  // Return anser for this request
  s = command->Write(socket_connection);
  if (s != au::OK) {
    LM_W(("Error in REST interface ( %s / %s )", status(s),
          error.GetMessage().c_str()));
    socket_connection->Close();
    return;
  }

  // Close socket connection in all cases
  socket_connection->Close();
}
}
}
