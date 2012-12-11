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
#include "au/console/Console.h"  // Own interface

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>

#include "au/ThreadManager.h"
#include "au/console/ConsoleAutoComplete.h"
#include "au/console/ConsoleCommand.h"
#include "au/console/ConsoleCommandHistory.h"
#include "au/console/ConsoleEscapeSequence.h"
#include "au/containers/list.h"
#include "au/log/LogMain.h"
#include "au/mutex/TokenTaker.h"
#include "au/statistics/Cronometer.h"
#include "au/string/StringUtilities.h"
#include "au/utils.h"


struct termios old_tio, new_tio;

std::string GetEscapeSequenceDescription(const std::string& sequece) {
  std::ostringstream output;

  output << "( " << sequece << " ";
  for (size_t i = 0; i < sequece.length(); ++i) {
    output << au::str("[%d]", static_cast<int>(sequece[i]));
  }
  output << " )";
  return output.str();
}

void finish_console_mode() {
  /* set the new settings immediately */
  tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
}

void init_console_mode() {
  /* get the terminal settings for stdin */
  tcgetattr(STDIN_FILENO, &old_tio);

  /* we want to keep the old setting to restore them a the end */
  new_tio = old_tio;

  /* disable canonical mode (buffered i/o) and local echo */
  new_tio.c_lflag &= (~ICANON & ~ECHO);

  /* set the new settings immediately */
  tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);

  atexit(finish_console_mode);
}

