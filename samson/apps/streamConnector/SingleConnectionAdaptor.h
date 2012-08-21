
#ifndef _H_STREAM_CONNECTOR_SINGLE_CONNECTION_ADAPTOR
#define _H_STREAM_CONNECTOR_SINGLE_CONNECTION_ADAPTOR


#include "Adaptor.h"
#include "Connection.h"

namespace stream_connector {
class Connection;
class Channel;

class SingleConnectionAdaptor : public Adaptor, public Connection {
public:

  SingleConnectionAdaptor(Channel *channel,  ConnectionType type, const std::string& name)
    : Adaptor(channel, type, name)
      , Connection(this, type, name) {
  }

  virtual void start_item() {
    // Add myself as a connection ( unique and permanent connection )
    add(this);
  };

  // Get status of this element
  virtual std::string getStatus() {
    return "OK";         // To be overwrited by real connection
  }

  virtual void review_item() {
    // Nothing to do here
  }
};
}


#endif // ifndef _H_STREAM_CONNECTOR_SINGLE_CONNECTION_ADAPTOR