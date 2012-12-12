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
#ifndef _AU_CONSOLE_COMMAND_HISTORY
#define _AU_CONSOLE_COMMAND_HISTORY

#include <sstream>
#include <string>
#include <vector>


namespace au {
namespace console {
class ConsoleCommand;

/**
 * \brief History commands introduced by user in a console
 */

class ConsoleCommandHistory {
public:

  ConsoleCommandHistory();
  ~ConsoleCommandHistory();

  /**
   * \brief Get current command
   */

  ConsoleCommand *GetCurrentCommand() const;

  /**
   * \brief Recover history commands from file
   */
  void RecoverHistory();

  /**
   * \brief Save history commands to file
   */
  void SaveHistory();

  /**
   * \brief Move up in the list of history
   */
  void MoveUp();

  /**
   * \brief Mode down in the list of history
   */
  void MoveDown();

  /**
   * \brief Add an empty command to collect another command from user
   */
  void AddNewEmptyCommand();

  /**
   * \brief Get a string with command history to be displayed on console
   */
  std::string str_history(size_t limit) const;

  /**
   * \brief Find a string in the history
   */
  int Find(int pos, const std::string& message);

  /**
   * \brief Get string command at a particular position
   */
  std::string GetStringCommand(size_t pos) const;


  /**
   * \brief Move to the last command
   */
  void MoveToLastcommand();

private:

  // Reverse search mode
  bool reverse_search_mode_;
  ConsoleCommand reverse_search_command_;

  std::string file_name_;  /**< Filename to store and recover history commands */
  size_t pos_;  /**< Position inside commands_ vector */
  std::vector<ConsoleCommand *> commands_;  /**< Commands instroduced so far */
};
}
}
#endif  // ifndef _AU_CONSOLE_COMMAND_HISTORY
