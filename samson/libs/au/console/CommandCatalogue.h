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
#ifndef _H_AU_COMMAND_CATALOGUE
#define _H_AU_COMMAND_CATALOGUE

#include <algorithm>
#include <cstdlib>
#include <errno.h>
#include <fcntl.h>
#include <sstream>
#include <stdio.h>
#include <time.h>

#include "logMsg/logMsg.h"

#include "au/console/Console.h"
#include "au/console/ConsoleAutoComplete.h"
#include "au/mutex/TokenTaker.h"
#include "au/string/Tokenizer.h"

namespace au {
/**
 * \brief namespace for au::console library: Generic full-featured console
 */
namespace console {
/**
 * \brief Namespace to protect scope of enum inside au::console library
 */
namespace options {
// Class used to store a valid command in delilah console
typedef enum {
  option_bool,
  option_int,
  option_uint64,
  option_double,
  option_string,
} Type;
}

/**
 * \brief Item inside a CommandInstance ( name, value and type of any of the arguments/options of provided command )
 */

class CommandItem {
public:

  CommandItem(const std::string& name
              , options::Type type
              , bool optional =  true
              , const std::string& help = ""
              , const std::string& default_value = ""
              , const std::string& min_value = ""
              , const std::string& max_value = "");

  CommandItem(const CommandItem& command_item);

  const std::string& name();
  options::Type type();
  bool optional();
  const std::string& help();
  const std::string& default_value();

  void set_options_group(const std::string& options_group);
  void review_options_group();

  void autoComplete(au::console::ConsoleAutoComplete *info);
  bool HasValidValue(const std::string& value);  // Check valid valie

  std::string str_help();
  std::string str_usage_option();
  std::string str_usage_argument();

  const char *str_type();

private:

  std::string name_;                             // Name of the option -l , -name
  options::Type type_;                           // Type of value accepting this element
  bool optional_;                                // Flag to indicate that this parameter is optional
  std::string help_;                             // Help for this parameter

  std::string min_value_;                        // Min value ( if any )
  std::string max_value_;                        // Max value ( if any )
  std::string default_value_;                    // Default parameter

  std::string options_group_;                    // Options group to get possible values ( autocomplete )
  std::vector<std::string> options_group_values;  // Possible values ( from options_group_ )
};

/**
 * \brief Class to define a possible command with all its arguments and options
 */

class Command {
public:

  Command(
    const std::string& name
    , const std::string& category = "general"
    , const std::string& short_description = ""
    , const std::string& help = ""
    );

  Command(const Command& command);

  ~Command();

  const std::string& name();
  const std::string& category();
  const std::string& short_description();
  const std::string& help();

  std::string usage();

  // Add argument or options
  void add_argument(CommandItem *item);
  void add_option(CommandItem *item);

  // methods for tags
  void set_tag(const std::string& tag) {
    tags_.insert(tag);
  }

  bool tag(const std::string& tag) {
    return ( tags_.find(tag) != tags_.end());
  }

  void autoComplete(au::console::ConsoleAutoComplete *info);

  const std::vector<CommandItem *>& options();
  const std::vector<CommandItem *>& arguments();

  CommandItem *get_option(const std::string& name);
  CommandItem *get_argument(const std::string& name);

private:

  std::string name_;                                // Command itself ( first word in the line )
  std::string category_;                            // Category for this command

  std::string short_description_;                   // One line description of the command
  std::string help_;                                // Full help for this command

  std::vector<CommandItem *> options_;              // List of options for this command
  std::vector<CommandItem *> arguments_;            // List of arguments for this command

  std::set<std::string> tags_;                      // Generic tags for clasifying commands
};

/**
 * \brief Instance of a command.
 * Once a string is provided ( i.e. in a console ), and instance of this class is obtained from a CommandCatalogue
 */

class CommandInstance {
public:

  CommandInstance(Command *command, const std::string& command_line);

  const std::string main_command();
  const std::string command_line();
  Command *command();  // Get the original command ( definition of options and arguments )

