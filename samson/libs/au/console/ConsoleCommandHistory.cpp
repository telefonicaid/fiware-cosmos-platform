
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "au/console/ConsoleCommand.h"

#include "au/console/ConsoleCommandHistory.h"  // Own interface


namespace au {
ConsoleCommandHistory::ConsoleCommandHistory() {
  // Default name
  file_name = getenv("HOME");
  file_name.append("/.history_console");

  // Recover history
  recover_history();

  commands.push_back(new ConsoleCommand());
  pos = commands.size() - 1;
}

ConsoleCommandHistory::~ConsoleCommandHistory() {
  save_history();

  while (1) {
    if (commands.size() == 0)
      break; delete commands[commands.size() - 1];
    commands.pop_back();
  }
}

ConsoleCommand *ConsoleCommandHistory::current() {
  return commands[pos];
}

void ConsoleCommandHistory::recover_history() {
  FILE *file = fopen(file_name.c_str(), "r");

  if (!file)
    return;

  char line[1024];
  while (fgets(line, 1024, file)) {
    // Remove the \n char at the end of each line
    line[ strlen(line) - 1 ] = '\0';

    commands.push_back(new ConsoleCommand(line));
  }

  fclose(file);
}

void ConsoleCommandHistory::save_history() {
  // Remove the last one if empty...
  if (commands[ commands.size() - 1 ]->getCommand() == "") {
    delete commands[ commands.size() - 1 ];
    commands.pop_back();

    if (pos > ( commands.size() - 1 ))
      pos = commands.size() - 1;
  }

  FILE *file = fopen(file_name.c_str(), "w");

  if (!file)
    return;

  for (size_t i = 0; i < commands.size(); i++) {
    fprintf(file, "%s\n", commands[i]->getCommand().c_str());
  }

  fclose(file);
}

void ConsoleCommandHistory::move_up() {
  if (pos > 0)
    pos--;
}

void ConsoleCommandHistory::move_down() {
  if (pos < (commands.size() - 1))
    pos++;
}

void ConsoleCommandHistory::new_command() {
  // Copy to the new one if we are editing a previous entry
  if (pos != (commands.size() - 1)) {
    commands[commands.size() - 1]->setCommand(commands[pos]->getCommand());  // If we are repeting previous command, do not add again
  }
  if (commands.size() > 1)
    if (commands[pos]->getCommand() == commands[commands.size() - 2]->getCommand()) {
      commands[commands.size() - 1]->setCommand("");
      pos = commands.size() - 1;
      return;
    }
  if (commands[commands.size() - 1]->getCommand() != "")
    // Add a new line for the new command
    commands.push_back(new ConsoleCommand()); pos = commands.size() - 1;
}
}
