
#include "CommandCatalogue.h"  // Own interface

namespace au {
namespace console {
//
// CommandItem
//

CommandItem::CommandItem(const std::string& name
                         , options::Type type
                         , bool optional
                         , const std::string& help
                         , const std::string& default_value
                         , const std::string& min_value
                         , const std::string& max_value) {
  type_ = type;
  name_ = name;
  optional_ = optional;
  help_ = help;
  default_value_ = default_value;
  min_value_ = min_value;
  max_value_ = max_value;

  options_group_ = "";      // No group by default
}

CommandItem::CommandItem(const CommandItem& command_item) {
  type_ = command_item.type_;
  name_ = command_item.name_;
  optional_ = command_item.optional_;
  help_ = command_item.help_;
  default_value_ = command_item.default_value_;
  min_value_ = command_item.min_value_;
  max_value_ = command_item.max_value_;
  options_group_ = command_item.options_group_;
}

const std::string& CommandItem::name() {
  return name_;
}

options::Type CommandItem::type() {
  return type_;
}

bool CommandItem::optional() {
  return optional_;
}

const std::string& CommandItem::help() {
  return help_;
}

const std::string& CommandItem::default_value() {
  return default_value_;
}

void CommandItem::set_options_group(const std::string& options_group) {
  options_group_ = options_group;
  review_options_group();
}

void CommandItem::review_options_group() {
  if (options_group_ != "") {
    std::vector<std::string> components = au::split(options_group_, ':');
    if (components.size() > 0) {
      if (components[0] == "Options") {
        options_group_values.clear();

        for (size_t i = 1; i < components.size(); i++) {
          options_group_values.push_back(components[i]);
        }
      }
    }
  }
}

std::string CommandItem::str_help() {
  std::ostringstream output;

  // Add help
  if (help_ != "") {
    output << help_ << "\n";      // Spetial case in group options
  }
  if (options_group_values.size() > 0) {
    std::ostringstream str_options;
    for (size_t i = 0; i < options_group_values.size(); i++) {
      str_options << options_group_values[i];
      if (i != (options_group_values.size() - 1 )) {
        str_options << ",";
      }
    }

    std::string str_str_options = str_options.str();

    if (str_str_options.length() < 15) {
      output << "Values: " << str_str_options << "\n";
    } else {
      output << au::str("Values: %lu options\n", options_group_values.size());
    }
  } else {
    output << "Type: " << str_type() << "\n";
  }

  if (optional_) {
    output << "Default value: " << default_value_ << " \n";
  }
  if (min_value_ != "") {
    output << "Min value: " << min_value_ << "\n";
  }
  if (max_value_ != "") {
    output << "Max value: " << max_value_ << "\n";
  }
  return output.str();
}

std::string CommandItem::str_usage_option() {
  std::ostringstream output;

  if (optional_) {
    output << "[";
  }
  switch (type_) {
    case options::option_bool:
      output << name_;
      break;

    case options::option_int:
      output << name_ + " (int)";
      break;

    case options::option_double:
      output << name_ + " (double)";
      break;

    case options::option_string:
      output << name_ + " (string)";
      break;

    case options::option_uint64:
      output << name_ + " (uint64)";
      break;
  }

  // Set of possible values
  if (( options_group_values.size() > 0 ) && ( options_group_values.size() < 5 )) {
    output << "(";
    for (size_t i = 0; i < options_group_values.size(); i++) {
      output << options_group_values[i];
      if (i != ( options_group_values.size() - 1 )) {
        output << "/";
      }
    }
    output << ")";
  }


  if (optional_) {
    output << "]";
  }
  return output.str();
}

std::string CommandItem::str_usage_argument() {
  std::ostringstream output;

  if (optional_) {
    output << "[";
  }
  output << name_;

  // Set of possible values
  if (( options_group_values.size() > 0 ) && ( options_group_values.size() < 5 )) {
    output << "(";
    for (size_t i = 0; i < options_group_values.size(); i++) {
      output << options_group_values[i];
      if (i != ( options_group_values.size() - 1 )) {
        output << "/";
      }
    }
    output << ")";
  }

  if (optional_) {
    output << "]";
  }
  return output.str();
}

void CommandItem::autoComplete(au::ConsoleAutoComplete *info) {
  /*
   * if( help_ != "" )
   * info->setHelpMessage( au::str("Seting option '%s': (%s) %s" , name_.c_str() , str_type() , help_.c_str() ) );
   */

  // If options are available
  if (options_group_values.size() > 0) {
    for (size_t i = 0; i < options_group_values.size(); i++) {
      info->add(options_group_values[i]);
    }      // Auto complete for files
  }
  if (options_group_ == "#file") {
    info->auto_complete_files("");
  }
}

const char *CommandItem::str_type() {
  switch (type_) {
    case options::option_bool:
      return "bool";

      break;

    case options::option_int:
      return "int";

      break;

    case options::option_string:
      return "string";

      break;

    case options::option_uint64:
      return "uint64";

      break;

    case options::option_double:
      return "double";

      break;
  }

  LM_X(1, ("Compiler error"));
  return "";
}

//
// Command
//

Command::Command(
  const std::string& name
  , const std::string& category
  , const std::string& short_description
  , const std::string& help
  ) {
  name_ = name;
  category_ = category;
  short_description_ = short_description;
  help_ = help;
}

Command::Command(const Command& command) {
  name_ = command.name_;
  category_ = command.category_;
  short_description_ = command.short_description_;
  help_ = command.help_;

  // Copy all options and arguments
  for (size_t i = 0; i < command.arguments_.size(); i++) {
    add_argument(new CommandItem(*command.arguments_[i]));
  }
  for (size_t i = 0; i < command.options_.size(); i++) {
    add_option(new CommandItem(*command.options_[i]));
  }

  // Copy all the tags
  tags_ = command.tags_;
}

const std::string& Command::name() {
  return name_;
}

const std::string& Command::category() {
  return category_;
}

const std::string& Command::short_description() {
  return short_description_;
}

const std::string& Command::help() {
  return help_;
}

std::string Command::usage() {
  std::ostringstream output;

  output << name_ << " ";
  for (size_t i = 0; i  < options_.size(); i++) {
    output << options_[i]->str_usage_option() << " ";
  }
  for (size_t i = 0; i  < arguments_.size(); i++) {
    output << arguments_[i]->str_usage_argument() << " ";
  }
  return output.str();
}

// Add argument or option
void Command::add_argument(CommandItem *item) {
  arguments_.push_back(item);
}

void Command::add_option(CommandItem *item) {
  options_.push_back(item);
}

void Command::autoComplete(au::ConsoleAutoComplete *info) {
  // Identify if it is completing a particular item ( argument or option )
  if (!info->completingSecondWord()) {
    // Get the last work
    std::string last_word = info->lastCompletedWord();

    // Autocomplete if we are filling an option
    if ((last_word.length() > 0) && ( last_word[0] == '-' )) {
      // Try to identify the options it is filling
      CommandItem *item = get_option(last_word);

      if (item && ( item->type() != options::option_bool )) {
        // We are completing this item
        item->autoComplete(info);
        return;
      }
    }
  }

  // Auto complete with any of the provided options
  for (size_t i = 0; i < options_.size(); i++) {
    std::string label = au::str("%-15s : %s", options_[i]->name().c_str(), options_[i]->help().c_str());
    info->add(label, options_[i]->name(), true);
  }

  // Autocomplete if we are filling an argument
  int argument_pos = 0;
  const std::vector<std::string>& previous_words = info->get_previous_words();
  for (size_t i = 1; i < previous_words.size(); i++) {
    if (( previous_words[i].length() > 0 ) && ( previous_words[i][0] == '-' )) {
      CommandItem *item = get_option(previous_words[i]);
      if (!item) {
        return;      // Not possible to autocomplete since this error is wrong.
      }
      if (item->type() == options::option_bool) {
        continue;
      } else {
        i++;      // skip the value
      }
    } else {
      argument_pos++;
    }
  }

  if (arguments_.size() <= (size_t)argument_pos) {
    return;      // Not possible to autocomplete
  }
  arguments_[argument_pos]->autoComplete(info);

  // Set help based on usage
  info->setHelpMessage("Usage: " + usage());
}

const std::vector<CommandItem *>& Command::options() {
  return options_;
}

const std::vector<CommandItem *>& Command::arguments() {
  return arguments_;
}

CommandItem *Command::get_option(const std::string& name) {
  // Robust for asking without "-"
  if (( name.length() > 0 ) && ( name[0] != '-' )) {
    return get_option("-" + name);
  }

  for (size_t i = 0; i < options_.size(); i++) {
    if (options_[i]->name() == name) {
      return options_[i];
    }
  }
  return NULL;
}

CommandItem *Command::get_argument(const std::string& name) {
  for (size_t i = 0; i < arguments_.size(); i++) {
    if (arguments_[i]->name() == name) {
      return arguments_[i];
    }
  }
  return NULL;
}

//
// CommandCatalogue*
//

CommandCatalogue::CommandCatalogue() {
  add("help", "general", "Get some help for commands and categories of commands");
  add_string_argument("help", "concept", "",
                      "Command or category of commands. Type 'help all' for a complete list of commands");
}

Command *CommandCatalogue::add(const std::string& name
                               , const std::string& category
                               , const std::string& short_description
                               , const std::string& help) {
  Command *command = new Command(name, category, short_description, help);

  commands_.push_back(command);
  return command;
}

CommandItem *CommandCatalogue::add_option(const std::string& command_name
                                          , const std::string& name
                                          , options::Type type
                                          , bool optional
                                          , const std::string& help
                                          , const std::string& default_value
                                          , const std::string& min_value
                                          , const std::string& max_value) {
  Command *c = get_command(command_name);

  if (!c) {
    LM_W(("Not possible to add option %s to command %s. This command has not been added previously"
          , name.c_str(), command_name.c_str()));
    return NULL;
  }
  
  // Robust for asking without "-"
  std::string final_name;
  if (( name.length() > 0 ) && ( name[0] != '-' )) {
    final_name = "-" + name;
  }
  else
    final_name = name;
  
  
  CommandItem *item = new CommandItem(final_name, type, optional, help, default_value, min_value, max_value);
  c->add_option(item);
  return item;
}

CommandItem *CommandCatalogue::add_argument(const std::string& command_name
                                            , const std::string& name
                                            , options::Type type
                                            , bool optional
                                            , const std::string& help
                                            , const std::string& default_value
                                            , const std::string& min_value
                                            , const std::string& max_value) {
  Command *c = get_command(command_name);

  if (!c) {
    LM_W(("Not possible to add argument '%s' to command '%s'. This command has not been added previously"
          , name.c_str(), command_name.c_str()));
    return NULL;
  }
  CommandItem *item = new CommandItem(name, type, optional, help, default_value, min_value, max_value);
  c->add_argument(item);
  return item;
}

CommandItem *CommandCatalogue::add_int_option(const std::string& command_name
                                              , const std::string& name
                                              , int default_value
                                              , const std::string& help) {
  return add_option(command_name
                    , name
                    , options::option_int
                    , true
                    , help
                    , au::str("%d", default_value));
}

CommandItem *CommandCatalogue::add_uint64_option(const std::string& command_name
                                                 , const std::string& name
                                                 , size_t default_value
                                                 , const std::string& help) {
  return add_option(command_name
                    , name
                    , options::option_uint64
                    , true
                    , help
                    , au::str("%lu", default_value));
}

CommandItem *CommandCatalogue::add_string_option(const std::string& command_name
                                                 , const std::string& name
                                                 , const std::string& default_value
                                                 , const std::string& help) {
  return add_option(command_name
                    , name
                    , options::option_string
                    , true
                    , help
                    , default_value);
}

CommandItem *CommandCatalogue::add_bool_option(const std::string& command_name
                                               , const std::string& name
                                               , const std::string& help) {
  return add_option(command_name
                    , name
                    , options::option_bool
                    , true
                    , help);
}

CommandItem *CommandCatalogue::add_string_argument(const std::string& command_name
                                                   , const std::string& name
                                                   , const std::string& default_value
                                                   , const std::string& help) {
  return add_argument(command_name
                      , name
                      , options::option_string
                      , true
                      , help
                      , default_value);
}

CommandItem *CommandCatalogue::add_mandatory_string_argument(const std::string& command_name
                                                             , const std::string& name
                                                             , const std::string& help) {
  return add_argument(command_name
                      , name
                      , options::option_string
                      , false
                      , help
                      );
}

CommandItem *CommandCatalogue::add_string_options_argument(const std::string& command_name
                                                           , const std::string& name
                                                           , const std::string& options_group
                                                           , const std::string& help) {
  CommandItem *item = add_argument(command_name
                                   , name
                                   , options::option_string
                                   , true
                                   , help);

  item->set_options_group(options_group);
  return item;
}

void CommandCatalogue::autoComplete(au::ConsoleAutoComplete *info) {
  // Autocomplete with the names
  if (info->completingFirstWord()) {
    info->add("help: Show help for commands or categories", "help", true);

    for (size_t i = 0; i < commands_.size(); i++) {
      info->add(commands_[i]->name());
    }
    return;
  }

  // Spetial help auto-completion
  if (info->firstWord() == "help") {
    if (info->completingSecondWord()) {
      info->add("all");      // To view help for all categories
      info->add("categories");      // To view available categories

      // Add all commands
      for (size_t i = 0; i < commands_.size(); i++) {
        info->add(commands_[i]->name());
      }

      // Add categories as options
      au::StringVector categories = getCategories();
      for (size_t i = 0; i < categories.size(); i++) {
        info->add(categories[i] + " (category)", categories[i], true);
      }
    } else {
      info->setHelpMessage("help message accepts a unique parameter");
      return;
    }

    return;
  }


  // Autocompleting with options and arguments if this is a valid command
  Command *c = get_command(info->firstWord());
  if (c) {
    c->autoComplete(info);
  } else {
    info->setHelpMessage(au::str("Command %s is not a valid command in this catalogue",
                                 info->firstWord().c_str()));
  }
}

std::string CommandCatalogue::getCommandsTable(const std::string& category) {
  au::StringVector columns = au::StringVector("Command", "Category,left", "Description,left");
  au::tables::Table table(columns);

  for (size_t i = 0; i < commands_.size(); i++) {
    if (category == "" || (commands_[i]->category() == category )) {
      table.addRow(au::StringVector(commands_[i]->name(), commands_[i]->category(),
                                    commands_[i]->short_description()));
    }
  }

  // Print the table

  if (category == "") {
    table.setTitle("All commands");
  } else {
    table.setTitle(au::str("Commands of category %s", category.c_str()));
  } return table.str();
}

std::string CommandCatalogue::getHelpForConcept(const std::string& name) {
  Command *c = get_command(name);

  if (c) {
    std::ostringstream output;

    output << au::StringRepeatingCharInConsole('-') << "\n";
    output << au::str(au::purple, "COMMAND       %s\n", name.c_str(), c->category().c_str());
    output << au::str(au::purple, "CATEGORY      %s\n", c->category().c_str());

    output << au::StringRepeatingCharInConsole('-') << "\n";

    output << au::str(au::purple, "DESCRIPTION   ") << c->short_description() << "\n";
    output << au::str(au::purple, "USAGE         ")  << c->usage() << "\n";
    output << "\n";

    // Show options for this command

    const std::vector<CommandItem *>& options   = c->options();
    const std::vector<CommandItem *>& arguments = c->arguments();

    for (size_t i = 0; i <  options.size(); i++) {
      output << au::str("                 %-s\n", options[i]->name().c_str());
      output << au::str_indent(options[i]->str_help(), 25);
      output << "\n";
    }

    for (size_t i = 0; i <  arguments.size(); i++) {
      output << au::str("                 %-s\n", arguments[i]->name().c_str());
      output << au::str_indent(arguments[i]->str_help(), 25);
      output << "\n";
    }


    if (c->help().length() > 0) {
      output << au::StringRepeatingCharInConsole('-') << "\n";

      if (c->help() != "") {
        output << au::str_indent(c->help(), 14);
        output << "\n";
      }
    }

    output << au::StringRepeatingCharInConsole('-') << "\n";

    return output.str();
  }


  // Check if it is a category...
  if (isValidCategory(name)) {
    return getCommandsTable(name);
  }

  // Show list of categories
  if (name == "categories") {
    au::StringVector cats = getCategories();
    std::ostringstream output;
    output << "\nCategories:\n\n";
    for (size_t i = 0; i < cats.size(); i++) {
      output << "\tCategory " << cats[i] << "\n";
    }
    return output.str();
  }

  if (name == "all") {
    return getCommandsTable();
  }

  return au::str("Unknown command or categories %s\n", name.c_str());
}

au::StringVector CommandCatalogue::getCategories() {
  au::StringVector categories;

  for (size_t i = 0; i < commands_.size(); i++) {
    categories.push_back(commands_[i]->category());
  }

  categories.RemoveDuplicated();
  return categories;
}

bool CommandCatalogue::isValidCommand(const std::string& command) {
  for (size_t i = 0; i < commands_.size(); i++) {
    if (commands_[i]->name() == command) {
      return true;
    }
  }
  return false;
}

bool CommandCatalogue::isValidCategory(const std::string& category) {
  for (size_t i = 0; i < commands_.size(); i++) {
    if (commands_[i]->category() == category) {
      return true;
    }
  }
  return false;
}

Command *CommandCatalogue::get_command(const std::string& name) {
  for (size_t i = 0; i < commands_.size(); i++) {
    if (commands_[i]->name() == name) {
      return commands_[i];
    }
  }
  return NULL;
}

CommandInstance *CommandCatalogue::parse(const std::string command_line, au::ErrorManager& error) {
  // Tokenize provided line
  token::Tokenizer tokenizer;

  tokenizer.addSingleCharTokens(" \t\n");
  token::TokenVector token_vector = tokenizer.parse(command_line);

  // Get component eliminating spaces ( literals are placed toguether )
  std::vector<std::string> components;
  while (!token_vector.eof()) {
    token::Token *token = token_vector.popToken();
    if (!token->isSeparator()) {
      components.push_back(token->content);
    }
  }

  // Analyse the line
  if (components.size() == 0) {
    error.set("No provided command");
    return NULL;
  }

  // Create a new C
  std::string main_command = components[0];
  Command *command = get_command(main_command);
  if (!command) {
    error.set(au::str("Command '%s' not found", main_command.c_str()));
    return NULL;
  }

  CommandInstance *command_instance = new CommandInstance(command, command_line);

  int pos_argument = 0;      // Position of the argument we are parsing

  for (size_t i = 1; i < components.size(); i++) {
    // Check if we are at the begining of a -option element
    if (components[i].length() > 0) {
      if (components[i][0] == '-') {
        std::string option_name = components[i];

        // Find option in the command
        CommandItem *item = command->get_option(option_name);
        if (!item) {
          error.set(au::str("Option %s is not defined for command %s", option_name.c_str(),
                             main_command.c_str()));
          error.AddWarning(au::str("Usage: %s", command->usage().c_str()));
          error.AddMessage(au::str("Type 'help %s' for more info.", command->name().c_str()));
          delete command_instance;
          return NULL;
        } else {
          if (item->type() == options::option_bool) {
            command_instance->set_value(option_name, "true");
            continue;
          } else {
            if (i == (components.size() - 1)) {
              error.set(au::str("No value provided for option %s in command %s"
                                 , option_name.c_str(), main_command.c_str()));
              error.AddWarning(au::str("Usage: %s", command->usage().c_str()));
              error.AddMessage(au::str("Type 'help %s' for more info.", command->name().c_str()));
              delete command_instance;
              return NULL;
            } else {
              std::string value = components[i + 1];
              i++;

              if (!item->isValidValue(value)) {
                error.set(au::str("%s is not a valid value for option %s in command %s",
                                   value.c_str(), option_name.c_str(), main_command.c_str()));
                error.AddWarning(au::str("Usage: %s", command->usage().c_str()));
                error.AddMessage(au::str("Type 'help %s' for more info.", command->name().c_str()));
                delete command_instance;
                return NULL;
              } else {
                command_instance->set_value(option_name, value);
                continue;
              }
            }
          }
        }
      }      // A new argument is obtained
    }
    if (command->arguments().size() <= (size_t)pos_argument) {
      error.set(au::str("Extra non-defined argument (%s) provided for Command %s"
                         , components[i].c_str()
                         , main_command.c_str(), command->usage().c_str()));
      error.AddWarning(au::str("Usage: %s", command->usage().c_str()));
      error.AddMessage(au::str("Type 'help %s' for more info.", command->name().c_str()));
      delete command_instance;
      return NULL;
    }

    // Recover item we are filling
    CommandItem *item = command->arguments()[pos_argument++];
    std::string value = components[i];

    if (!item->isValidValue(value)) {
      error.set(au::str("%s is not a valid value for argument %s in command %s",
                         value.c_str(), item->name().c_str(), main_command.c_str()));
      error.AddWarning(au::str("Usage: %s", command->usage().c_str()));
      error.AddMessage(au::str("Type 'help %s' for more info.", command->name().c_str()));
      delete command_instance;
      return NULL;
    }

    command_instance->set_value(item->name(), value);
  }

  // Verify it is correct
  for (size_t i = 0; i < command->options().size(); i++) {
    if (!command->options()[i]->optional()) {
      if (!command_instance->hasValueFor(command->options()[i]->name())) {
        error.set(au::str("Missing mandatory option %s in command %s",
                           command->options()[i]->name().c_str(), main_command.c_str()));
        error.AddWarning(au::str("Usage: %s", command->usage().c_str()));
        error.AddMessage(au::str("Type 'help %s' for more info.", command->name().c_str()));
        delete command_instance;
        return NULL;
      }
    }
  }

  for (size_t i = 0; i < command->arguments().size(); i++) {
    if (!command->arguments()[i]->optional()) {
      if (!command_instance->hasValueFor(command->arguments()[i]->name())) {
        error.set(au::str("Missing mandatory argument %s in command %s",
                           command->arguments()[i]->name().c_str(), main_command.c_str()));
        error.AddWarning(au::str("Usage: %s", command->usage().c_str()));
        error.AddMessage(au::str("Type 'help %s' for more info.", command->name().c_str()));
        delete command_instance;
        return NULL;
      }
    }
  }


  return command_instance;
}

void CommandCatalogue::add_tag(const std::string& command_name, const std::string& tag) {
  Command *c = get_command(command_name);

  if (!c) {
    LM_W(("Not possible to add tag %s to command %s. This command has not been added previously"
          , tag.c_str(), command_name.c_str()));
    return;
  }
  c->set_tag(tag);
}
}
}      // End of namespace au::console
