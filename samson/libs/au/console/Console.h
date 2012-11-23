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

#ifndef _AU_CONSOLE
#define _AU_CONSOLE

#include <termios.h>                // termios

#include <list>
#include <string>

#include "au/console/ConsoleEntry.h"
#include "au/console/ConsoleEscapeSequence.h"

#include "au/ErrorManager.h"
#include "au/log/LogCentralPlugin.h"
#include "au/log/LogFormatter.h"
#include "au/mutex/Token.h"

namespace au {
namespace console {
class Console;
class ConsoleAutoComplete;
class ConsoleCommandHistory;

/**
 * \brief Full-featured console for easy interaction with user
 *
 * Subclasses should implement "evalCommand" to respond to a command introduced by user:
 * virtual void evalCommand(const std::string& command);
 *
 * Optional method to get custom prompts:
 * virtual std::string getPrompt();
 *
 * Optional method to autocomplete user command entry with "tab" key:
 * virtual void autoComplete(ConsoleAutoComplete *info);
 *
 * Optional, subclasses could implement following method for escape sequence handling:
 * virtual void process_escape_sequence(const std::string& sequence);
 *
 */

class Console {
  // History information ( all commands introduced before )
  ConsoleCommandHistory *command_history;

  // Pending messages to be displayed
  pthread_t t_running;
  std::list< std::string > pending_messages;
  au::Token token_pending_messages;

  // Flag to block background messages
  bool block_background_messages;

  // Flag to quit internal loop
  bool quit_console;

  // Counter used only for testing
  int counter;

  // Detector of escape sequences
  ConsoleEscapeSequence escape_sequence;

public:
  Console();
  virtual ~Console();

  void runConsole();
  void runConsoleInBackground();
  void quitConsole();

  /* Methods to write things on screen */
  void writeWarningOnConsole(const std::string& message);
  void writeErrorOnConsole(const std::string& message);
  void writeOnConsole(const std::string& message);

  void write(au::ErrorManager *error);

  // Customize console
  virtual std::string getPrompt();
  virtual void evalCommand(const std::string& command);
  virtual void autoComplete(ConsoleAutoComplete *info);

  void addEspaceSequence(const std::string& sequence);
  virtual void process_escape_sequence(const std::string& sequence) {
  };

  void refresh();

  // Wait showing a message on screen.... ( background message works )
  int waitWithMessage(const std::string& message, double sleep_time, ConsoleEntry *entry);

  // Make sure all messages are shown
  void flush();

  /* ask about Console quit status */
  bool isQuitting();

  // Append to current command
  void appendToCommand(const std::string& txt);

  // Get the history string
  std::string str_history(int limit);

private:
  void print_command();
  bool isImputReady();

  void process_auto_complete(ConsoleAutoComplete *info);
  void process_char(char c);

  void internal_process_escape_sequence(std::string sequence);
  void internal_command(const std::string& sequence);

  void process_background();
  bool isNormalChar(char c);

  void write(const std::string& message);

  // Get the next entry from console
  void getEntry(ConsoleEntry *entry);
};
}
}
#endif  // ifndef _AU_CONSOLE

