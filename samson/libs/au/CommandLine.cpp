
#include <iostream>                     // std::cout

#include "au/CommandLine.h"             // Own interface

namespace au {
CommandLine::CommandLine(std::string command) {
  Parse(command);
}

CommandLine::CommandLine(int argc, const char *argv[]) {
  Parse(argc, argv);
}

void CommandLine::Parse(int args, const char *argv[]) {
  std::ostringstream o;

  for (int i = 0; i < args; i++) {
    o << argv[i] << " ";
  }
  Parse(o.str());
}

void CommandLine::ResetFlags() {
  flags_.clear();
  arguments_.clear();
}

void CommandLine::SetFlagBoolean(std::string name) {
  CommandLineFlag flag;

  flag.type = "bool";
  flag.default_value = "false";

  flags_.insert(std::pair<std::string, CommandLineFlag>(name, flag));
}

void CommandLine::SetFlagInt(std::string name, int default_value) {
  CommandLineFlag flag;

  flag.type = "int";

  std::ostringstream o;
  o << default_value;
  flag.default_value = o.str();

  flags_.insert(std::pair<std::string, CommandLineFlag>(name, flag));
}

void CommandLine::SetFlagDouble(std::string name, double default_value) {
  CommandLineFlag flag;

  flag.type = "double";

  std::ostringstream o;
  o << default_value;
  flag.default_value = o.str();

  flags_.insert(std::pair<std::string, CommandLineFlag>(name, flag));
}

void CommandLine::SetFlagString(std::string name, std::string default_value) {
  CommandLineFlag flag;

  flag.type = "string";
  flag.default_value = default_value;

  flags_.insert(std::pair<std::string, CommandLineFlag>(name, flag));
}

void CommandLine::SetFlagUint64(std::string name, std::string default_value) {
  CommandLineFlag flag;

  flag.type = "uint64";
  flag.default_value = default_value;

  flags_.insert(std::pair<std::string, CommandLineFlag>(name, flag));
}

void CommandLine::SetFlagUint64(std::string name, size_t default_value) {
  CommandLineFlag flag;

  flag.type = "uint64";

  std::ostringstream o;
  o << default_value;
  flag.default_value = o.str();

  flags_.insert(std::pair<std::string, CommandLineFlag>(name, flag));
}

void CommandLine::Parse(std::string command) {
  ClearValues();

  std::vector<std::string> tockens;

  // Copy the original command
  command_ = command;

  std::string delimiters = " \t\n";                                     // All possible delimiters
  std::string delimiters_and_literal = " \t\n";

  std::string::size_type pos = 0;
  while (pos < command.length()) {
    // skip the delimeters
    pos = command.find_first_not_of(delimiters, pos);

    if (pos == std::string::npos) {
      break;            // No more tockens in this string
    }
    if (command[pos] == '\"') {
      if (command.length() <= pos) {
        break;          // No more characters to finish the literal
      }
      pos++;
      size_t last = command.find_first_of("\"", pos);
      if (last == std::string::npos) {
        // Add the unfinished literal and finish
        tockens.push_back(command.substr(pos, command.length() - pos));
        break;
      } else {
        tockens.push_back(command.substr(pos, last - pos));
        pos = last + 1;
      }
    } else if (command[pos] == '\'') {
      if (command.length() <= pos) {
        break;          // No more characters to finish the literal
      }
      pos++;
      size_t last = command.find_first_of("'", pos);
      if (last == std::string::npos) {
        // Add the unfinished literal and finish
        tockens.push_back(command.substr(pos, command.length() - pos));
        break;
      } else {
        tockens.push_back(command.substr(pos, last - pos));
        pos = last + 1;
      }
    } else {
      // find the next element to break the tocken ( it could be a literal begin )
      size_t last = command.find_first_of(delimiters_and_literal, pos);

      if (last == std::string::npos) {
        // Create the last tocken
        tockens.push_back(command.substr(pos, command.length() - pos));
        break;
      } else {
        tockens.push_back(command.substr(pos, last - pos));
        pos = last;
      }
    }
  }

  ParseTockens(tockens);
}

void CommandLine::ClearValues() {
  // Remove the "value" field in all "flags"
  std::map< std::string, CommandLineFlag >::iterator iter;
  for (iter = flags_.begin(); iter != flags_.end(); iter++) {
    iter->second.value = "unknown";     // Default value when no assigned
  }
  // Remove the arguments vector
  arguments_.clear();
}

void CommandLine::ParseTockens(std::vector<std::string> &tockens) {
  // Parse tockens to distinghuish between arguments and flags parameters

  std::vector<std::string>::iterator iter;

  std::map<std::string, CommandLineFlag>::iterator flag_iterator;


  for (iter = tockens.begin(); iter < tockens.end(); iter++) {
    std::string tocken = *iter;


#ifdef CommandLine_DEBUG
    fprintf(stderr, "Processing tocken: %s\n", tocken.c_str());
#endif

    if (tocken[0] == '-') {
      std::string flag_name = tocken.substr(1, tocken.size() - 1);

      flag_iterator = flags_.find(flag_name);
      if (flag_iterator != flags_.end()) {
        if (flag_iterator->second.type == "bool") {
          flag_iterator->second.value = "true";
        } else {
          // This is a unitary parameter
          iter++;
          if (iter != tockens.end()) {
            flag_iterator->second.value = *iter;
          }
        }
      }
    } else {
      // Normal argument
      arguments_.push_back(tocken);
    }
  }
}

/**
 * Acces information about arguments
 */

int CommandLine::get_num_arguments() {
  return arguments_.size();
}

std::string CommandLine::get_argument(int index) {
  if ((index < 0) || ( index >= (int)arguments_.size())) {
    return "no-argument";
  }

  return arguments_[index];
}

/**
 * Access to flags
 */

std::string CommandLine::GetFlagValue(std::string flag_name) {
  std::map<std::string, CommandLineFlag>::iterator flag_iterator;

  flag_iterator = flags_.find(flag_name);
  if (flag_iterator != flags_.end()) {
    if (flag_iterator->second.value != "unknown") {
      return flag_iterator->second.value;
    } else {
      return flag_iterator->second.default_value;
    }
  } else {
    return "unknown_flag";
  }
}

std::string CommandLine::GetFlagType(std::string flag_name) {
  std::map<std::string, CommandLineFlag>::iterator flag_iterator;

  flag_iterator = flags_.find(flag_name);
  if (flag_iterator != flags_.end()) {
    return flag_iterator->second.type;
  } else {
    return "unknown_type";
  }
}

/**
 * Specialed access to parametes
 */

bool CommandLine::GetFlagBool(std::string flag_name) {
  if (GetFlagType(flag_name) != "bool") {
    return false;
  }

  return getBoolValue(GetFlagValue(flag_name));
}

int CommandLine::GetFlagInt(std::string flag_name) {
  if (GetFlagType(flag_name) != "int") {
    return 0;
  }

  return getIntValue(GetFlagValue(flag_name));
}

double CommandLine::GetFlagDouble(std::string flag_name) {
  if (GetFlagType(flag_name) != "double") {
    return 0;
  }

  return getDoubleValue(GetFlagValue(flag_name));
}

std::string CommandLine::GetFlagString(std::string flag_name) {
  if (GetFlagType(flag_name) != "string") {
    return 0;
  }

  return ( GetFlagValue(flag_name));
}

size_t CommandLine::GetFlagUint64(std::string flag_name) {
  if (GetFlagType(flag_name) != "uint64") {
    return 0;
  }

  return getUint64Value(GetFlagValue(flag_name));
}

/**
 * Functions to transform values
 */

int CommandLine::getIntValue(std::string value) {
  return atoi(value.c_str());
}

size_t CommandLine::getUint64Value(std::string value) {
  size_t base = 1;      // Default base

  if (value[value.size() - 1] == 'M') {
    base = 1024 * 1024;
    value = value.substr(0, value.size() - 1);
  } else if (value[value.size() - 1] == 'G') {
    base = 1024 * 1024 * 1024;
    value = value.substr(0, value.size() - 1);
  }

  return base * atoi(value.c_str());
}

double CommandLine::getDoubleValue(std::string value) {
  return atof(value.c_str());
}

bool CommandLine::getBoolValue(std::string value) {
  return ( value == "true" );
}

std::string CommandLine::command() {
  return command_;
}
}