  // Handy function to check content
  bool has_string_argument(const std::string& name);
  bool get_bool_option(const std::string& name);
  int get_int_option(const std::string& name);
  size_t get_uint64_option(const std::string& name);
  double get_double_option(const std::string& name);
  std::string get_string_option(const std::string& name);
  std::string get_string_argument(const std::string& name);
  // Set and get values
  void set_value(const std::string& name, const std::string& value);
  bool hasValueFor(const std::string& name);
  std::string ErrorMessage(const std::string error_message);

private:
  au::ErrorManager error_;                           // Error during parse operation
  Command *command_;                                 // Duplicate command definition
  std::string command_line_;                         // Copy of the original command line
  au::simple_map<std::string, std::string> values_;  // Values assigned to each item
};

// Catalogue of commands

class CommandCatalogue {
public:

  // Vector of available commands
  au::vector<Command> commands_;

  // Construtor
  CommandCatalogue();

  ~CommandCatalogue() {
    commands_.clearVector();
  }

  // Add elements adn extra description
  Command *add(const std::string& name
               , const std::string& category = "general"
               , const std::string& short_description = ""
               , const std::string& help = "");

  // Add option to previously added command
  CommandItem *add_option(const std::string& command_name
                          , const std::string& name
                          , options::Type type
                          , bool optional =  true
                          , const std::string& help = ""
                          , const std::string& default_value = ""
                          , const std::string& min_value = ""
                          , const std::string& max_value = "");

  // Add argument to previously added command
  CommandItem *add_argument(const std::string& command_name
                            , const std::string& name
                            , options::Type type
                            , bool optional =  true
                            , const std::string& help = ""
                            , const std::string& default_value = ""
                            , const std::string& min_value = ""
                            , const std::string& max_value = "");

  // Handy methods to add tags
  void add_tag(const std::string& command_name, const std::string& tag);

  // Handy function to add common options
  CommandItem *add_int_option(const std::string& command_name
                              , const std::string& name
                              , int default_value
                              , const std::string& help = "");

  CommandItem *add_uint64_option(const std::string& command_name
                                 , const std::string& name
                                 , size_t default_value
                                 , const std::string& help = "");

  CommandItem *add_string_option(const std::string& command_name
                                 , const std::string& name
                                 , const std::string& default_value
                                 , const std::string& help = "");

  CommandItem *add_bool_option(const std::string& command_name
                               , const std::string& name
                               , const std::string& help = "");



  // Handy function to add common arguments
  CommandItem *add_string_argument(const std::string& command_name
                                   , const std::string& name
                                   , const std::string& default_value
                                   , const std::string& help);

  CommandItem *add_mandatory_string_argument(const std::string& command_name
                                             , const std::string& name
                                             , const std::string& help);

  CommandItem *add_mandatory_uint64_argument(const std::string& command_name
                                             , const std::string& name
                                             , const std::string& help);


  CommandItem *add_string_options_argument(const std::string& command_name
                                           , const std::string& name
                                           , const std::string& group_value
                                           , const std::string& help = "");

  // Autocompletion functionality for this command catalogue ( see au::console::Console )
  void autoComplete(au::console::ConsoleAutoComplete *info);

  // Get a table with available command ( optional filter per category )
  std::string getCommandsTable(const std::string& category = "");

  // Get help message for an individual command
  std::string getHelpForConcept(const std::string& name);

  // Get list of cagtegories to be displayed in help message
  au::StringVector getCategories();

  // Check if a command is valid ( used in repeat command )
  bool isValidCommand(const std::string& command);

  // Check if is a category
  bool isValidCategory(const std::string& category);

  // Parse command to check validity
  CommandInstance *parse(const std::string command_line, au::ErrorManager& error);

private:

  // Get a particular command ( supouselly added beforehand )
  Command *get_command(const std::string& name);
};
}
}

#endif  // ifndef _H_AU_COMMAND_CATALOGUE
