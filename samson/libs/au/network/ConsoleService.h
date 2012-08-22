#ifndef _H_AU_NETWORK_CONSOLE_SERVICE
#define _H_AU_NETWORK_CONSOLE_SERVICE

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

  bool Write(au::gpb::ConsolePacket *packet,
             au::ErrorManager *error);
  bool Read(au::gpb::ConsolePacket **packet,
            au::ErrorManager *error);

  // Methods related with au::Console
  std::string getPrompt();
  void evalCommand(std::string command, au::ErrorManager *error);
  virtual void autoComplete(ConsoleAutoComplete *info);
  void addEspaceSequence(std::string sequence);
  virtual void process_escape_sequence(std::string sequence) {
  };

private:

  // Full a message to be sent
  void FillMessage(au::gpb::ConsolePacket *message,
                   au::ErrorManager *error);

  int port_;
  SocketConnection *socket_connection_;

  // Prompt request delayed
  au::Cronometer cronometer_prompt_request_;
  std::string current_prompt_;
};

// Simple console to interact with the client

class ConsoleServiceClient : public ConsoleServiceClientBase,
                             public Console {
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

  ConsoleService(int port) : Service(port) {
  }

  virtual void runCommand(std::string command,
                          au::Environment *environment,
                          au::ErrorManager *error) {
    // Do soemthing...
  }

  virtual void autoComplete(ConsoleAutoComplete *info,
                            au::Environment *environment) {
  }

  virtual std::string getPrompt(au::Environment *environment) {
    return ">>";
  }

  void fill_message(au::ErrorManager *error,
                    au::gpb::ConsolePacket *message) {
    const au::vector<au::ErrorMessage>& error_messages =
      error->errors();
    for (size_t i = 0; i < error_messages.size(); i++) {
      ErrorMessage *error_message = error_messages[i];

      au::gpb::Message *m = message->add_message();
      m->set_txt(error_message->GetMultiLineMessage());

      switch (error_message->type()) {
        case ErrorMessage::item_message:
          m->set_type(au::gpb::Message::message);
          break;
        case ErrorMessage::item_warning:
          m->set_type(au::gpb::Message::warning);
          break;
        case ErrorMessage::item_error:
          m->set_type(au::gpb::Message::error);
          break;
      }
    }
  }

  void run(SocketConnection *socket_connection, bool *quit) {
    // Environment for this connection
    au::Environment environment;

    while (true) {
      // Read command line from the other side
      au::gpb::ConsolePacket *message = NULL;;
      au::Status s = readGPB(
        socket_connection->fd(), &message, -1);

      // Finish connection if not possible to read a message
      if (s != OK) {
        LM_W((
               "ConsoleService: Could not read message from client correctly (%s).Closing connection",
               status(s)));
        socket_connection->Close();
        if (message) {
          delete message;
        }
        return;
      }

      // Message that will be used in the answer...
      au::gpb::ConsolePacket answer_message;


      if (message->has_auto_complettion_command()) {
        // Auto completion request....
        ConsoleAutoComplete info(
          message->auto_complettion_command());
        autoComplete(&info, &environment);

        // Fill answer message with alternatives
        for (size_t i = 0; i < info.getNumAlternatives(); i++) {
          ConsoleAutoCompleteAlternative alternative =
            info.getAlternative(i);
          au::gpb::AutoCompletionAlternative *a =
            answer_message.add_auto_completion_alternatives();
          a->set_command(alternative.command);
          a->set_label(alternative.label);
          a->set_add_space_if_unique(
            alternative.add_space_if_unique);
        }
      } else if (message->has_prompt_request()) {
        // Ignore prompt request fild.
        answer_message.set_prompt(getPrompt(&environment));
      } else {
        // Run this command message...
        au::ErrorManager error;
        runCommand(message->command(), &environment, &error);
        fill_message(&error, &answer_message);
      }


      // Create the message to answer back to the client
      // Finish connection if not possible to read a message
      s = writeGPB(socket_connection->fd(), &answer_message);
      if (s != OK) {
        LM_W((
               "ConsoleService: Could not send message back to client correctly (%s).Closing connection",
               status(s)));
        socket_connection->Close();
        return;
      }
    }
  }
};
}
}

#endif  // ifndef _H_AU_NETWORK_CONSOLE_SERVICE
