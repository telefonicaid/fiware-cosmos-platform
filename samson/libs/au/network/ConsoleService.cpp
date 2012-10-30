/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
#include "au/network/ConsoleService.h"  // Own interface

#include <vector>

namespace au {
namespace network {
ConsoleServiceClientBase::ConsoleServiceClientBase(int port) :
  port_(port) {
  socket_connection_ = NULL;
}

bool ConsoleServiceClientBase::Write(au::gpb::ConsolePacket *packet, au::ErrorManager *error) {
  if (!socket_connection_) {
    error->AddError("Not connected to any host");
    return false;
  }

  // Write on the socket
  au::Status s = writeGPB(socket_connection_->fd(), packet);

  if (s != OK) {
    error->AddError(au::str("Not possible to sent message (%s). Disconnecting...", status(s)));
    Disconnect(error);
    return false;
  }
  return true;
}

bool ConsoleServiceClientBase::Read(au::gpb::ConsolePacket **packet, au::ErrorManager *error) {
  if (!socket_connection_) {
    error->AddError("Not connected to any host");
    return false;
  }

  Status s = readGPB(socket_connection_->fd(), packet, -1);
  if (s != OK) {
    error->AddError(au::str("Not possible to receive answer (%s). Disconnecting...", status(s)));
    Disconnect(error);
    return false;
  }
  return true;
}

void ConsoleServiceClientBase::FillMessage(au::gpb::ConsolePacket *message, au::ErrorManager *error) {
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

void ConsoleServiceClientBase::Disconnect(au::ErrorManager *error) {
  if (socket_connection_) {
    error->AddWarning(au::str("Closing connection with %s\n", socket_connection_->host_and_port() .c_str()));

    socket_connection_->Close();
    delete socket_connection_;
    socket_connection_ = NULL;
  }
}

void ConsoleServiceClientBase::Connect(std::string host, au::ErrorManager *error) {
  // Disconnect from previos one if any...
  Disconnect(error);

  if (error->IsActivated()) {
    return;
  }

  // Try connection
  au::Status s = SocketConnection::Create(host, port_, &socket_connection_);
  if (s != OK) {
    Disconnect(error);
    error->AddError(au::str("Not possible to connect with %s (%s)\n", host.c_str(), status(s)));
  } else {
    error->AddWarning(au::str("Connection stablished with %s\n", host.c_str()));
  }
}

std::string ConsoleServiceClientBase::getPrompt() {
  if( current_prompt_ != "" )
    if (cronometer_prompt_request_.seconds() < 2) {
      return current_prompt_;
    }

  // Prepare message to be send to server
  au::gpb::ConsolePacket m;
  m.set_prompt_request("yes");

  // Send request to server
  au::ErrorManager error;
  // Send to server
  if (!Write(&m, &error)) {
    return current_prompt_;
  }

  // Recover answer from server
  // Read answer from server
  au::gpb::ConsolePacket *answer;
  if (!Read(&answer, &error)) {
    return current_prompt_;
  }

  current_prompt_ = answer->prompt();
  delete answer;

  return current_prompt_;
}

void ConsoleServiceClientBase::evalCommand(std::string command, au::ErrorManager *error) {
  // Establish connection
  au::CommandLine cmdLine;

  cmdLine.Parse(command);

  if (cmdLine.get_num_arguments() == 0) {
    return;
  }

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
    error->AddError("Not connected to any host. Type connect 'host'");
    return;
  } else {
    // Write command to the server
    au::gpb::ConsolePacket m;
    m.set_command(command);
    if (!Write(&m, error)) {
      return;
    }

    // Read answer
    au::gpb::ConsolePacket *answer;

    if (!Read(&answer, error)) {
      return;
    }

    // Transform into a au::ErrorManager
    FillMessage(answer, error);
    delete answer;
  }
}

void ConsoleServiceClientBase::autoComplete(ConsoleAutoComplete *info) {
  // Options for connection and disconnection...
  if (info->completingFirstWord()) {
    info->add("connect");
    info->add("disconnect");
  }

  // Prepare message to be send to server
  au::gpb::ConsolePacket m;
  m.set_auto_complettion_command(info->getCurrentCommand());

  au::ErrorManager error;   // Not used error

  // Send to server
  if (!Write(&m, &error)) {
    return;
  }

  // Read answer from server
  au::gpb::ConsolePacket *answer;
  if (!Read(&answer, &error)) {
    return;
  }

  // Fill info structure with received information
  for (int i = 0; i < answer->auto_completion_alternatives_size(); i++) {
    std::string label = answer->auto_completion_alternatives(i).label();
    std::string command = answer->auto_completion_alternatives(i).command();
    bool add_space = answer->auto_completion_alternatives(i). add_space_if_unique();
    info->add(label, command, add_space);
  }
}

void ConsoleServiceClientBase::addEspaceSequence(std::string sequence) {
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

void ConsoleServiceClient::autoComplete(ConsoleAutoComplete *info) {
  ConsoleServiceClientBase::autoComplete(info);
}

std::string ConsoleServiceClient::getPrompt() {
  return ConsoleServiceClientBase::getPrompt();
}

// Fill message to be sent to client
void ConsoleService::fill_message(au::ErrorManager *error, au::gpb::ConsolePacket *message) {
  const au::vector<au::ErrorMessage>& error_messages = error->errors();
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

// main function executed by all threads
void ConsoleService::run(SocketConnection *socket_connection, bool *quit) {
  // Environment for this connection
  au::Environment environment;

  while (true) {
    // Read command line from the other side
    au::gpb::ConsolePacket *message = NULL;
    au::Status s = readGPB(socket_connection->fd(), &message, -1);

    // Finish connection if not possible to read a message
    if (s != OK) {
      LM_W(("ConsoleService: Could not read message from client correctly (%s).Closing connection", status(s)));
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
      ConsoleAutoComplete info(message->auto_complettion_command());
      autoComplete(&info, &environment);

      // Fill answer message with alternatives
      for (size_t i = 0; i < info.getNumAlternatives(); i++) {
        ConsoleAutoCompleteAlternative alternative = info.getAlternative(i);
        au::gpb::AutoCompletionAlternative *a = answer_message.add_auto_completion_alternatives();
        a->set_command(alternative.command);
        a->set_label(alternative.label);
        a->set_add_space_if_unique(alternative.add_space_if_unique);
      }
    } else if (message->has_prompt_request()) {
      // Prompt request
      answer_message.set_prompt(getPrompt(&environment));
    } else {
      // Run this command message...
      au::ErrorManager error;
      runCommand(message->command(), &environment, &error);
      fill_message(&error, &answer_message);
    }

    // Answer back to the client
    s = writeGPB(socket_connection->fd(), &answer_message);
    // Finish connection if not possible to read a message
    if (s != OK) {
      LM_W(("ConsoleService: Could not send message back to client correctly (%s).Closing connection", status(s)));
      socket_connection->Close();
      return;
    }
  }
}
}
} // end of namespace
