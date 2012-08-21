

#ifndef _H_AU_LOG_TO_SERVER
#define _H_AU_LOG_TO_SERVER

#include <arpa/inet.h>          // inet_ntoa
#include <netdb.h>              // gethostbyname
#include <netinet/in.h>         // struct sockaddr_in
#include <netinet/tcp.h>        // TCP_NODELAY
#include <signal.h>
#include <sys/socket.h>         // socket, bind, listen
#include <sys/un.h>             // sockaddr_un
#include <unistd.h>

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"
#include "parseArgs/paBuiltin.h"
#include "parseArgs/paConfig.h"
#include "parseArgs/paIsSet.h"
#include "parseArgs/parseArgs.h"

#include "au/containers/list.h"
#include "au/containers/set.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "au/network/SocketConnection.h"
#include "au/string.h"

#include "au/log/Log.h"
#include "au/log/LogPlugin.h"
#include "au/log/log_server_common.h"

namespace au {
// start and stop connection with log server
void start_log_to_server(std::string log_host, int log_port, std::string local_log_file);
void stop_log_to_server();

// After fork, this should be called to avoid deadlocks in log system
void restart_log_to_server(std::string local_log_file);

// Set direct mode ( no blocking to reconnection )
void set_log_direct_mode(bool flag);

// Get fd ( to avoid closing when forking )
int get_log_fd();

// Add and remove plugins
void add_log_plugin(LogPlugin *plugin);
void remove_log_plugin(LogPlugin *plugin);

// Change the server name to send logs
void set_log_server(std::string log_host, int log_port = AU_LOG_SERVER_PORT);

// Function to handle logs from lm library
void logToLogServer(void *vP,
                    char *text,
                    char type,
                    time_t secondsNow,
                    int timezone,
                    int dst,
                    const char *file,
                    int lineNo,
                    const char *fName,
                    int tLev,
                    const char *stre);
}

#endif  // ifndef _H_AU_LOG_TO_SERVER
