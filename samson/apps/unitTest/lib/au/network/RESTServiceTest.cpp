#include <fcntl.h>

#include "au/network/RESTService.h"
#include "au/network/RESTServiceCommand.h"
#include "gtest/gtest.h"
#include "unitTest/test_common.h"

class RESTServiceInterfaceImpl : public au::network::RESTServiceInterface {
public:
  virtual void process(
    au::SharedPointer<au::network::RESTServiceCommand> command) {
    num_commands_++;
    last_command_ = command;
  }

  static int num_commands() {
    return num_commands_;
  }

  static void clear_num_commands() {
    num_commands_ = 0;
  }

private:
  static au::SharedPointer<au::network::RESTServiceCommand> last_command_;
  static int num_commands_;
};

au::SharedPointer<au::network::RESTServiceCommand> RESTServiceInterfaceImpl::
last_command_;
int RESTServiceInterfaceImpl::num_commands_ = 0;

TEST(au_network_RESTService, basic) {
  RESTServiceInterfaceImpl impl;

  RESTServiceInterfaceImpl::clear_num_commands();  // Just in case we run multiple tests
  au::network::RESTService rest_service(14567, &impl);
  EXPECT_EQ(au::OK, rest_service.InitService());

  EXPECT_EQ(0, RESTServiceInterfaceImpl::num_commands());

  // Socket connection to send a message
  au::SocketConnection *socket_connection;
  EXPECT_EQ(au::OK,
            au::SocketConnection::Create("localhost", 14567, &socket_connection));

  const char *message =                                                \
    "GET /articles/hoho.htm HTTP/1.0\n"                                \
    "Accept-Language: ko,en-us;q=0.5\n"                                \
    "User-Agent: Mozilla/4.0 (compatible; MSIE 5.5; Windows NT 5.0)\n" \
    "\n";

  EXPECT_EQ(au::OK,
            socket_connection->partWrite(message, strlen(message),
                                         "Send message"));

  // Wait until connection is closed
  char line[1024];
  socket_connection->ReadLine(line, sizeof(line), 1000);

  socket_connection->Close();
  delete socket_connection;

  // Close service
  rest_service.StopService();

  // Check on the last message
  EXPECT_EQ(1, RESTServiceInterfaceImpl::num_commands());
}
