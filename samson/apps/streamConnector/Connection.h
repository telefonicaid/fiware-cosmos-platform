#ifndef _H_SAMSON_CONNECTOR_CONNECTION_BASE
#define _H_SAMSON_CONNECTOR_CONNECTION_BASE

#include "au/ThreadManager.h"
#include "engine/Buffer.h"

#include "BufferList.h"
#include "LogManager.h"
#include "TrafficStatistics.h"
#include "common.h"
#include "samson/client/SamsonClient.h"

namespace stream_connector {
class Channel;
class Adaptor;
class BufferProcessor;

//
// ConnectionBase
//
// Manage information about conection ( time , disconnection time etc... )
//

class ConnectionBase {
  au::Cronometer creation_cronometer;
  au::Cronometer connection_cronometer;       // Time since connected or disconnected
  bool connected;       // Flag to indicate if we are connected or not ( just informatio )

  double time_connected;
  double time_disconnected;

public:

  ConnectionBase() {
    connected = false;  // By default it is not connected
    time_connected = 0;
    time_disconnected = 0;
  }

  void set_as_connected(bool c) {
    if (c == connected) {
      return;           // Nothing changed
    }
    // Accumulate time
    if (connected) {
      time_connected += connection_cronometer.seconds();
    } else {
      time_disconnected += connection_cronometer.seconds();
    }

    connection_cronometer.Reset();


    // Set flag connected
    connected = c;
  }

  double getConnectionRate() {
    double c = time_connected;
    double nc = time_connected;

    if (connected) {
      c += connection_cronometer.seconds();
    } else {
      nc += connection_cronometer.seconds();
    }


    double t = c + nc;

    if (t == 0) {
      return 0;
    }



    return c / (c + nc);
  }

  std::string getConnectionStatus() {
    double rate = getConnectionRate();

    std::ostringstream output;

    if (!connected) {
      output << "Disconnected ";

      if (time_connected == 0) {
        output << "( never connected )";
      } else {
        output << connection_cronometer;
        output << " ago ( ";
        output << au::str_time_simple(time_connected);
        output << " time connectd )";
      }
    } else {
      output << "Connected " << connection_cronometer;
      if (rate < 0.8) {
        output << " ( " << au::str_percentage(rate) <<  ")";
      }
    }

    return output.str();
  }
};

class Connection : public ConnectionBase {
  friend class StreamConnector;
  friend class Adaptor;

  au::Token token;                      // Mutex protection for this connection

  std::string description_;             // Description name
  ConnectionType type;                  // Type of item ( input or output )
  Adaptor *item;                        // My adaptor

  BufferProcessor *buffer_processor;    // Processor for input buffers ( only in input )

  BufferList *input_buffer_list;        // List of buffers at the input
  BufferList *output_buffer_list;       // List of buffers at the output to be sent ( in output connections )

  bool initialized;                     // Falg to indicate init_connection has been called
  bool canceled;                        // Flag to indicate this is canceled ( not call review again )
  bool finished;                        // Falg to indicate this is finished ( no more data )


  size_t id;                            // Assigned when added to a item

  // Information about this connection
  au::Cronometer cronometer;            // Global cronometer

public:

  TrafficStatistics traffic_statistics;  // Information about input & output

protected:

  // Method to recover buffers to be pushed to the output ( output connections )
  engine::BufferPointer getNextBufferToSent();

  // Method to push any input buffer ( input connections )
  void pushInputBuffer(engine::BufferPointer buffer);
  void flushInputBuffers();

public:

  Connection(Adaptor *_item, ConnectionType _type, std::string _name);
  virtual ~Connection();

  ConnectionType getType();                     // Get type
  const char *getTypeStr();
  std::string getDescription();                 // Get a name of this element
  std::string getFullName();                    // Get a name of this element
  size_t getId();


  void setDescription(const std::string& description) {
    description_ = description;
  }

  virtual size_t bufferedSize();                // Get currect size accumulated here
  virtual size_t bufferedSizeOnMemory();        // Get current size accumulated in memory

  // Method to push data from channel ( only output )
  virtual void push(engine::BufferPointer buffer);

  virtual std::string getStatus() = 0;          // Get status of this element
  virtual void review_connection() = 0;         // Method called to review this connection
  virtual void start_connection() = 0;          // Start connection effectivelly ( create threads and stuff )
  virtual void stop_connection() = 0;           // Stop everything in this connection just to be removed

  void init_connecton();

  void cancel_connecton();

  void review();

  std::string str();

  void cancel_connection();

  void report_output_size(size_t);
  void report_input_size(size_t);

  void set_as_finished();
  bool is_finished();

  // Log system
  void log(std::string type, std::string message);
  void log(Log *log);
};
}


#endif  // ifndef _H_SAMSON_CONNECTOR_CONNECTION_BASE