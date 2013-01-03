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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "au/console/ConsoleCommand.h"

#include "au/console/ConsoleCommandHistory.h"  // Own interface


namespace au {
namespace console {
ConsoleCommandHistory::ConsoleCommandHistory() {
  // Default filename to save and reload command history
  file_name_ = getenv("HOME");
  file_name_.append("/.history_console");

  // Recover history
  RecoverHistory();

  reverse_search_mode_ = false;

  // New command to start typing
  commands_.push_back(new ConsoleCommand());
  pos_ = commands_.size() - 1;
}

ConsoleCommandHistory::~ConsoleCommandHistory() {
  SaveHistory();

  while (1) {
    if (commands_.size() == 0) {
      break;
    }
    delete commands_[commands_.size() - 1];
    commands_.pop_back();
  }
}

ConsoleCommand *ConsoleCommandHistory::GetCurrentCommand() const {
  return commands_[pos_];
}

void ConsoleCommandHistory::RecoverHistory() {
  FILE *file = fopen(file_name_.c_str(), "r");

  if (!file) {
    return;
  }

  char line[1024];
  while (fgets(line, 1024, file) != NULL) {
    // Remove the \n char at the end of each line
    line[strlen(line) - 1] = '\0';

    commands_.push_back(new ConsoleCommand(line));
  }

  fclose(file);
}

void ConsoleCommandHistory::SaveHistory() {
  // Remove the last one if empty...
  if (commands_[commands_.size() - 1]->command() == "") {
    delete commands_[commands_.size() - 1];
    commands_.pop_back();

    if (pos_ > (commands_.size() - 1)) {
      pos_ = commands_.size() - 1;
    }
  }

  FILE *file = fopen(file_name_.c_str(), "w");

  if (!file) {
    return;
  }

  for (size_t i = 0; i < commands_.size(); ++i) {
    fprintf(file, "%s\n", commands_[i]->command().c_str());
  }

  fclose(file);
}

void ConsoleCommandHistory::MoveUp() {
  if (pos_ > 0) {
    --pos_;
  }
}

void ConsoleCommandHistory::MoveDown() {
  if (pos_ < (commands_.size() - 1)) {
    ++pos_;
  }
}

void ConsoleCommandHistory::AddNewEmptyCommand() {
  // Copy to the new one if we are editing a previous entry
  if (pos_ != (commands_.size() - 1)) {
    commands_[commands_.size() - 1]->SetCommand(commands_[pos_]->command());  // If we are repeting previous command, do not add again
  }
  if (commands_.size() > 1) {
    if (commands_[pos_]->command() == commands_[commands_.size() - 2]->command()) {
      commands_[commands_.size() - 1]->SetCommand("");
      pos_ = commands_.size() - 1;
      return;
    }
  }
  if (commands_[commands_.size() - 1]->command() != "") {
    // Add a new line for the new command
    commands_.push_back(new ConsoleCommand());
  }
  pos_ = commands_.size() - 1;
}

std::string ConsoleCommandHistory::str_history(size_t limit) const {
  std::ostringstream output;
  size_t pos = 0;

  if (limit > 0) {
    if (commands_.size() > limit) {
      pos = commands_.size() - limit;
    }
  }
  for (; pos < commands_.size(); ++pos) {
    output << commands_[pos]->command() << "\n";
  }
  return output.str();
}

int ConsoleCommandHistory::Find(int pos, const std::string& message) {
  if (commands_.size() == 0) {
    return -1;
  }

  if (pos < 0) {
    pos = commands_.size() - 1;
  }
  while (pos > 0) {
    if (commands_[pos]->Contains(message)) {
      return pos;
    }
    pos--;
  }

  return -1;
}

std::string ConsoleCommandHistory::GetStringCommand(size_t pos) const {
  if (pos >= commands_.size()) {
    return "";
  }

  return commands_[pos]->command();
}

void ConsoleCommandHistory::MoveToLastcommand() {
  pos_ = commands_.size() - 1;
}
}
}
