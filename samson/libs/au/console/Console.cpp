#include "au/Cronometer.h"
#include "au/ThreadManager.h"
#include "au/containers/list.h"
#include "au/string.h"
#include "au/utils.h"
#include <iostream>
#include <signal.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include "au/mutex/TokenTaker.h"

#include "au/console/ConsoleAutoComplete.h"
#include "au/console/ConsoleCode.h"
#include "au/console/ConsoleCommand.h"
#include "au/console/ConsoleCommandHistory.h"
#include "au/console/ConsoleEscapeSequence.h"


#include "au/console/Console.h"  // Own interface

struct termios old_tio, new_tio;

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
Console *current_console = NULL;
void handle_winch(int sig);
void handle_tstp(int sig);

Console::Console() : token_pending_messages("token_pending_messages") {
  command_history = new ConsoleCommandHistory();
  counter = 0;

  // By default, background messages are not blocked ( esc - b )
  block_background_messages = false;
}

Console::~Console() {
  delete command_history;
}

void Console::print_command() {
  std::string _command = command_history->current()->getCommand();
  int _pos = command_history->current()->getPos();

  clear_line();
  // printf("[%d]",counter); // debuggin

  printf("%s %s", getPrompt().c_str(), _command.c_str());

  if (_pos != (int)_command.length()) {
    printf("\r");

    // printf("[%d]",counter); // debuggin

    printf("%s ",  getPrompt().c_str());
    for (int i = 0; i < _pos; i++) {
      printf("%c", _command[i]);
    }
  }

  counter++;

  fflush(stdout);
  // At the moment we are not considering pos_command
}

bool Console::isImputReady() {
  struct timeval timeVal;

  timeVal.tv_sec  = 0;
  timeVal.tv_usec = 0;

  fd_set rFds;
  FD_ZERO(&rFds);
  FD_SET(0, &rFds);

  int s = select(1, &rFds, NULL, NULL, &timeVal);

  return (s == 1);
}

void Console::process_auto_complete(ConsoleAutoComplete *info) {
  // Do something with autocomplete information provided by thrid party

  std::string help_message = info->getHelpMessage();

  if (( help_message.length() > 0) || info->necessary_print_last_words_alternatives()) {
    print_command();
    printf("\n");
  }

  if (info->necessary_print_last_words_alternatives())
    info->print_last_words_alternatives(); if (help_message.length() > 0) {
    printf("Help: %s\n", help_message.c_str());  // Add necessary stuff...
  }
  command_history->current()->add(info->stringToAppend());
  print_command();
}

void Console::process_char(char c) {
  command_history->current()->add(c);

  if (command_history->current()->isCursorAtEnd()) {
    printf("%c", c);
    fflush(stdout);
  } else {
    print_command();
  }
}

void Console::internal_command(std::string command) {
  if (command == "tab") {
    ConsoleAutoComplete *info = new ConsoleAutoComplete(command_history->current()->getCommandUntilPointer());
    autoComplete(info);
    process_auto_complete(info);
    delete info;
    return;
  }

  if (command == "return") {
    std::string _command = command_history->current()->getCommand();

    // Print the command on screen...
    print_command();
    printf("\n");

    // Eval the command....
    evalCommand(_command);

    // New command in history
    command_history->new_command();
    print_command();
  }

  if (command == "move_home") {
    command_history->current()->move_home();
    print_command();
  }

  if (command == "move_end") {
    command_history->current()->move_end();
    print_command();
  }

  if (command == "del_rest_line") {
    command_history->current()->delete_rest_line();
    print_command();
  }

  if (command == "del") {
    command_history->current()->delete_char();
    print_command();
  }

  if (command == "delete_word") {
    command_history->current()->delete_word();
    print_command();
  }

  if (command == "toogle") {
    command_history->current()->toogle();
    print_command();
  }
}

void Console::addEspaceSequence(std::string sequence) {
  escape_sequence.addSequence(sequence);
}

