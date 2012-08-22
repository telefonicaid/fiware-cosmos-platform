#include <fcntl.h>

#include "au/ThreadManager.h"
#include "au/network/FileDescriptor.h"
#include "au/network/NetworkListener.h"
#include "au/network/SocketConnection.h"
#include "gtest/gtest.h"
#include "unitTest/test_common.h"

class NetworkListenerInterfaceImpl : public au::NetworkListenerInterface {
public:

  virtual void newSocketConnection(au::NetworkListener *listener,
                                   au::SocketConnection *socket_connetion) {
    if (socket_connetion == NULL)
      LM_X(1, ("Null socket connection")); char line[1024];
    num_connections_++;

    if (socket_connetion->ReadLine(line, sizeof(line)) == au::OK)
      socket_connetion->WriteLine(line, strlen(line)); socket_connetion->Close();
  }

  static int num_connections() {
    return num_connections_;
  }

  static void clear_connections() {
    num_connections_ = 0;
  }

private:
  static int num_connections_;
};

int NetworkListenerInterfaceImpl::num_connections_ = 0;



TEST(au_network_NetworkListener, basic) {
  // No background threads check
  EXPECT_EQ(0, au::ThreadManager::shared()->getNumThreads());

  NetworkListenerInterfaceImpl impl;
  NetworkListenerInterfaceImpl::clear_connections();  // just in case we execute repeated tests
  au::NetworkListener network_listener(&impl);

  EXPECT_EQ(au::OK,
            network_listener.InitNetworkListener(14567)) <<
  "Error starting au::NetworkListener";
  EXPECT_EQ(0, NetworkListenerInterfaceImpl::num_connections());
  EXPECT_EQ(14567, network_listener.port());
  EXPECT_TRUE(network_listener.IsNetworkListenerRunning());

  // Create a socket connection to test reception
  au::SocketConnection *socket_connection;
  EXPECT_EQ(au::OK,
            au::SocketConnection::Create("localhost", 14567, &socket_connection));


  EXPECT_EQ(au::OK, socket_connection->WriteLine("Hola\n"));
  char line[1024];
  EXPECT_EQ(au::OK, socket_connection->ReadLine(line, sizeof(line), 5));
  EXPECT_EQ(0, strcmp(line, "Hola\n"));

  socket_connection->Close();
  delete socket_connection;

  // Close network listener
  network_listener.StopNetworkListener();
  // Compiler complains about EXPECT_EQ(false,), but not for EXPECT_EQ(true,)
  EXPECT_FALSE(network_listener.IsNetworkListenerRunning());

  // Expect one connection after all
  EXPECT_EQ(1, NetworkListenerInterfaceImpl::num_connections());

  // No background threads check
  EXPECT_EQ(0, au::ThreadManager::shared()->getNumThreads());
}
