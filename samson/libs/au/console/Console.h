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

#include "au/Thread.h"
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
class ConsoleCommand;

/**
 * \brief Full-featured console for easy interaction with user
 *
 * Subclasses should implement "evalCommand" to respond to a command introduced by user:
 * virtual void EvalCommand(const std::string& command);
 *
 * Optional method to get custom prompts:
 * virtual std::string GetPrompt();
 *
 * Optional method to autocomplete user command entry with "tab" key:
 * virtual void AutoComplete(ConsoleAutoComplete *info);
 *
 * Optional, subclasses could implement following method for escape sequence handling:
 * virtual void ProcessEscapeSequence(const std::string& sequence);
 *
 */

class Console : public au::Thread {
public:

  Console();
  virtual ~Console();

  /**
   * \brief Get prompt to be shown to the user
   */
  virtual std::string GetPrompt();
  /**
   * \brief Main method to eval a command instroduced by user
   */
  virtual void EvalCommand(const std::string& command);

  /**
   * \brief Get all auto-complete options for currently introduced command
   */
  virtual void AutoComplete(ConsoleAutoComplete *info);

  /**
   * \brief Process a escape sequence introduced by user ( see AddEspaceSequence )
   */
  virtual void ProcessEscapeSequence(const std::string& sequence) {
  };

  /**
   * \brief Start background thread to read command from user.
   * \param block_thread Specifies if the calling thread has to be blocked until StopConsole() is called
   */
  void StartConsole(bool block_thread = false);

  /**
   * \brief Stop background thread ( it is robust agains multiple calls to this method )
   */
  void StopConsole();

  /**
   * \brief Main method to write something on screen ( if foreground page-control is applyied )
   */
  void Write(const std::string& message);

  /**
   * \brief Main method to add a line with a particular color to the console
   *
   * If set_colors(false) has been set, no color-codes are emitted to the output
   */
  void Write(au::Color color, const std::string& message);

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

  /**
   * \brief General write function for all content inside a ErrorManager instance
   */
  void Write(au::ErrorManager& error);

  /**
   * \brief Write some messages included in error with a prefix
   */
  void Write(au::ErrorManager& error, const std::string& prefix_message);

  /**
   * \brief Add a escape sequence to be considered in this console
   */
  void AddEspaceSequence(const std::string& sequence);

  /**
   * \brief Refresh content of the console
   */
  void Refresh();

  /*
   * \brief Wait showing a message on screen.... ( background message works )
   */
  int WaitWithMessage(const std::string& message, double sleep_time, ConsoleEntry *entry);

  /**
   * \brief Flush background messages to the console
   */
  void Flush();

  /**
   * \brief Append some text to the current command introduced by the user
   */
  void AppendToCommand(const std::string& txt);

  /**
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
   * \brief Flush accumulated messages (even when console thread is no running)
   * This command is useful if au::Console is only used to accumulate output of a process
   */
  void FlushBackgroundMessages();

private:

  void PrintCommand();
  void PrintCurrentCommand();
  void PrintReverseSearchCommand();

  void RunThread();     // Main routine for background process
  void PrintLines(const std::string&message);
  bool IsInputReady() const;
  void ProcessAutoComplete(ConsoleAutoComplete *info);

  void ProcessReturn();
  void ProcessTab();

  void ProcessEscapeSequenceInternal(const std::string& sequence);
  void ProcessBackgroundMessages();
  void GetEntry(ConsoleEntry *entry);        // Get the next entry from console
  void ProcessEntry(ConsoleEntry& entry);

  void StartReverseSearchMode();
  void StopReverseSearchMode();
  void UpdateReverseSearchMode();
  void NextReverseSearch();

  // History information ( all commands introduced before and current command )
  ConsoleCommandHistory *command_history_;

  // Pending messages to be displayed in background
  au::Token token_pending_messages_;
  std::list<std::string> pending_messages_;

  // Flag to block background messages
  bool block_background_messages_;

  // Detector of escape sequences
  ConsoleEscapeSequence escape_sequence_;

  // Flag to indicate if colors are allowed in this console
  bool colors_;

  // Reverse search mode
  bool reverse_search_mode_;
  int reverse_search_history_pos_;
  au::SharedPointer<ConsoleCommand> reverse_search_command_;
};
}
}
#endif  // ifndef _AU_CONSOLE

