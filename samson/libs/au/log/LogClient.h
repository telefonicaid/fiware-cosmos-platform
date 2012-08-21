

#ifndef _H_LOG_CLIENT
#define _H_LOG_CLIENT

#include <fcntl.h>
#include <string>
#include <sys/stat.h>  // mkdir

#include "logMsg/logMsg.h"

#include "au/console/Console.h"
#include "au/containers/list.h"
#include "au/containers/map.h"
#include "au/network/FileDescriptor.h"
#include "au/network/Service.h"
#include "au/string/split.h"
#include "au/tables/Table.h"
#include "au/utils.h"

#include "au/log/Log.h"
#include "au/log/log_server_common.h"

namespace au {
/*
 *  class LogClient : public Console
 *  {
 *      // Main socket connection with the server
 *      au::SocketConnection * socket_connection;
 *
 *      // Server & port
 *      std::string host;
 *      int port;
 *
 *  public:
 *
 *      // Constructor
 *      LogClient();
 *
 *      // Destructor
 *      ~LogClient();
 *
 *      // Connect and disconnect commands
 *      void connect( std::string _host , int _port , au::ErrorManager * error );
 *      void closeConnection( ErrorManager * error );
 *
 *      // au::Console stuff
 *      std::string getPrompt();
 *      void evalCommand( std::string command );
 *      void evalCommand( std::string command , Console * console );
 *      void autoComplete( au::ConsoleAutoComplete* info );
 *
 *      // Methods to send commands and receive response
 *      void sent_command( std::string command , au::ErrorManager * error );
 *      std::string getMessageFromLogServer( au::ErrorManager * error );
 *
 *  };
 */
}
#endif // ifndef _H_LOG_CLIENT