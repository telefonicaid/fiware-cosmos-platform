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


#ifndef _AU_CONSOLE_COMMAND
#define _AU_CONSOLE_COMMAND

#include <string>

#include "au/console/ConsoleEntry.h"

namespace au {
namespace console {
/**
 * \brief Command currently being introduced by the user
 */
class ConsoleCommand {
public:

  ConsoleCommand();
  ConsoleCommand(const std::string& command);

  /**
   * \brief Add a character to the current command
   */
  void AddChar(char c);

  /**
   * \brief Add a string to current command
   */

  void AddString(const std::string& txt);

  /**
   * \brief Replace current command with a new command
   */

  void SetCommand(const std::string& command);

  /**
   * \brief Delete a char in the current command
   */

  void DeleteChar();

  /**
   * \brief Delete a work ( backward delete )
   */

  void DeleteWord();

  /**
   * \brief Remove the rest of the line ( forward )
   */

  void DeleteRestOfLine();

  /**
   * \brief Toggle the next two consecutive letters
   */

  void Toggle();

  /**
   * \brief Move pointer to the beginning of the line
   */
  void MoveHome();

  /**
   * \brief Mode pointer to the end of the line
   */
  void MoveEnd();

  /**
   * \brief Move position of cursor in the current command ( possitive and negative offsets allowed )
   */
  void MoveCursor(int offset);

  /**
   * \brief Check if the pointer is at the end of the line
   */
  bool IsCursorAtEnd() const;

  /**
   * \brief Get the current command
   */
  std::string command() const;

  /**
   * \brief Get the substring until the pointer position
   */
  std::string GetCommandUntilCursor() const;

  /**
   * \brief Get the pointer position (inside current command)
   */
  int cursor() const;

  /**
   * \brief Check if this command contains a string
   */
  bool Contains(const std::string& message);

  /**
   * \brief Process an entry from user
   */
  void ProcessEntry(ConsoleEntry& entry);

  /**
   * \brief Check if an entry given by user is "processable" by a ConsoleCommand
   */
  static bool CanProcessEntry(ConsoleEntry& entry);

private:

  std::string command_;      /**< Command we are currently reading from the user */
  int pos_;                  /**< Current position in the command */
};
}
}
#endif  // ifndef _AU_CONSOLE_COMMAND