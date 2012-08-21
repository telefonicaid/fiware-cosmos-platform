
#ifndef _H_ZOOKEEPER_CONNECTION_WATCHER_INTERFACE
#define _H_ZOOKEEPER_CONNECTION_WATCHER_INTERFACE

#include <string>

namespace samson { namespace zoo {
                   class Connection;

                   // ------------------------------------------------------------------
                   //
                   // class ConnectionWatcherInterface
                   //
                   // Interface to receive watcher alerts
                   //
                   // ------------------------------------------------------------------

                   class ConnectionWatcherInterface {
public:

                     virtual void watcher(Connection *connection, int type, int state, const char *path) = 0;
                   };
                   } } // End of namespace samson::zoo

#endif // ifndef _H_ZOOKEEPER_CONNECTION_WATCHER_INTERFACE