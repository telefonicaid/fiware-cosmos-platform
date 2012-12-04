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

Console::Console()
  : au::Thread("Console")
    , command_history_(new ConsoleCommandHistory())
    , token_pending_messages_("token_pending_messages")
    , block_background_messages_(false)  // By default, background messages are not blocked ( esc - b for toogle )
    , quit_console_(false)  // Internal flag to quit console ( i.e. the user has tiped command quit )
    , colors_(true) {  // By default colors are allowed
}

Console::~Console() {
  command_history_->save_history();
  delete command_history_;
}

void Console::StartConsole(bool block_thread) {
  quit_console_ = false;
  StartThread();

  if (block_thread) {
    JoinThread();  // Wait for background thread to finish
  }
}

void Console::StopConsole() {
  if (IsBackgroundThread()) {
    quit_console_ = true;   // Interal flag to make sure background thread is finished
  } else {
    StopThread();   // Stop background thread directly
  }
}

void Console::PrintCommand() {
  // Get prompt
  std::string prompt = getPrompt();
  // Get command and position inside the command
  std::string _command = command_history_->current()->getCommand();
  int _pos = command_history_->current()->getPos();

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
  printf("%s ", getPrompt().c_str());
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
    printf("%s ", getPrompt().c_str());
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
  command_history_->current()->add(info->stringToAppend());
  PrintCommand();
}

void Console::ProcessChar(char c) {
  // Add char to the current command
  command_history_->current()->add(c);

  // We always print command again to detect if command is larger than the console window
  PrintCommand();
}

void Console::ProcessInternalCommand(const std::string& command) {
  if (command == "tab") {
    ConsoleAutoComplete *info = new ConsoleAutoComplete(command_history_->current()->getCommandUntilPointer());
    autoComplete(info);
    ProcessAutoComplete(info);
    delete info;
    return;
  }

  if (command == "return") {
    std::string _command = command_history_->current()->getCommand();

    // Print the command on screen...
    PrintCommand();
    printf("\n");

    // Eval the command....
    evalCommand(_command);

    // New command in history
    command_history_->new_command();
    PrintCommand();
  }

  if (command == "move_home") {
    command_history_->current()->move_home();
    PrintCommand();
  }

  if (command == "move_end") {
    command_history_->current()->move_end();
    PrintCommand();
  }

  if (command == "del_rest_line") {
    command_history_->current()->delete_rest_line();
    PrintCommand();
  }

  if (command == "del") {
    command_history_->current()->delete_char();
    PrintCommand();
  }

  if (command == "delete_word") {
    command_history_->current()->delete_word();
    PrintCommand();
  }

  if (command == "toogle") {
    command_history_->current()->toogle();
    PrintCommand();
  }
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
    command_history_->current()->delete_word();
    PrintCommand();
    return;
  }

  if (sequence == "[A") {
    // Mode up
    command_history_->move_up();
    PrintCommand();
    return;
  }
  if (sequence == "[B") {
    // Move down
    command_history_->move_down();
    PrintCommand();
    return;
  }
  if (sequence == "[C") {
    // Move forward
    command_history_->current()->move_cursor(1);
    PrintCommand();
    return;
  }
  if (sequence == "[D") {
    // Move backward
    command_history_->current()->move_cursor(-1);
    PrintCommand();
    return;
  }

  if (sequence == "au") {
    command_history_->current()->add("Andreu Urruela (andreu@tid.es,andreu@urruela.com)");
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

  process_escape_sequence(sequence);
}

void Console::PrintLines(const std::string&message) {
  // Divide input message in lines
  std::vector<std::string> lines = au::split(message, '\n');

  // Get terminal size
  int x, y;
  au::GetTerminalSize(1, &x, &y);
  std::string line_to_print;
  for (size_t i = 0; i < lines.size(); ++i) {
    if (lines[i].length() <= (size_t)x) {
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

bool Console::IsNormalChar(char c) const {
  if ((c >= 32) && (c <= 126)) {
    return true;
  }
  return false;
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
    if (quit_console_) {
      return;
    }

    while (!IsInputReady()) {
      // Review background messages
      ProcessBackgroundMessages();
      usleep(20000);
    }

    // Read an entry or a escape sequence
    ConsoleEntry entry;
    GetEntry(&entry);

    if (entry.isChar()) {
      char c = entry.getChar();

      if (IsNormalChar(c)) {
        ProcessChar(c);
      } else {
        switch (c) {
          case '\n':
            ProcessInternalCommand("return");
            break;
          case '\t':
            ProcessInternalCommand("tab");
            break;
          case 127:
            ProcessInternalCommand("del");
            break;
          case 11:
            ProcessInternalCommand("del_rest_line");
            break;
          case 1:
            ProcessInternalCommand("move_home");
            break;
          case 5:
            ProcessInternalCommand("move_end");
            break;
          case 23:
            ProcessInternalCommand("delete_word");
            break;
          case 20:
            ProcessInternalCommand("toogle");
            break;
          case 7:
            printf("%c", c);  // bell
            break;

          default:
            WriteWarningOnConsole(au::str("Ignoring unkown char (%d)", c));
            break;
        }
      }
    } else if (entry.isEscapeSequence()) {
      std::string seq = entry.getEscapeSequece();
      ProcessEscapeSequenceInternal(escape_sequence_.getCurrentSequence());
    } else if (entry.isUnknownEscapeSequence()) {
      std::string seq = entry.getEscapeSequece();
      WriteWarningOnConsole(au::str("Unknown escape sequence (%s)", GetEscapeSequenceDescription(seq).c_str()));
    }
  }

  // Clear line to quit nicely...
  ClearTerminalLine();

  // Remove the signal handler...
  signal(SIGWINCH, SIG_IGN);
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
    // Page control is applyed if necessary
    ClearTerminalLine();

    // Divide input message in lines
    std::vector<std::string> lines = au::split(message, '\n');

    // Get terminal size
    int x, y;
    au::GetTerminalSize(1, &x, &y);

    int pos = 0;   // Line to print
    size_t num_lines_on_screen = y - 3;
    bool continue_flag = false;

    while (true) {
      // Print all of them and return
      ClearTerminalLine();
      size_t max_pos = pos + num_lines_on_screen;
      for (size_t i = pos; i < std::min(lines.size(), max_pos); ++i) {
        std::string line_to_print;
        if (lines[i].length() <= (size_t)x) {
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

std::string Console::getPrompt() {
  return "> ";
}

void Console::evalCommand(const std::string& command) {
  WriteWarningOnConsole("Console::evalCommand not implemented");

  // Simple quit function...
  if (command == "quit") {
    StopConsole();
  }
}

void Console::autoComplete(ConsoleAutoComplete *info) {
  WriteWarningOnConsole(au::str("Console::auto_complete not implemented (%p)", info));
}

void Console::Refresh() {
  PrintCommand();
}

void Console::AppendToCommand(const std::string& txt) {
  command_history_->current()->add(txt);
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
}
}
