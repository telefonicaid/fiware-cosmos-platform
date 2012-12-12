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



#include "au/console/ConsoleCommand.h"  // Own interface

namespace au {
namespace console {
ConsoleCommand::ConsoleCommand() {
  pos_ = 0;
}

ConsoleCommand::ConsoleCommand(const std::string& command) {
  command_ = command;
  pos_ = command.length();
}

void ConsoleCommand::AddChar(char c) {
  command_.insert(pos_, 1, c);
  ++pos_;
}

void ConsoleCommand::AddString(const std::string& txt) {
  command_.insert(pos_, txt);
  pos_ += txt.length();
}

void ConsoleCommand::DeleteChar() {
  // Delete last char introduced ( at pos )
  if (pos_ > 0) {
    command_.erase(pos_ - 1, 1);
    --pos_;
  }
}

void ConsoleCommand::MoveHome() {
  pos_ = 0;
}

void ConsoleCommand::MoveEnd() {
  pos_ = command_.length();
}

void ConsoleCommand::MoveCursor(int offset) {
  pos_ += offset;
  if (pos_ < 0) {
    pos_ = 0;
  }
  if (pos_ > static_cast<int>(command_.length())) {
    pos_ = command_.length();
  }
}

void ConsoleCommand::DeleteWord() {
  do {
    DeleteChar();
  } while ((pos_ > 0) && (command_[pos_ - 1] != ' '));
}

void ConsoleCommand::Toggle() {
  if (command_.length() < 2) {
    return;
  }
  if (pos_ < 1) {
    return;
  }

  int pos_toogle = pos_;
  if (pos_toogle >= (int)command_.length()) {
    pos_toogle = command_.length() - 1;
  }
  char a = command_[pos_toogle];
  command_[pos_toogle] = command_[pos_toogle - 1];
  command_[pos_toogle - 1] = a;

  pos_ = pos_toogle + 1;
}

void ConsoleCommand::DeleteRestOfLine() {
  command_.erase(pos_, command_.length() - pos_);
}

std::string ConsoleCommand::command() const {
  return command_;
}

std::string ConsoleCommand::GetCommandUntilCursor() const {
  return command_.substr(0, pos_);
}

void ConsoleCommand::SetCommand(const std::string& command) {
  command_ = command;
  pos_ = command.length();
}

int ConsoleCommand::cursor() const {
  return pos_;
}

bool ConsoleCommand::IsCursorAtEnd() const {
  return (pos_ == (int)command_.length());
}

bool ConsoleCommand::Contains(const std::string& message) {
  return (command_.find(message) != std::string::npos);
}

void ConsoleCommand::ProcessEntry(ConsoleEntry& entry) {
  if (entry.IsNormalChar()) {
    AddChar(entry.getChar());
    return;
  }

  if (entry.isChar()) {
    char c = entry.getChar();
    switch (c) {
      case 1:  // C-a
        MoveHome();
        break;
      case 5:  // C-e
        MoveEnd();
        break;
      case 11:  // C-k
        DeleteRestOfLine();
        break;
      case 20:  // C-t
        Toggle();
        break;
      case 23:  // C-del
        DeleteWord();
        break;
      case 127:  // del
        DeleteChar();
        break;
    }
    return;
  }
}

bool ConsoleCommand::CanProcessEntry(ConsoleEntry& entry) {
  if (entry.IsNormalChar()) {
    return true;
  }

  if (entry.isChar()) {
    char c = entry.getChar();
    switch (c) {
      case 1:
      case 5:
      case 11:
      case 20:
      case 23:
      case 127:
        return true;

        break;

      default:
        return false;

        break;
    }
  }

  return false;
}
}
}