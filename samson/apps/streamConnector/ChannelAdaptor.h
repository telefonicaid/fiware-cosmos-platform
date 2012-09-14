
#ifndef _H_STREAM_CONNECTOR_INTERCHANNEL_ITEM
#define _H_STREAM_CONNECTOR_INTERCHANNEL_ITEM

#include "au/network/SocketConnection.h"

#include "engine/ProcessItem.h"

#include "Adaptor.h"
#include "Connection.h"
#include "InterChannelLink.h"
#include "InterChannelPacket.h"
#include "PacketReaderWriter.h"
#include "common.h"
#include "common.h"
#include "message.pb.h"

namespace stream_connector {
class InputInterChannelConnection
  : public Connection
    , public au::network::PacketReaderInteface<InterChannelPacket>{
  // Real element to exchange data using "InterChannelPacket"
  InterChannelLink *link_;

  // Keep a poiter to the socket connection
  au::SocketConnection *socket_connection_;

  // Global pointer ( necessary to select channel )
  StreamConnector *stream_connector_;

  // Keep information about host
  std::string host_name_;

  // Selected channel
  std::string target_channel_;
  std::string source_channel_name_;       // Information about the source

public:

  InputInterChannelConnection(StreamConnector *stream_connector,
                              std::string host_name
                              , au::SocketConnection *socket_connection);

  static std::string getName(std::string host, std::string channel);
  // au::network::PacketReaderInteface<InterChannelPacket>
  virtual void process_packet(au::SharedPointer<InterChannelPacket> packet);

  // Close connection sending an error message to the other endpoint
  void close_connection(std::string error_message);

  // Connection virtual methods
  virtual std::string getStatus();
  virtual void review_connection();
  virtual void start_connection();
  virtual void stop_connection();

  // More information for this connection ( during first step )
  std::string host_and_port();
};


class OutputInterChannelConnection
  : public Connection
    , public au::network::PacketReaderInteface<InterChannelPacket>{
  // Information about connection
  std::string channel_name_;
  std::string host_;

  // Information about retrials
  au::Cronometer connection_cronometer;
  int connection_trials;

  // Link ( when established )
  InterChannelLink *link_;

  // Last error while trying to connect
  std::string last_error;

  // finish handshare
  bool hand_shake_finished;

  // List of pending packets from previous connection
  au::Queue<InterChannelPacket> pending_packets;

public:

  OutputInterChannelConnection(Adaptor *item
                               , std::string host
                               , std::string channel_name
                               );

  void init_hand_shake(std::string target_channel);

  // au::network::PacketReaderInteface<InterChannelPacket>
  virtual void process_packet(au::SharedPointer<InterChannelPacket> packet);


  // Connection virtual methods
  virtual void start_connection();
  virtual void stop_connection();
  virtual void review_connection();
  virtual std::string getStatus();
  virtual size_t bufferedSize();


private:

  // Type to establish this connection with remote server
  void try_connect();
};


class OutputChannelAdaptor : public Adaptor {
  // Information about connection
  std::string channel_name_;
  std::string host_;


public:

  OutputChannelAdaptor(Channel *channel
                       , const std::string& host
                       , const std::string& channel_name)
    : Adaptor(channel
              , connection_output
              , au::str("CHANNEL(%s:%s)", host.c_str(), channel_name.c_str())) {
    // Information for connection
    host_ = host;
    channel_name_ = channel_name;
  }

  virtual void start_item() {
    // Create a single connection for this item
    add(new OutputInterChannelConnection(this, host_, channel_name_));
  };

  // Get status of this element
  std::string getStatus() {
    return "Ok";
  }

  virtual void review_item() {
    // Nothing to do here
  }
};

class InputChannelAdaptor : public Adaptor {
  // Information about retrials
  au::Cronometer connection_cronometer;
  int connection_trials;

public:

  InputChannelAdaptor(Channel *channel)
    : Adaptor(channel
              , connection_input
              , au::str("CHANNELS(*)")) {
  }

  // Get status of this element
  std::string getStatus() {
    return au::str("%lu connections", getNumConnections());
  }

  void review_item() {
    // Nothing to do here
  }

  // Check if we accept a particular connection
  // We have not implemented any criteria to reject this connection
  virtual bool accept(InputInterChannelConnection *connection) {
    return true;
  }
};
}


#endif  // ifndef _H_STREAM_CONNECTOR_INTERCHANNEL_ITEM
