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
/**
 * \brief Enum to describe types of arguments used in CommandCatalogue
 */
typedef enum {
  option_bool,
  option_int,
  option_uint64,
  option_double,
  option_string,
} Type;
}

/**
 * \brief Item inside a Command ( name, value and type of any of the arguments/options of provided command )
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

  void AutoComplete(au::console::ConsoleAutoComplete *info);
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

  std::string name() const;
  std::string category() const;
  std::string short_description() const;
  std::string help() const;
  std::string usage() const;

  // Add argument or options
  void AddArgument(CommandItem *item);
  void AddOption(CommandItem *item);

  // methods for tags
  void SetTag(const std::string& tag) {
    tags_.insert(tag);
  }

  bool HasTag(const std::string& tag) {
    return (tags_.find(tag) != tags_.end());
  }

  /**
   * \brief Fill "info" with all possible auto-completion elements based on this command
   */
  void AutoComplete(au::console::ConsoleAutoComplete *info);

  CommandItem *GetOption(const std::string& name) const;
  CommandItem *GetArgument(const std::string& name) const;

  const std::vector<CommandItem *>& options();
  const std::vector<CommandItem *>& arguments();

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
 * \brief Instance of a command generated from a string usually entered by the user
 *
 * Once a string is provided ( i.e. in a console ), an instance of this class is obtained from a CommandCatalogue
 */

class CommandInstance {
public:

  CommandInstance(Command *command, const std::string& command_line);

  const std::string main_command();
  const std::string command_line();

  /**
   * \brief Get the original command ( definition of options and arguments )
   */
  Command *command() const;

  /**
   * \brief Check if a string argument has been provided in the command line
   */
  bool HasStringArgument(const std::string& name) const;

  /**
   * \brief Gets the value of a string argument provided in the comamnd line
   */
  std::string GetStringArgument(const std::string& name) const;

  /**
   * \brief Gets the value of a bool-option provided in the comamnd line
   */
  bool GetBoolOption(const std::string& name) const;

  /**
   * \brief Gets the value of a int-option provided in the comamnd line
   */
  int GetIntOption(const std::string& name) const;

  /**
   * \brief Gets the value of a uint64-option provided in the comamnd line
   */
  size_t GetUint64Option(const std::string& name) const;

  /**
   * \brief Gets the value of a double-option provided in the comamnd line
   */
  double GetDoubleOption(const std::string& name) const;

  /**
   * \brief Gets the value of a string-option provided in the comamnd line
   */
  std::string GetStringOption(const std::string& name) const;

  /**
   * \brief Check if a particular option/argument has value
   */
  bool hasValueFor(const std::string& name) const;

  /**
   * \brief Get a complete error message with extra information about provided command
   */
  std::string GetErrorMessage(const std::string error_message) const;

private:

  friend class CommandCatalogue;

  /**
   * \brief Set value for a particular option/argument
   */
  void SetValue(const std::string& name, const std::string& value);


  au::ErrorManager error_;                           // Error during parse operation
  Command *command_;                                 // Duplicate command definition
  std::string command_line_;                         // Copy of the original command line
  au::simple_map<std::string, std::string> values_;  // Values assigned to each item
};

/**
 * \brief Complete catalogue of possible commands with all their options and arguments
 *
 * Very useful used toguether with au::Console since commands introduced by user can be verified and parsed here
 */

class CommandCatalogue {
public:

  CommandCatalogue();
  ~CommandCatalogue() {
    commands_.clearVector();
  }

  /**
   * \brief Parse a provided command line based on the commands defined in this catalogue
   * In case of error, NULL is returned.
   * Otherwise, returned instance contains all values for arguments and options of the selected command
   */
  CommandInstance *Parse(const std::string command_line, au::ErrorManager& error) const;

/**
 * \brief Add a new command to the catalogue
 */
  Command *AddCommand(const std::string& name
                      , const std::string& category = "general"
                      , const std::string& short_description = ""
                      , const std::string& help = "");

/**
 * \brief Add option to previously added command
 */
  CommandItem *AddOption(const std::string& command_name
                         , const std::string& name
                         , options::Type type
                         , bool optional =  true
                         , const std::string& help = ""
                         , const std::string& default_value = ""
                         , const std::string& min_value = ""
                         , const std::string& max_value = "");

  /**
   * \brief Add argument to previously added command
   */
  CommandItem *AddArgument(const std::string& command_name
                           , const std::string& name
                           , options::Type type
                           , bool optional =  true
                           , const std::string& help = ""
                           , const std::string& default_value = ""
                           , const std::string& min_value = ""
                           , const std::string& max_value = "");

  /**
   * \brief Set a tag for a previously added command
   */
  void AddTag(const std::string& command_name, const std::string& tag);

  /**
   * \brief Set a int-option for a previously added command
   */
  CommandItem *AddIntOption(const std::string& command_name
                            , const std::string& name
                            , int default_value
                            , const std::string& help = "");


  /**
   * \brief Set a uint64-option for a previously added command
   */
  CommandItem *AddUInt64Option(const std::string& command_name
                               , const std::string& name
                               , size_t default_value
                               , const std::string& help = "");

  /**
   * \brief Set a string-option for a previously added command
   */
  CommandItem *AddStringOption(const std::string& command_name
                               , const std::string& name
                               , const std::string& default_value
                               , const std::string& help = "");

  /**
   * \brief Set a bool-option for a previously added command
   */
  CommandItem *AddBoolOption(const std::string& command_name
                             , const std::string& name
                             , const std::string& help = "");


  /**
   * \brief Set a string-argument for a previously added command
   */
  CommandItem *AddStringArgument(const std::string& command_name
                                 , const std::string& name
                                 , const std::string& default_value
                                 , const std::string& help);

  CommandItem *AddMandatoryStringArgument(const std::string& command_name
                                          , const std::string& name
                                          , const std::string& help);

  CommandItem *AddMandatoryUInt64Argument(const std::string& command_name
                                          , const std::string& name
                                          , const std::string& help);


  CommandItem *AddStringOptionsArgument(const std::string& command_name
                                        , const std::string& name
                                        , const std::string& group_value
                                        , const std::string& help = "");

  /**
   * \brief Full "info" with all auto-completion options based on command included in this catalogue
   */
  void AutoComplete(au::console::ConsoleAutoComplete *info);

  /**
   * \brief Get a table with available command ( optional filter per category )
   */
  std::string GetCommandsTable(const std::string& category = "") const;

  /**
   * \brief Get help message for an individual command
   */
  std::string GetHelpForConcept(const std::string& name) const;

  /**
   * \brief Get list of categories to be displayed in help message
   */
  au::StringVector GetCategories() const;

  /**
   * \brief Check if a command is defined
   */
  bool IsValidCommand(const std::string& command) const;

  /**
   * \brief Check if a category is included in this command catalogue
   */
  bool IsValidCategory(const std::string& category) const;

private:

  // Vector of available commands
  au::vector<Command> commands_;

  // Get a particular command ( supouselly added beforehand )
  Command *GetCommand(const std::string& name) const;
};
}
}

#endif  // ifndef _H_AU_COMMAND_CATALOGUE
