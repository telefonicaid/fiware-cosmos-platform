#ifndef _H_SAMSON_CONNECTOR
#define _H_SAMSON_CONNECTOR

#include <set>

#include "logMsg/logMsg.h"

#include "au/console/Console.h"
#include "au/containers/map.h"
#include "au/containers/vector.h"
#include "au/mutex/Token.h"
#include "au/string.h"

#include "engine/Buffer.h"

#include "au/network/ConsoleService.h"
#include "au/network/NetworkListener.h"
#include "au/network/NetworkListener.h"
#include "au/network/RESTService.h"

#include "DiskAdaptor.h"
#include "LogManager.h"
#include "SamsonAdaptor.h"
#include "StreamConnector.h"
#include "common.h"

#include "BufferProcessor.h"

#include "Adaptor.h"
#include "Channel.h"
#include "ConnectorCommand.h"
#include "ConnectorCommand.h"

/*
 *
 * ------------------------------------------------------------
 *
 * Input 1 -> BufferProcessor                   Output1
 * Input 2 -> BufferProcessor --> Channel 1  --> Output2
 * Input 3 -> BufferProcessor                   Output3
 *
 * Input 1 -> BufferProcessor                   Output1
 * Input 2 -> BufferProcessor --> Channel 2 --> Output2
 * Input 3 -> BufferProcessor                   Output3
 *
 * ------------------------------------------------------------
 *
 * Inputs
 *
 * port:P            Open port P and accept connections
 * connection:H:P    Establish a connection to host H port P
 * samson:H:Q        Establish a connection to samson at host H and receive from queue Q
 * disk:D            Read content from directory D
 * channel           All interchannel connections
 * stdin             Standard input ( only in non iterative / deamon )
 * hdfs:H:D          Establish a connection with a HDFS service at host H and write content to direcotry D
 *
 * Outputs
 *
 * port:P            Open port P and accept connections
 * connection:H:P    Establish a connection to host H port P
 * samson:H:Q        Establish a connection to samson at host H and receive from queue Q
 * disk:D            Read content from directory D
 * channel:H:C       Connect to channel "C" at host "H"
 * stdout            Standard output ( only in non iterative / deamon )
 * hdfs:H:D          Establish a connection with a HDFS service at host H and write content to direcotry D
 *
 */

extern bool interactive;
extern bool run_as_daemon;
extern int sc_console_port;
extern int sc_web_port;


namespace stream_connector {
class Channel;
class StreamConnectorService;
class InputInterChannelConnection;

class StreamConnector : public au::Console
                        , public au::network::RESTServiceInterface
                        , public au::NetworkListenerInterface {
  // Mutex protection for streamConnector( REST / Console / Review / ... )
  au::Token token;


  // List of channels in this streamConnector
  au::map<std::string, Channel> channels_;

  // General environment
  au::Environment environment;

  // Services over streamConnector
  StreamConnectorService *service;                     // Service to accept monitor connection
  au::network::RESTService *rest_service;              // REST Interface
  au::NetworkListener *inter_channel_listener;         // Simple listener for interchannel connection

  // List of connection for interchannel
  au::list<InputInterChannelConnection> input_inter_channel_connections;

  // Information about input & output
  TrafficStatistics traffic_statistics;

  // Global cronometer
  au::Cronometer cronometer;

public:

  StreamConnector();

  // Init external connection services
  void init_reset_service();                            // Init rest interface
  void init_inter_channel_connections_service();        // Init listener to receive inter channel connections
  void init_remove_connections_service();               // Add service to accept monitor connection

  // General process command line ( from rest / console / remote connections / .... )
  void process_command(std::string command, au::ErrorManager *error);

  // au::Console interface
  std::string getPrompt();
  void evalCommand(std::string command);
  void autoComplete(au::ConsoleAutoComplete *info);
  void autoCompleteWithChannelNames(au::ConsoleAutoComplete *info);
  void autoCompleteWithAdaptorsNames(au::ConsoleAutoComplete *info);

  // Review
  void review();

  // Extract global information
  size_t getOutputConnectionsBufferedSize();      // Get pending data to be sent
  int getNumInputItems();                         // Get the number of input items
  size_t getNumUnfinishedInputConnections();      // Get number of unfinished input connections

  // User-password manafement
  std::string getPasswordForUser(std::string user);
  void setPasswordForUser(std::string user, std::string password);

  // au::network::RESTServiceInterface
  void process(au::SharedPointer<au::network::RESTServiceCommand> command);

  // au::network::NetworkListenerInterface
  virtual void newSocketConnection(au::NetworkListener *listener
                                   , au::SocketConnection *socket_connetion);

  // Write errors depending on setup ( interactive, deamon, normal )
  void log(au::SharedPointer<Log> log);
  void log(const std::string& name, const std::string& type, const std::string& message);


  // Select channel for an interchannel connection
  void select_channel(InputInterChannelConnection *connection, std::string target_channel, au::ErrorManager *error);

  // Close finished items and connections
  void remove_finished_items_and_connections(au::ErrorManager *error);

  void report_output_size(size_t size);
  void report_input_size(size_t size);

  // Get tables with information about current status
  au::tables::Table *getSummaryTable();
  au::tables::Table *getChannelsTable(std::string type = "default");
  au::tables::Table *getItemsTable(std::string type = "default");
  au::tables::Table *getConnectionsTable(std::string type = "default", std::string select_channel = "");
  au::tables::Table *getInputInterChannelConnections();
};
}
#endif  // ifndef _H_SAMSON_CONNECTOR
