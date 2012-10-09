#ifndef _H_AU_NETWORK_CONSOLE_SERVICE
#define _H_AU_NETWORK_CONSOLE_SERVICE

#include <string>

#include "au/Status.h"
#include "au/containers/set.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"

#include "au/tables/Table.h"

#include "au/Environment.h"
#include "au/au.pb.h"
#include "au/console/Console.h"
#include "au/gpb.h"
#include "au/network/Service.h"

namespace au {
namespace network {
class Service;

class ConsoleServiceClientBase {
  public:

    ConsoleServiceClientBase(int port);

    void Connect(std::string host, au::ErrorManager *error);
    void Disconnect(au::ErrorManager *error);

    bool Write(au::gpb::ConsolePacket *packet, au::ErrorManager *error);
    bool Read(au::gpb::ConsolePacket **packet, au::ErrorManager *error);

    // Methods related with au::Console
    std::string getPrompt();
    void evalCommand(std::string command, au::ErrorManager *error);
    virtual void autoComplete(ConsoleAutoComplete *info);
    void addEspaceSequence(std::string sequence);
    virtual void process_escape_sequence(std::string sequence) {
    }
    ;

  private:

    // Full a message to be sent
    void FillMessage(au::gpb::ConsolePacket *message, au::ErrorManager *error);

    int port_;
    SocketConnection *socket_connection_;

    // Prompt request delayed
    au::Cronometer cronometer_prompt_request_;
    std::string current_prompt_;
};

// Simple console to interact with the client

class ConsoleServiceClient : public ConsoleServiceClientBase, public Console {
  public:

    ConsoleServiceClient(int port);

    // Virtual methods of console
    virtual void evalCommand(std::string command);
    virtual void autoComplete(ConsoleAutoComplete *info);
    virtual std::string getPrompt();
};

// Service based on a remote console

class ConsoleService : public Service {
  public:

    ConsoleService(int port) :
      Service(port) {
    }

    // Virtual methods to be implemented by subclasses
    virtual void runCommand(std::string command, au::Environment *environment, au::ErrorManager *error) {
    }
    virtual void autoComplete(ConsoleAutoComplete *info, au::Environment *environment) {
    }
    virtual std::string getPrompt(au::Environment *environment) {
      return ">>";
    }

    // main function executed by all threads
    virtual void run(SocketConnection *socket_connection, bool *quit);

  private:

    // Fill message to be sent to client
    void fill_message(au::ErrorManager *error, au::gpb::ConsolePacket *message);

};
}
}

#endif  // ifndef _H_AU_NETWORK_CONSOLE_SERVICE