void Console::internal_process_escape_sequence(std::string sequence) {
  if (sequence == au::str("b")) {
    // It is theoretically impossible, but just in case
    if (pthread_self() != t_running)
      return;

    block_background_messages = !block_background_messages;

    if (block_background_messages)
      writeWarningOnConsole("Background messages blocked ( press esc b to show again )\n"); else
      writeWarningOnConsole("Background messages are shown again\n"); clear_line();
    flush();
    print_command();

    return;
  }

  if (sequence == au::str("%c", 127)) {
    // Remove a word
    command_history->current()->delete_word();
    print_command();
    return;
  }

  if (sequence == "[A") {
    // Mode up
    command_history->move_up();
    print_command();
    return;
  }
  if (sequence == "[B") {
    // Move down
    command_history->move_down();
    print_command();
    return;
  }
  if (sequence == "[C") {
    // Move forward
    command_history->current()->move_cursor(1);
    print_command();
    return;
  }
  if (sequence == "[D") {
    // Move backward
    command_history->current()->move_cursor(-1);
    print_command();
    return;
  }

  if (sequence == "au") {
    command_history->current()->add("Andreu Urruela (andreu@tid.es,andreu@urruela.com)");
    print_command();
    return;
  }

  if (sequence == "h") {
    clear_line();

    // Print history
    printf("---------------------------------------------\n");
    printf(" HISTORY\n");
    printf("---------------------------------------------\n");
    printf("%s", command_history->str_history(10).c_str());
    printf("---------------------------------------------\n");

    // Print command again
    print_command();
    return;
  }

  process_escape_sequence(sequence);
}

void Console::process_background() {
  if (block_background_messages)
    return;

  if (pending_messages.size() != 0) {
    au::TokenTaker tt(&token_pending_messages);

    clear_line();

    while (pending_messages.size() != 0) {
      std::string txt = pending_messages.front();
      pending_messages.pop_front();
      printf("%s",  txt.c_str());
    }

    print_command();
    fflush(stdout);
  }
}

void Console::flush() {
  process_background();
}

bool Console::isNormalChar(char c) {
  if (( c >= 32) && ( c <= 126))
    return true; return false;
}

std::string getSequenceDescription(std::string sequece) {
  std::ostringstream output;

  output << "( " << sequece << " ";
  for (size_t i = 0; i < sequece.length(); i++) {
    output << au::str("[%d]", (int)sequece[i]);
  }
  output << " )";
  return output.str();
}

void *run_console(void *p) {
  Console *console = (Console *)p;

  console->runConsole();
  return NULL;
}

void Console::runConsoleInBackground() {
  pthread_t t;

  au::ThreadManager::shared()->addThread("au::Console::runConsoleInBackground", &t, NULL, run_console, this);
}

void Console::getEntry(ConsoleEntry *entry) {
  char c;

  if (read(0, &c, 1) != 1)
    LM_X(1, ("reading from stdin failed")); if (c == 27) {
    // Escape sequence
    escape_sequence.init();

    while (true) {
      if (read(0, &c, 1) != 1) {
        LM_X(1, ("reading from stdin failed"));  // Add the new character
      }
      escape_sequence.add(c);

      // Detect if it is a valid sequence code
      SequenceDetectionCode sequence_detection_code = escape_sequence.checkSequence();

      if (sequence_detection_code == sequence_finished) {
        entry->setEscapeSequence(escape_sequence.getCurrentSequence());
        return;
      }

      if (sequence_detection_code == sequence_non_compatible) {
        entry->setUnknownEscapeSequence(escape_sequence.getCurrentSequence());
        return;
      }

      // Continue reading escape sequence...
    }
  } else {
    entry->setChar(c);
  }
}

