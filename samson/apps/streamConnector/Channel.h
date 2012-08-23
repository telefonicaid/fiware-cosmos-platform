#ifndef _H_SAMSON_CHANNEL
#define _H_SAMSON_CHANNEL

#include "ConnectorCommand.h"
#include "LogManager.h"
#include "TrafficStatistics.h"
#include "common.h"
#include "engine/Buffer.h"

namespace stream_connector {
class Adaptor;
class StreamConnector;

class Channel {
  // Mutex protection
  au::Token token;

  // Name of this channel
  std::string name_;

  // Name of the splitters ( process on all inputs for this channel )
  std::string splitter_;

  // All items ( adapters ) included in this channel
  au::map<std::string, Adaptor> items;


  // Pointer to the global connector
  StreamConnector *connector_;

  friend class Connection;
  friend class StreamConnector;

public:

  // Input & Output statistics
  TrafficStatistics traffic_statistics;

public:

  // Constructor
  Channel(StreamConnector *connector, std::string name, std::string splitter);
  ~Channel();

  // item management
  void add_input(std::string name, std::string input_string, au::ErrorManager *error);
  void add_output(std::string name, std::string input_string, au::ErrorManager *error);
  void add(std::string name, Adaptor *item);
  void remove_item(std::string name, au::ErrorManager *error);


  // General review to check if there are connections to be closed
  void review();

  // Common method to push data to all output connections ( from all items at input )
  void push(engine::BufferPointer buffer);

  // Channel management
  void cancel_channel();
  void remove_finished_items_and_connections(au::ErrorManager *error);


  // Information
  int getNumItems();
  int getNumInputItems();
  int getNumOutputItems();
  int getNumConnections();

  std::string getName();
  std::string getSplitter();
  std::string getInputsString();
  std::string getOutputsString();

  size_t getOutputConnectionsBufferedSize();


  // Log system
  void log(std::string type, std::string message);
  void log(au::SharedPointer<Log> log);

  // Data report system
  void report_output_size(size_t size);
  void report_input_size(size_t size);

  // Auto complete
  void autoCompleteWithAdaptorsNames(au::ConsoleAutoComplete *info);
};
}

#endif  // ifndef _H_SAMSON_CHANNEL