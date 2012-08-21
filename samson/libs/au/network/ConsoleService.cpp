
#include "ConsoleService.h"  // Own interface

namespace au {
namespace network {
ConsoleServiceClientBase::ConsoleServiceClientBase(int port)
  : port_(port) {
  socket_connection_ = NULL;
  current_prompt_ = ">>";
}

bool ConsoleServiceClientBase::Write(
  au::gpb::ConsolePacket *packet, au::ErrorManager *error) {
  if (!socket_connection_) {
    error->AddError("Not connected to any host");
    return false;
  }

  // Write on the socket
  au::Status s = writeGPB(socket_connection_->fd(), packet);

  if (s != OK) {
    error->AddError(au::str(
                      "Not possible to sent message (%s). Disconnecting...",
                      status(s)));
    Disconnect(error);
    return false;
  }
  return true;
}

bool ConsoleServiceClientBase::Read(
  au::gpb::ConsolePacket **packet, au::ErrorManager *error) {
  if (!socket_connection_) {
    error->AddError("Not connected to any host");
    return false;
  }

  Status s = readGPB(socket_connection_->fd(), packet, -1);
  if (s != OK) {
    error->AddError(au::str(
                      "Not possible to receive answer (%s). Disconnecting...",
                      status(s)));
    Disconnect(error);
    return false;
  }
  return true;
}

void ConsoleServiceClientBase::FillMessage(
  au::gpb::ConsolePacket *message, au::ErrorManager *error) {
  for (int i = 0; i < message->message_size(); i++) {
    std::string txt = message->message(i).txt();

    switch (message->message(i).type()) {
      case au::gpb::Message::message:
        error->AddMessage(txt);
        break;
      case au::gpb::Message::warning:
        error->AddWarning(txt);
        break;
      case au::gpb::Message::error:
        error->AddError(txt);
        break;
    }
  }
}

void ConsoleServiceClientBase::Disconnect(
  au::ErrorManager *error) {
  if (socket_connection_) {
    error->AddWarning(au::str("Closing connection with %s\n"
                              , socket_connection_->host_and_port()
                              .c_str()));

    socket_connection_->Close();
    delete socket_connection_;
    socket_connection_ = NULL;
  }
}

void ConsoleServiceClientBase::Connect(std::string host,
                                       au::ErrorManager *error) {
  // Disconnect from previos one if any...
  Disconnect(error);

  if (error->IsActivated())
    return;

  // Try connection
  au::Status s = SocketConnection::Create(host, port_,
                                          &socket_connection_);
  if (s != OK) {
    Disconnect(error);
    error->AddError(au::str(
                      "Not possible to connect with %s (%s)\n",
                      host.c_str(), status(s)));
  } else {
    error->AddWarning(au::str("Connection stablished with %s\n",
                              host.c_str()));
  }
}

std::string ConsoleServiceClientBase::getPrompt() {
  if (cronometer_prompt_request_.seconds() < 2)
    return current_prompt_;

  // Prepare message to be send to server
  au::gpb::ConsolePacket m;
  m.set_prompt_request("yes");

  // Send request to server
  au::ErrorManager error;
  // Send to server
  if (!Write(&m, &error))
    return current_prompt_;

  // Recover answer from server
  // Read answer from server
  au::gpb::ConsolePacket *answer;
  if (!Read(&answer, &error))
    return current_prompt_;

  current_prompt_ = answer->prompt();
  delete answer;

  return current_prompt_;
}

void ConsoleServiceClientBase::evalCommand(
  std::string command, au::ErrorManager *error) {
  // Establish connection
  au::CommandLine cmdLine;

  cmdLine.parse(command);

  if (cmdLine.get_num_arguments() == 0)
    return;

  std::string main_command = cmdLine.get_argument(0);

  if (main_command == "disconnect") {
    Disconnect(error);
    return;
  }

  if (main_command == "connect") {
    if (cmdLine.get_num_arguments() < 2) {
      error->AddError("Usage: connect host");
      return;
    }
    Connect(cmdLine.get_argument(1), error);
    return;
  }

  if (!socket_connection_) {
    error->AddError(
      "Not connected to any host. Type connect 'host'");
    return;
  } else {
    // Write command to the server
    au::gpb::ConsolePacket m;
    m.set_command(command);
    if (!Write(&m, error))
      return;

    // Read answer
    au::gpb::ConsolePacket *answer;

    if (!Read(&answer, error))
      return;

    // Transform into a au::ErrorManager
    FillMessage(answer, error);
    delete answer;
  }
}

void ConsoleServiceClientBase::autoComplete(
  ConsoleAutoComplete *info) {
  // Options for connection and disconnection...
  if (info->completingFirstWord()) {
    info->add("connect");
    info->add("disconnect");
  }

  // Prepare message to be send to server
  au::gpb::ConsolePacket m;
  m.set_auto_complettion_command(info->getCurrentCommand());

  au::ErrorManager error;                 // Not used error

  // Send to server
  if (!Write(&m, &error))
    return;

  // Read answer from server
  au::gpb::ConsolePacket *answer;
  if (!Read(&answer, &error))
    return;

  // Fill info structure with received information
  for (int i = 0; i < answer->auto_completion_alternatives_size();
       i++)
  {
    std::string label =
      answer->auto_completion_alternatives(i).label();
    std::string command =
      answer->auto_completion_alternatives(i).command();
    bool add_space =
      answer->auto_completion_alternatives(i).
      add_space_if_unique();
    info->add(label, command, add_space);
  }
}

void ConsoleServiceClientBase::addEspaceSequence(
  std::string sequence) {
}

ConsoleServiceClient::ConsoleServiceClient(int port) :
  ConsoleServiceClientBase(port) {
}

// Write all messages on console
void ConsoleServiceClient::evalCommand(std::string command) {
  au::ErrorManager error;

  ConsoleServiceClientBase::evalCommand(command, &error);
  Console::write(&error);
}

void ConsoleServiceClient::autoComplete(
  ConsoleAutoComplete *info) {
  ConsoleServiceClientBase::autoComplete(info);
}

std::string ConsoleServiceClient::getPrompt() {
  return ConsoleServiceClientBase::getPrompt();
}
}
}  // end of namespace