void Console::runConsole() {
  // Init console
  init_console_mode();

  t_running = pthread_self();

  // Signal to handle terminal changes...
  current_console = this;
  signal(SIGWINCH, handle_winch);
  signal(SIGTSTP, handle_tstp);

  // First version with just the promtp
  print_command();

  quit_console = false;
  while (!quit_console) {
    while (!isImputReady()) {
      // Review background messages
      process_background();
      usleep(20000);
    }


    // Read an entry or a escape sequence
    ConsoleEntry entry;
    getEntry(&entry);


    if (entry.isChar()) {
      char c = entry.getChar();

      if (isNormalChar(c))
        process_char(c); else if (c == '\n')
        internal_command("return"); else if (c == '\t')
        internal_command("tab"); else if (c == 127)
        internal_command("del"); else if (c == 11)
        internal_command("del_rest_line"); else if (c == 4)
        Console::quitConsole(); else if (c == 1)
        internal_command("move_home"); else if (c == 5)
        internal_command("move_end"); else if (c == 23)    // CTRL-W
        internal_command("delete_word"); else if (c == 20) // CTRL-W
        internal_command("toogle"); else if (c == 7)       // bell
        printf("%c", c); else
        writeWarningOnConsole(au::str("Ignoring unkown char (%d)", c));
    } else if (entry.isEscapeSequence()) {
      std::string seq =  entry.getEscapeSequece();
      internal_process_escape_sequence(escape_sequence.getCurrentSequence());
    } else if (entry.isUnknownEscapeSequence()) {
      std::string seq =  entry.getEscapeSequece();
      writeWarningOnConsole(au::str("Unknown escape sequence (%s)", getSequenceDescription(seq).c_str()));
    }
  }


  // Clear line to quit nicely...
  clear_line();

  // Remove the signal handler...
  signal(SIGWINCH, SIG_IGN);
}

void Console::quitConsole() {
  command_history->save_history();
  quit_console = true;
}

/* ask about Console quit status */
/* Goyo. Trying to protect against info reports after quitting delilah (SAMSON-312) */
bool Console::isQuitting() {
  return quit_console;
}

void Console::write(au::ErrorManager *error) {
  const au::vector<ErrorMessage>& messages = error->errors();

  for (size_t i = 0; i < messages.size(); i++) {
    ErrorMessage *item = messages[i];

    switch (item->type()) {
      case ErrorMessage::item_message:
        writeOnConsole(item->GetMultiLineMessage());
        break;

      case ErrorMessage::item_warning:
        writeWarningOnConsole(item->GetMultiLineMessage());
        break;
      case ErrorMessage::item_error:
        writeErrorOnConsole(item->GetMultiLineMessage());
        break;
    }
  }
}

/* Methods to write things on screen */
void Console::writeWarningOnConsole(std::string message) {
  if (message.substr(message.length() - 1) != "\n")
    message.append("\n"); std::ostringstream output;
  output << "\033[1;35m" << message << "\033[0m";
  write(output.str());
}

void Console::writeErrorOnConsole(std::string message) {
  if (message.substr(message.length() - 1) != "\n")
    message.append("\n"); std::ostringstream output;
  output << "\033[1;31m" << message << "\033[0m";
  std::string txt = output.str();
  write(txt);
}

int Console::waitWithMessage(std::string message, double sleep_time, ConsoleEntry *entry) {
  // Clear line
  clear_line();

  // Print message
  printf("%s", message.c_str());
  fflush(stdout);

  size_t total_slept = 0;
  size_t total_to_sleep = sleep_time * 1000000;

  while (total_slept < total_to_sleep) {
    if (isImputReady()) {
      getEntry(entry);
      return 0;
    }

    // Message in background
    process_background();

    // Review background messages
    process_background();
    usleep(20000);
    total_slept += 20000;
  }

  return 1;   // Timeout
}

