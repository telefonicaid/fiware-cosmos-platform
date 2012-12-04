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

#include "au/ThreadManager.h"
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

class Console : public au::Thread {
public:

  Console();
  virtual ~Console();

  // Customize console
  virtual std::string getPrompt();
  virtual void evalCommand(const std::string& command);
  virtual void autoComplete(ConsoleAutoComplete *info);
  virtual void process_escape_sequence(const std::string& sequence) {
  };


  // Methods to run and stop the console in background
  void StartConsole(bool block_thread = false);
  void StopConsole();

  /**
   * Main method to write something on screen ( if foreground page-control is applyied )
   */
  void Write(const std::string& message);

  /**
   * \brief Main method to add a line with a particular color to the console
   */
  void Write(au::Color color, const std::string& message) {
    if (colors_) {
      Write(str(color, message));
    } else {
      Write(message);
    }
  }

  /**
   * \brief Write a warning ( message with some color or [WARNING] message )
   */
  void WriteWarningOnConsole(const std::string& message);

  /**
   * \brief Write an error ( message with some color or [ERROR] message )
   */
  void WriteErrorOnConsole(const std::string& message);

  /**
   * \brief Write an error ( message with some color or [ERROR] message )
   */
  void WriteBoldOnConsole(const std::string& message);


  /*
   * \brief General write function for all content inside a ErrorManager instance
   */
  void Write(au::ErrorManager& error);

  /**
   * \brief Write some messges included in error with a previx
   */
  void Write(au::ErrorManager& error, const std::string& prefix_message) {
    au::ErrorManager tmp_error;

    tmp_error.Add(error, prefix_message);
    Write(tmp_error);
  }

  /*
   * \brief Add a escape sequence to be considered in this console
   */
  void AddEspaceSequence(const std::string& sequence);

  /*
   * \brief Refresh content of the console
   */
  void Refresh();

  /*
   * \brief Wait showing a message on screen.... ( background message works )
   */
  int WaitWithMessage(const std::string& message, double sleep_time, ConsoleEntry *entry);

  /*
   * \brief Flush background messages to the console
   */
  void Flush();

  /*
   * \brief Append some text to the current command introduced by the user
   */
  void AppendToCommand(const std::string& txt);

  /*
   * \brief Get a string with previous history
   */
  std::string str_history(int limit);

  /**
   * \brief Activate or reactivate color output
   */

  void set_colors(bool value) {
    colors_ = value;
  }

  /**
   * \brief FLush accumulated messages even when console thread is no running
   */

  void FlushBackgroundMessages();

private:

  void PrintLines(const std::string&message);

  void RunThread();     // Main routine for background process

  void PrintCommand();
  bool IsInputReady() const;

  void ProcessAutoComplete(ConsoleAutoComplete *info);
  void ProcessInternalCommand(const std::string& sequence);
  void ProcessChar(char c);
  void ProcessEscapeSequenceInternal(const std::string& sequence);

  void ProcessBackgroundMessages();
  bool IsNormalChar(char c) const;

  void GetEntry(ConsoleEntry *entry);        // Get the next entry from console

  // History information ( all commands introduced before )
  ConsoleCommandHistory *command_history_;

  // Pending messages to be displayed in background
  au::Token token_pending_messages_;
  std::list<std::string> pending_messages_;

  // Flag to block background messages
  bool block_background_messages_;

  // Detector of escape sequences
  ConsoleEscapeSequence escape_sequence_;

  // Flag to finish the background thread
  bool quit_console_;

  // Flag to indicate if colors are allowed in this console
  bool colors_;
};
}
}
#endif  // ifndef _AU_CONSOLE

