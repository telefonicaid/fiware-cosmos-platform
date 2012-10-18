#ifndef _H_STREAM_CONNECTOR_CONNECTION_FileDescriptorConnection
#define _H_STREAM_CONNECTOR_CONNECTION_FileDescriptorConnection

#include "au/statistics/Cronometer.h"
#include "au/mutex/TokenTaker.h"

#include "au/network/NetworkListener.h"
#include "au/network/SocketConnection.h"
#include "samson/client/SamsonClient.h"
#include "samson/client/SamsonPushBuffer.h"


#include "Connection.h"
#include "common.h"

extern size_t buffer_size;

namespace stream_connector {
class Adaptor;

/*
 * Simple FileDescriptor connection ( used in ListenerAdaptor or ConnectionItem )
 */

class FileDescriptorConnection : public Connection {
  au::FileDescriptor *file_descriptor_;           // Current file descritor to read or write
  bool thread_running_;                           // Flag to indicate if thread is still running in background


  int num_connections_;                           // Number of connections
  au::Cronometer cronometer_reconnection_;        // Cronometer since last reconnection

  size_t input_buffer_size;                       // Variable length input buffer

public:

  FileDescriptorConnection(Adaptor *_item, ConnectionType _type, std::string _name);
  ~FileDescriptorConnection();

  // Get the File descriptor
  virtual au::FileDescriptor *getFileDescriptor() = 0;

  // Main function of the dedicated thread
  void run();
  void run_as_input();
  void run_as_output();

  // Connection virtual methods
  virtual void start_connection();
  virtual void review_connection();
  virtual void stop_connection();
  virtual std::string getStatus();

private:

  void connect();
};


class SimpleFileDescriptorConnection : public FileDescriptorConnection {
  au::FileDescriptor *file_descriptor_;

public:

  SimpleFileDescriptorConnection(Adaptor *_item
                                 , ConnectionType _type
                                 , std::string _name
                                 , au::FileDescriptor *file_descriptor);

  virtual au::FileDescriptor *getFileDescriptor();
};
}


#endif  // ifndef _H_STREAM_CONNECTOR_CONNECTION_FileDescriptorConnection