namespace au {
namespace console {
Console *current_console = NULL;
void handle_winch(int sig);
void handle_tstp(int sig);

Console::Console() : au::Thread("Console"),
                     command_history_(new ConsoleCommandHistory()),
                     token_pending_messages_("token_pending_messages"),
                     block_background_messages_(false),  // By default, background messages are not blocked ( esc - b for toogle )
                     colors_(true),  // By default colors are allowed
                     reverse_search_mode_(false),
                     reverse_search_command_(new ConsoleCommand()) {
}

Console::~Console() {
  command_history_->SaveHistory();
  delete command_history_;
}

void Console::StartConsole(bool block_thread) {
  StartThread();
  if (block_thread) {
    JoinThread();  // Wait for background thread to finish
    printf("\n"); // Print a last return to avoid writting after console prompt
  }
  
}

void Console::StopConsole() {
  StopThread();   // Stop background thread
}

void Console::PrintCommand() {
  if (reverse_search_mode_) {
    PrintReverseSearchCommand();        // Special mode to search command backwards
    return;
  }
  PrintCurrentCommand();      // Normal moode, print curretn command
}

void Console::PrintReverseSearchCommand() {
  ClearTerminalLine();

  std::string found_command;
  if (reverse_search_history_pos_ == -1) {
    found_command = "Not found";
  } else {
    found_command = command_history_->GetStringCommand(reverse_search_history_pos_);
  }

  printf("[Reverse Search: %s ] %s", reverse_search_command_->command().c_str(), found_command.c_str());
  printf("\r");
  printf("[Reverse Search: %s", reverse_search_command_->command().c_str());
  fflush(stdout);
}

void Console::PrintCurrentCommand() {
  // Get prompt
  std::string prompt = GetPrompt();
  // Get command and position inside the command
  std::string _command = command_history_->GetCurrentCommand()->command();
  int _pos = command_history_->GetCurrentCommand()->cursor();

  // Compute terminal width and available space for the command
  int w = GetTerminalWidth();
  int w_available = w - prompt.length() - 7;
  int command_len = static_cast<int>(_command.length());

  ClearTerminalLine();
  int command_begin = 0;
  int command_end = command_len;

  // Modify this if command is too large
  if (command_len > w_available) {
    if (_pos > command_len - w_available) {
      command_begin = command_end - w_available;  // End of the command
    } else {
      // Position at the middle
      command_begin = std::max(_pos - w_available / 2, 0);
      command_end = std::min(command_begin + w_available, command_len);
    }
  }

  // Print the entire command
  printf("%s ", GetPrompt().c_str());
  if (command_begin > 0) {
    printf("...");
  }
  for (int i = command_begin; i < command_end; ++i) {
    printf("%c", _command[i]);
  }
  if (command_end < command_len) {
    printf("...");
  }

  // Put cursor in the rigth place ( printing again text )
  if (_pos != command_end) {
    printf("\r");
    printf("%s ", GetPrompt().c_str());
    if (command_begin > 0) {
      printf("...");
    }
    for (int i = command_begin; i < _pos; ++i) {
      printf("%c", _command[i]);
    }
  }

  fflush(stdout);
}

bool Console::IsInputReady() const {
  struct timeval timeVal;

  timeVal.tv_sec = 0;
  timeVal.tv_usec = 0;

  fd_set rFds;
  FD_ZERO(&rFds);
  FD_SET(0, &rFds);

  int s = select(1, &rFds, NULL, NULL, &timeVal);

  return (s == 1);
}

void Console::ProcessAutoComplete(ConsoleAutoComplete *info) {
  std::string help_message = info->getHelpMessage();

  if ((help_message.length() > 0) || info->necessary_print_last_words_alternatives()) {
    PrintCommand();
    printf("\n");
  }

  if (info->necessary_print_last_words_alternatives()) {
    info->print_last_words_alternatives();
  }
  if (help_message.length() > 0) {
    printf("Help: %s\n", help_message.c_str());   // Add necessary stuff...
  }
  command_history_->GetCurrentCommand()->AddString(info->stringToAppend());
  PrintCommand();
}

void Console::ProcessReturn() {
  PrintCommand();
  printf("\n");

  std::string command = command_history_->GetCurrentCommand()->command();
  EvalCommand(command);    // Eval the command with virtual method EvalCommand
  command_history_->AddNewEmptyCommand();      // New command in history
  PrintCommand();
}

void Console::ProcessTab() {
  ConsoleAutoComplete *info = new ConsoleAutoComplete(command_history_->GetCurrentCommand()->GetCommandUntilCursor());

  AutoComplete(info);
  ProcessAutoComplete(info);
  delete info;
}

void Console::AddEspaceSequence(const std::string& sequence) {
  escape_sequence_.addSequence(sequence);
}

void Console::ProcessEscapeSequenceInternal(const std::string& sequence) {
  if (sequence == au::str("b")) {
    // When esc-b is typed, we toggle value of block_background_messages_
    // When block_background_messages_ is true, messages sent to the console from other threads are displayed.
    // When block_background_messages_ is false, these messages are retained until block_background_messages_ is true

    if (!IsThreadRunning()) {
      LOG_SW(("Console thread is not yet running"));
      return;
    }

    // This method can only be called from background thread while processing escalpe sequences
    if (!IsBackgroundThread()) {
      return;
    }

    // Toggle value of block_background_messages_ flag
    block_background_messages_ = !block_background_messages_;

    if (block_background_messages_) {
      WriteWarningOnConsole("Background messages blocked ( press esc b to show again )\n");
    } else {
      WriteWarningOnConsole("Background messages are shown again\n");
    }

    ClearTerminalLine();
    Flush();
    PrintCommand();

    return;
  }

  if (sequence == au::str("%c", 127)) {
    // Remove a word
    command_history_->GetCurrentCommand()->DeleteWord();
    PrintCommand();
    return;
  }

  if (sequence == "[A") {
    // Mode up
    command_history_->MoveUp();
    PrintCommand();
    return;
  }
  if (sequence == "[B") {
    // Move down
    command_history_->MoveDown();
    PrintCommand();
    return;
  }
  if (sequence == "[C") {
    // Move forward
    command_history_->GetCurrentCommand()->MoveCursor(1);
    PrintCommand();
    return;
  }
  if (sequence == "[D") {
    // Move backward
    command_history_->GetCurrentCommand()->MoveCursor(-1);
    PrintCommand();
    return;
  }

  if (sequence == "au") {
    command_history_->GetCurrentCommand()->AddString("Andreu Urruela (andreu@tid.es,andreu@urruela.com)");
    PrintCommand();
    return;
  }

  if (sequence == "h") {
    ClearTerminalLine();

    // Print history
    printf("---------------------------------------------\n");
    printf(" HISTORY\n");
    printf("---------------------------------------------\n");
    printf("%s", command_history_->str_history(10).c_str());
    printf("---------------------------------------------\n");

    // Print command again
    PrintCommand();
    return;
  }

  ProcessEscapeSequence(sequence);
}

void Console::PrintLines(const std::string&message) {
  // Divide input message in lines
  std::vector<std::string> lines = au::split(message, '\n');

  // Get terminal size
  int x, y;
  au::GetTerminalSize(1, &x, &y);
  size_t console_width = x;

  std::string line_to_print;
  for (size_t i = 0; i < lines.size(); ++i) {
    if (lines[i].length() <= console_width) {
      line_to_print = lines[i];
    } else {
      line_to_print = lines[i].substr(0, x - 3) + "...";
    }
    printf("%s\n", line_to_print.c_str());
  }
}

void Console::ProcessBackgroundMessages() {
  if (block_background_messages_) {
    return;
  }

  if (pending_messages_.size() != 0) {
    au::TokenTaker tt(&token_pending_messages_);

    ClearTerminalLine();

    while (pending_messages_.size() != 0) {
      std::string txt = pending_messages_.front();
      pending_messages_.pop_front();
      PrintLines(txt);
    }

    PrintCommand();
    fflush(stdout);
  }
}

void Console::Flush() {
  ProcessBackgroundMessages();
}

void Console::FlushBackgroundMessages() {
  while (pending_messages_.size() != 0) {
    std::string txt = pending_messages_.front();
    pending_messages_.pop_front();
    PrintLines(txt);
  }
}

void Console::GetEntry(ConsoleEntry *entry) {
  char c;

  if (read(0, &c, 1) != 1) {
    LM_X(1, ("reading from stdin failed"));
  }
  if (c == 27) {
    // Escape sequence
    escape_sequence_.init();

    while (true) {
      if (read(0, &c, 1) != 1) {
        LM_X(1, ("reading from stdin failed"));   // Add the new character
      }
      escape_sequence_.add(c);

      // Detect if it is a valid sequence code
      SequenceDetectionCode sequence_detection_code = escape_sequence_.checkSequence();

      if (sequence_detection_code == sequence_finished) {
        entry->setEscapeSequence(escape_sequence_.getCurrentSequence());
        return;
      }

      if (sequence_detection_code == sequence_non_compatible) {
        entry->setUnknownEscapeSequence(escape_sequence_.getCurrentSequence());
        return;
      }

      // Continue reading escape sequence...
    }
  } else {
    entry->setChar(c);
  }
}

void Console::ProcessEntry(ConsoleEntry& entry) {
  if (reverse_search_mode_) {
    if (ConsoleCommand::CanProcessEntry(entry)) {
      reverse_search_command_->ProcessEntry(entry);
      UpdateReverseSearchMode();
      PrintCommand();
      return;
    }

    // Type C-r again
    if (entry.isChar() && (entry.getChar() == 18)) {
      NextReverseSearch();
      PrintCommand();
      return;
    }

    // Any other key, we come back to normal mode and process new key
    StopReverseSearchMode();
    ProcessEntry(entry);
    return;
  }
  ;

  // If entry can be process by ConsoleCommand, just do it
  if (ConsoleCommand::CanProcessEntry(entry)) {
    command_history_->GetCurrentCommand()->ProcessEntry(entry);
    PrintCommand();
    return;
  }

  // Special keys for the console
  if (entry.isChar()) {
    char c = entry.getChar();

    switch (c) {
      case '\n':
        ProcessReturn();
        break;

      case '\t':
        ProcessTab();
        break;

      case 7:
        printf("%c", c);    // bell
        break;

      case 18:
        StartReverseSearchMode();
        PrintCommand();   // update command on screen
        break;

      default:
        WriteWarningOnConsole(au::str("Ignoring unkown char (%d)", c));
        break;
    }
  } else if (entry.isEscapeSequence()) {
    std::string seq = entry.getEscapeSequece();
    ProcessEscapeSequenceInternal(escape_sequence_.getCurrentSequence());
  } else if (entry.isUnknownEscapeSequence()) {
    std::string seq = entry.getEscapeSequece();
    WriteWarningOnConsole(au::str("Unknown escape sequence (%s)", GetEscapeSequenceDescription(seq).c_str()));
  }
}

void Console::RunThread() {
  // Init console
  init_console_mode();

  // Signal to handle terminal changes...
  current_console = this;
  signal(SIGWINCH, handle_winch);
  signal(SIGTSTP, handle_tstp);

  // First version with just the promtp
  PrintCommand();

  while (true) {
    if (IsThreadQuiting()) {
      return;  // StopConsole has been called
    }

    while (!IsInputReady()) {
      // Review background messages
      ProcessBackgroundMessages();
      usleep(20000);
      
      if (IsThreadQuiting()) {
        return;  // StopConsole has been called
      }
    }

    // Read an entry or a escape sequence
    ConsoleEntry entry;
    GetEntry(&entry);
    ProcessEntry(entry);
  }

  // Clear line to quit nicely...
  ClearTerminalLine();

  // Remove the signal handler...
  signal(SIGWINCH, SIG_IGN);
}

void Console::Write(au::Color color, const std::string& message) {
  if (colors_) {
    Write(str(color, message));
  } else {
    Write(message);
  }
}

void Console::Write(au::ErrorManager& error, const std::string& prefix_message) {
  au::ErrorManager tmp_error;

  tmp_error.Add(error, prefix_message);
  Write(tmp_error);
}

void Console::Write(au::ErrorManager& error) {
  const au::vector<ErrorManagerItem>& items = error.items();

  for (size_t i = 0; i < items.size(); ++i) {
    ErrorManagerItem *item = items[i];

    switch (item->type()) {
      case au::message:
        Write(item->message());
        break;

      case au::warning:
        WriteWarningOnConsole(item->message());
        break;
      case au::error:
        WriteErrorOnConsole(item->message());
        break;
    }
  }
}

/* Methods to write things on screen */
void Console::WriteWarningOnConsole(const std::string& message) {
  if (colors_) {
    Write(BoldMagenta, message);
  } else {
    Write("[WARNING] " + message);
  }
}

void Console::WriteErrorOnConsole(const std::string& message) {
  if (colors_) {
    Write(BoldRed, message);
  } else {
    Write("[ERROR] " + message);
  }
}

void Console::WriteBoldOnConsole(const std::string& message) {
  if (colors_) {
    Write(BoldYellow, message);
  } else {
    Write(">>>> " + message);
  }
}

int Console::WaitWithMessage(const std::string& message, double sleep_time, ConsoleEntry *entry) {
  // Clear line
  ClearTerminalLine();

  // Print message
  printf("%s", message.c_str());
  fflush(stdout);

  size_t total_slept = 0;
  size_t total_to_sleep = sleep_time * 1000000;

  while (total_slept < total_to_sleep) {
    if (IsInputReady()) {
      GetEntry(entry);
      return 0;
    }

    // Message in background
    ProcessBackgroundMessages();
    usleep(20000);
    total_slept += 20000;
  }

  return 1;   // Timeout
}

void Console::Write(const std::string& message) {
  if (IsBackgroundThread()) {
    // Page control is applied if necessary
    ClearTerminalLine();

    // Divide input message in lines
    std::vector<std::string> lines = au::split(message, '\n');

    // Get terminal size
    int x, y;
    au::GetTerminalSize(1, &x, &y);
    size_t console_width = x;

    int pos = 0;   // Line to print
    size_t num_lines_on_screen = y - 3;
    bool continue_flag = false;

    while (true) {
      // Print all of them and return
      ClearTerminalLine();
      size_t max_pos = pos + num_lines_on_screen;
      for (size_t i = pos; i < std::min(lines.size(), max_pos); ++i) {
        std::string line_to_print;
        if (lines[i].length() <= console_width) {
          line_to_print = lines[i];
        } else {
          line_to_print = lines[i].substr(0, x - 3) + "...";
        }
        printf("%s\n", line_to_print.c_str());
      }

      if (pos + num_lines_on_screen >= lines.size()) {
        return;
      }

      if (!continue_flag) {
        double p1 = (double)(pos) / (double)lines.size();
        double p2 = (double)(pos + num_lines_on_screen) / (double)lines.size();

        printf("Lines %d-%d / %d [ %s ] [ space: next page c: continue q: quit ]"
               , static_cast<int>(pos + 1)
               , static_cast<int>(pos + num_lines_on_screen)
               , static_cast<int>(lines.size())
               , au::str_double_progress_bar(p1, p2, '.', '*', '.', 15).c_str());
        fflush(stdout);

        ConsoleEntry entry;
        GetEntry(&entry);

        if (entry.isChar('c')) {
          continue_flag = true;
          pos += num_lines_on_screen;
        } else if (entry.isChar('q')) {
          // Stop printing lines..
          break;
        } else if (entry.isCursorDown()) {
          pos++;
        } else if (entry.isCursorUp()) {
          pos--;
          if (pos < 0) {
            pos = 0;
          }
        } else {
          // If continue, just skip to the next block of lines
          pos += num_lines_on_screen;
        }
      } else {
        // If continue, just skip to the next block of lines
        pos += num_lines_on_screen;
      }
    }

    PrintCommand();
    fflush(stdout);
  } else {
    // Accumulate messages in background thread
    au::TokenTaker tt(&token_pending_messages_);
    pending_messages_.push_back(message);
  }
}

std::string Console::GetPrompt() {
  return "> ";
}

void Console::EvalCommand(const std::string& command) {
  WriteWarningOnConsole("Console::evalCommand not implemented");

  // Simple quit function...
  if (command == "quit") {
    StopConsole();
  }
}

void Console::AutoComplete(ConsoleAutoComplete *info) {
  WriteWarningOnConsole(au::str("Console::auto_complete not implemented (%p)", info));
}

void Console::Refresh() {
  PrintCommand();
}

void Console::AppendToCommand(const std::string& txt) {
  command_history_->GetCurrentCommand()->AddString(txt);
  PrintCommand();
}

std::string Console::str_history(int limit) {
  return command_history_->str_history(limit);
}

void handle_winch(int sig) {
  if (!current_console || sig == 0x12345678) {
    return;
  }

  // Rewrite current command
  current_console->Refresh();
}

//  SAMSON-1114 - Handle SIGTSTP so we can re-init the console allowing the
//  cursor keys to work
void handle_tstp(int sig) {
  sigset_t mask;

  if (sig == SIGTSTP) {
    // Unblock SIGSTSTP
    sigemptyset(&mask);
    sigaddset(&mask, SIGTSTP);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);

    // Reset the signal handler to the default
    signal(SIGTSTP, SIG_DFL);

    // And send ourself the signal so we get suspended as the user intended
    kill(getpid(), SIGTSTP);

    /*
     * In suspension .....
     */

    // When we come back reset the handler to our handler function
    signal(SIGTSTP, handle_tstp);  /* reestablish signal handler */

    // Re init console
    init_console_mode();
  }
}

void Console::StopReverseSearchMode() {
  reverse_search_mode_ = false;
  command_history_->MoveToLastcommand();

  std::string new_command;
  if (reverse_search_history_pos_ == -1) {
    new_command = reverse_search_command_->command();
  } else {
    new_command = command_history_->GetStringCommand(reverse_search_history_pos_);
  }
  command_history_->GetCurrentCommand()->SetCommand(new_command);
}

void Console::StartReverseSearchMode() {
  // Take current string as base for search
  reverse_search_mode_ = true;
  std::string txt = command_history_->GetCurrentCommand()->command();
  reverse_search_command_->SetCommand(txt);
  reverse_search_history_pos_ = -1;
  UpdateReverseSearchMode();
}

void Console::UpdateReverseSearchMode() {
  if (!reverse_search_mode_) {
    return;
  }

  std::string message = reverse_search_command_->command();
  if (message == "") {
    reverse_search_history_pos_ = -1;
    return;
  }
  reverse_search_history_pos_ = command_history_->Find(reverse_search_history_pos_, message);
  if (reverse_search_history_pos_ == -1) {
    reverse_search_history_pos_ = command_history_->Find(reverse_search_history_pos_, message);
  }
}

void Console::NextReverseSearch() {
  int previous_pos = reverse_search_history_pos_;
  std::string message = reverse_search_command_->command();

  if (previous_pos == -1) {
    reverse_search_history_pos_ = command_history_->Find(reverse_search_history_pos_, message);
    return;
  }

  int next_pos = command_history_->Find(previous_pos - 1, message);
  if (next_pos == -1) {
    return;   // No more search
  }
  // Keep this as the next por
  reverse_search_history_pos_ = next_pos;
}
}
}