void Console::writeOnConsole(std::string message) {
  // Print per lines if necessary
  // --------------------------------------------------------------------------------------------

  if (pthread_self() == t_running) {
    clear_line();

    // Divide input message in lines
    std::istringstream input(message);
    char line[1024];
    input.getline(line, 1024);
    std::vector<std::string> lines;
    while (!input.eof()) {
      if (strlen(line) > 0)
        lines.push_back(line); input.getline(line, 1024);
    }

    // Get terminal size
    int x, y;
    au::get_term_size(1, &x, &y);

    int pos = 0;   // Line to print
    size_t num_lines_on_screen = y - 3;
    bool continue_flag = false;

    while (true) {
      // Print all of them and return
      clear_line();
      size_t max_pos = pos + num_lines_on_screen;
      for (size_t i = pos; i < std::min(lines.size(), max_pos); i++) {
        std::string line_to_print;
        if (lines[i].length() <= (size_t)x)
          line_to_print = lines[i]; else
          line_to_print = lines[i].substr(0, x - 3) + "..."; printf("%s\n", line_to_print.c_str());
      }

      if (pos + num_lines_on_screen >= lines.size())
        return;

      if (!continue_flag) {
        double p1 = (double)(pos) / ( double )lines.size();
        double p2 = (double)(pos + num_lines_on_screen) / ( double )lines.size();

        printf("Lines %d-%d / %d [ %s ] [ space: next page c: continue q: quit ]"
               , (int)(pos + 1)
               , (int)(pos + num_lines_on_screen)
               , (int)lines.size()
               , au::str_double_progress_bar(p1, p2, '.', '*', '.', 15).c_str()
               );
        fflush(stdout);

        ConsoleEntry entry;
        getEntry(&entry);

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
          if (pos < 0)
            pos = 0;
        } else {
          // If continue, just skip to the next block of lines
          pos += num_lines_on_screen;
        }
      } else {
        // If continue, just skip to the next block of lines
        pos += num_lines_on_screen;
      }
    }

    print_command();
    fflush(stdout);
    return;
  }

  // --------------------------------------------------------------------------------------------

  write(message);
}

void Console::write(std::string message) {
  if (pthread_self() != t_running) {
    // Accumulate message
    au::TokenTaker tt(&token_pending_messages);
    pending_messages.push_back(message);
    return;
  }

  clear_line();
  printf("%s", message.c_str());

  print_command();
  fflush(stdout);
}

std::string Console::getPrompt() {
  return "> ";
}

void Console::evalCommand(std::string command) {
  writeWarningOnConsole("Console::evalCommand not implemented");

  // Simple quit function...
  if (command == "quit")
    quit_console = true;
}

void Console::autoComplete(ConsoleAutoComplete *info) {
  writeWarningOnConsole(au::str("Console::auto_complete not implemented (%p)", info));
}

void Console::refresh() {
  print_command();
}

void Console::appendToCommand(std::string txt) {
  command_history->current()->add(txt);
  print_command();
}

std::string Console::str_history(int limit) {
  return command_history->str_history(limit);
}

void handle_winch(int sig) {
  if (!current_console || sig == 0x12345678)
    return;

  // Rewrite current command
  current_console->refresh();
}

//  SAMSON-1114 - Handle SIGTSTP so we can re-init the console allowing the
//  cursor keys to work
void handle_tstp(int sig) {
  sigset_t mask;

  if (sig == SIGTSTP) {
    // Unblock SIGSTSTP
    sigemptyset(&mask); sigaddset(&mask, SIGTSTP); sigprocmask(SIG_UNBLOCK, &mask, NULL);

    // Reset the signal handler to the default
    signal(SIGTSTP, SIG_DFL);

    // And send ourself the signal so we get suspended as the user intended
    kill(getpid(), SIGTSTP);

    /*
     * In suspension .....
     */

    // When we come back reset the handler to our handler function
    signal(SIGTSTP, handle_tstp);   /* reestablish signal handler */

    // Re init console
    init_console_mode();
  }
}

void Console::process_log(au::SharedPointer<Log>log) {
  // Print a warning on screen

  std::string type = log->Get("type");

  if (( type == "E" ) || ( type == "X" ))
    writeErrorOnConsole("TRACE: " + log_formatter.get(log) + "\n"); else if (type == "W")
    writeWarningOnConsole("TRACE: " + log_formatter.get(log) + "\n"); else
    writeOnConsole("TRACE: " + log_formatter.get(log) + "\n");
}
}
