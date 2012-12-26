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
#include "au/CommandLine.h"             // Own interface
#include "au/log/LogMain.h"
#include <iostream>                     // std::cout

namespace au {
const std::string CommandLine::kUnknownValue("unknown");
const std::string CommandLine::kUnknownFlag("unknown_flag");
const std::string CommandLine::kNoArgument("no-argument");
const std::string CommandLine::kWrongType("wrong-type");
const std::string CommandLine::kTrue("true");
const std::string CommandLine::kFalse("false");

CommandLine::CommandLine(const std::string& command) {
  Parse(command);
}

CommandLine::CommandLine(int argc, const char *argv[]) {
  Parse(argc, argv);
}

void CommandLine::Parse(int args, const char *argv[], bool strict) {
  std::ostringstream o;

  for (int i = 0; i < args; ++i) {
    o << argv[i] << " ";
  }
  Parse(o.str(), strict);
}

void CommandLine::ResetFlags(void) {
  flags_.clear();
  arguments_.clear();
}

void CommandLine::SetFlagBoolean(const std::string& name) {
  CommandLineFlag flag;

  flag.type = kTypeBool;
  flag.default_value = kFalse;
  flag.collision_resolution = kCollisionOverwrite;

  flags_.insert(std::pair<std::string, CommandLineFlag>(name, flag));
}

void CommandLine::SetFlagInt(const std::string& name, int default_value) {
  SetFlagInt(name, default_value, kCollisionOverwrite);
}

void CommandLine::SetFlagInt(const std::string& name, int default_value,
                             CollisionResolutionValues collision_strategy) {
  CommandLineFlag flag;

  flag.type = kTypeInt;
  switch (collision_strategy) {
    case kCollisionConcatenateAtEnd:
    case kCollisionInsertAtBegin:
      LOG_SW(("Error, collision_resolution option not valid for int type flags"));
      flag.collision_resolution = kCollisionOverwrite;
      break;
    default:
      flag.collision_resolution = collision_strategy;
      break;
  }

  std::ostringstream o;
  o << default_value;
  flag.default_value = o.str();

  flags_.insert(std::pair<std::string, CommandLineFlag>(name, flag));
}

void CommandLine::SetFlagDouble(const std::string& name, double default_value) {
  SetFlagDouble(name, default_value, kCollisionOverwrite);
}

void CommandLine::SetFlagDouble(const std::string& name, double default_value,
                                CollisionResolutionValues collision_strategy) {
  CommandLineFlag flag;

  flag.type = kTypeDouble;
  switch (collision_strategy) {
    case kCollisionConcatenateAtEnd:
    case kCollisionInsertAtBegin:
      LOG_SW(("Error, collision_resolution option not valid for double type flags"));
      flag.collision_resolution = kCollisionOverwrite;
      break;
    default:
      flag.collision_resolution = collision_strategy;
      break;
  }

  std::ostringstream o;
  o << default_value;
  flag.default_value = o.str();

  flags_.insert(std::pair<std::string, CommandLineFlag>(name, flag));
}

void CommandLine::SetFlagString(const std::string& name, const std::string& default_value) {
  SetFlagString(name, default_value, kCollisionOverwrite);
}

void CommandLine::SetFlagString(const std::string& name, const std::string& default_value,
                                CollisionResolutionValues collision_strategy) {
  CommandLineFlag flag;

  flag.type = kTypeString;
  flag.default_value = default_value;
  switch (collision_strategy) {
    case kCollisionAddToPrevious:
    case kCollisionSubtractFromPrevious:
      LOG_SW(("Error, collision_resolution option not valid for string type flags"));
      flag.collision_resolution = kCollisionOverwrite;
      break;
    default:
      flag.collision_resolution = collision_strategy;
      break;
  }
  flags_.insert(std::pair<std::string, CommandLineFlag>(name, flag));
}

void CommandLine::SetFlagUint64(const std::string& name, const std::string& default_value) {
  SetFlagUint64(name, default_value, kCollisionOverwrite);
}

void CommandLine::SetFlagUint64(const std::string& name, size_t default_value) {
  std::ostringstream o;

  o << default_value;
  SetFlagUint64(name, o.str(), kCollisionOverwrite);
}

void CommandLine::SetFlagUint64(const std::string& name, size_t default_value,
                                CollisionResolutionValues collision_strategy) {
  std::ostringstream o;

  o << default_value;
  SetFlagUint64(name, o.str(), collision_strategy);
}

void CommandLine::SetFlagUint64(const std::string& name, const std::string& default_value,
                                CollisionResolutionValues collision_strategy) {
  CommandLineFlag flag;

  flag.type = kTypeUInt64;
  flag.default_value = default_value;
  switch (collision_strategy) {
    case kCollisionConcatenateAtEnd:
    case kCollisionInsertAtBegin:
      LOG_SW(("Error, collision_resolution option not valid for uint64 type flags"));
      flag.collision_resolution = kCollisionOverwrite;
      break;
    default:
      flag.collision_resolution = collision_strategy;
      break;
  }
  flags_.insert(std::pair<std::string, CommandLineFlag>(name, flag));
}

void CommandLine::Parse(const std::string& command, bool strict) {
  ClearValues();

  std::vector<std::string> tockens;

  // Copy the original command
  command_ = command;

  std::string delimiters = " \t\n";  // All possible delimiters
  std::string delimiters_and_literal = " \t\n";

  std::string::size_type pos = 0;
  while (pos < command.length()) {
    // skip the delimiters
    pos = command.find_first_not_of(delimiters, pos);

    if (pos == std::string::npos) {
      break;  // No more tockens in this string
    }
    if (command[pos] == '\"') {
      if (command.length() <= pos) {
        break;  // No more characters to finish the literal
      }
      ++pos;
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
        break;  // No more characters to finish the literal
      }
      ++pos;
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

  ParseTockens(tockens, strict);
}

std::string CommandLine::AddFlagValues(const std::string& addend1, const std::string& addend2, TypeValues type) {
  std::string result;

  switch (type) {
    case kTypeBool: {
      LOG_SW(("Wrong operation on flag type bool"));
      result = addend1;
      break;
    }
    case kTypeDouble: {
      double val_addend1 = getDoubleValue(addend1);
      double val_addend2 = getDoubleValue(addend2);
      result = au::str("%lf", val_addend1 + val_addend2);
      break;
    }
    case kTypeInt: {
      int val_addend1 = getIntValue(addend1);
      int val_addend2 = getIntValue(addend2);
      result = au::str("%d", val_addend1 + val_addend2);
      break;
    }
    case kTypeString: {
      result = addend1 + addend2;
      break;
    }
    case kTypeUInt64: {
      size_t val_addend1 = getUint64Value(addend1);
      size_t val_addend2 = getUint64Value(addend2);
      result = au::str("%lu", val_addend1 + val_addend2);
      break;
    }
    default: {
      LOG_SE(("Unknown flag type"));
      result = addend1;
      break;
    }
  }
  return result;
}

std::string CommandLine::SubtractFlagValues(const std::string& minuend, const std::string& subtrahend,
                                            TypeValues type) {
  std::string result;

  switch (type) {
    case kTypeBool: {
      LOG_SW(("Wrong operation on flag type bool"));
      result = minuend;
      break;
    }
    case kTypeDouble: {
      double val_minuend = getDoubleValue(minuend);
      double val_subtrahend = getDoubleValue(subtrahend);
      result = au::str("%lf", val_minuend - val_subtrahend);
      break;
    }
    case kTypeInt: {
      int val_minuend = getIntValue(minuend);
      int val_subtrahend = getIntValue(subtrahend);
      result = au::str("%d", val_minuend - val_subtrahend);
      break;
    }
    case kTypeString: {
      LOG_SW(("Wrong operation on flag type string"));
      result = minuend;
      break;
    }
    case kTypeUInt64: {
      size_t val_minuend = getUint64Value(minuend);
      size_t val_subtrahend = getUint64Value(subtrahend);
      if (val_subtrahend > val_minuend) {
        result = "0";
      } else {
        result = au::str("%lu", val_minuend - val_subtrahend);
      }
      break;
    }
    default: {
      LOG_SE(("Unknown flag type"));
      result = minuend;
      break;
    }
  }
  return result;
}

void CommandLine::ClearValues() {
  // Remove the "value" field in all "flags"
  std::map<std::string, CommandLineFlag>::iterator iter;
  for (iter = flags_.begin(); iter != flags_.end(); ++iter) {
    iter->second.value = kUnknownValue;  // Default value when no assigned
  }
  // Remove the arguments vector
  arguments_.clear();
}

void CommandLine::ParseTockens(std::vector<std::string> &tockens, bool strict) {
  // Parse tockens to distinguish between arguments and flags parameters

  std::vector<std::string>::iterator iter;

  std::map<std::string, CommandLineFlag>::iterator flag_iterator;

  for (iter = tockens.begin(); iter < tockens.end(); ++iter) {
    std::string tocken = *iter;

#ifdef CommandLine_DEBUG
    fprintf(stderr, "Processing tocken: %s\n", tocken.c_str());
#endif

    if (tocken[0] == '-') {
      std::string flag_name = tocken.substr(1, tocken.size() - 1);

      flag_iterator = flags_.find(flag_name);
      if (flag_iterator != flags_.end()) {
        if (flag_iterator->second.type == kTypeBool) {
          flag_iterator->second.value = kTrue;
        } else {
          // This is a unary parameter
          ++iter;
          if (iter != tockens.end()) {
            if (flag_iterator->second.value != kUnknownValue) {
              switch (flag_iterator->second.collision_resolution) {
                case kCollisionAddToPrevious:
                  flag_iterator->second.value = AddFlagValues(flag_iterator->second.value, *iter,
                                                              flag_iterator->second.type);
                  break;
                case kCollisionConcatenateAtEnd:
                  flag_iterator->second.value = flag_iterator->second.value + "." + *iter;
                  break;
                case kCollisionIgnore:
                  if (strict) {
                    LOG_SW(("Multiple appearances of flag:'%s', ignoring all but the first one", flag_name.c_str()));
                  }
                  break;
                case kCollisionInsertAtBegin:
                  flag_iterator->second.value = *iter + "." + flag_iterator->second.value;
                  break;
                case kCollisionOverwrite:
                  flag_iterator->second.value = *iter;
                  break;
                case kCollisionSubtractFromPrevious:
                  flag_iterator->second.value = SubtractFlagValues(flag_iterator->second.value, *iter,
                                                                   flag_iterator->second.type);
                  break;
              }
            } else {
              flag_iterator->second.value = *iter;
            }
          }
        }
      } else {
        if (strict) {
          LOG_SW(("Flag:'%s' in command line, not present among flags to be processed", flag_name.c_str()));
        }
      }
    } else {
      // Normal argument
      arguments_.push_back(tocken);
    }
  }
}

/**
 * Access information about arguments
 */

int CommandLine::get_num_arguments() const {
  return arguments_.size();
}

std::string CommandLine::get_argument(int index) const {
  if ((index < 0) || (index >= (int)arguments_.size())) {
    return kNoArgument;
  }
  return arguments_[index];
}

/**
 * Access to flags
 */

std::string CommandLine::GetFlagValue(const std::string& flag_name) const {
  std::map<std::string, CommandLineFlag>::const_iterator flag_iterator;

  flag_iterator = flags_.find(flag_name);
  if (flag_iterator != flags_.end()) {
    if (flag_iterator->second.value != kUnknownValue) {
      return flag_iterator->second.value;
    } else {
      return flag_iterator->second.default_value;
    }
  } else {
    return kUnknownFlag;
  }
}

CommandLine::TypeValues CommandLine::GetFlagType(const std::string& flag_name) const {
  std::map<std::string, CommandLineFlag>::const_iterator flag_iterator;

  flag_iterator = flags_.find(flag_name);
  if (flag_iterator != flags_.end()) {
    return flag_iterator->second.type;
  } else {
    LOG_SW(("Flag:'%s' not present among flags to be processed, type string returned", flag_name.c_str()));
    return kTypeString;
  }
}

/**
 * Specialized access to parameters
 */

bool CommandLine::GetFlagBool(const std::string& flag_name) const {
  if (GetFlagType(flag_name) != kTypeBool) {
    return false;
  }

  return getBoolValue(GetFlagValue(flag_name));
}

int CommandLine::GetFlagInt(const std::string& flag_name) const {
  if (GetFlagType(flag_name) != kTypeInt) {
    return 0;
  }

  return getIntValue(GetFlagValue(flag_name));
}

double CommandLine::GetFlagDouble(const std::string& flag_name) const {
  if (GetFlagType(flag_name) != kTypeDouble) {
    return 0;
  }

  return getDoubleValue(GetFlagValue(flag_name));
}

std::string CommandLine::GetFlagString(const std::string& flag_name) const {
  if (GetFlagType(flag_name) != kTypeString) {
    return kWrongType;
  }

  return (GetFlagValue(flag_name));
}

size_t CommandLine::GetFlagUint64(const std::string& flag_name) const {
  if (GetFlagType(flag_name) != kTypeUInt64) {
    return 0;
  }

  std::string value = GetFlagValue(flag_name);
  size_t n = getUint64Value(value);

  return n;
}

/**
 * Functions to transform values
 */

int CommandLine::getIntValue(const std::string& value) {
  return atoi(value.c_str());
}

size_t CommandLine::getUint64Value(const std::string& value) {
  size_t base = 1;  // Default base
  size_t num = 0;

  if (value[value.size() - 1] == 'K') {
    base = 1024;
    num = strtoull(value.substr(0, value.size() - 1).c_str(), NULL, 10);
  } else if (value[value.size() - 1] == 'M') {
    base = 1024 * 1024;
    num = strtoull(value.substr(0, value.size() - 1).c_str(), NULL, 10);
  } else if (value[value.size() - 1] == 'G') {
    base = 1024 * 1024 * 1024;
    num = strtoull(value.substr(0, value.size() - 1).c_str(), NULL, 10);
  } else {
    num = strtoull(value.c_str(), NULL, 10);
  }
  return base * num;
}

double CommandLine::getDoubleValue(const std::string& value) {
  return atof(value.c_str());
}

bool CommandLine::getBoolValue(const std::string& value) {
  return (value == kTrue);
}

std::string CommandLine::command() const {
  return command_;
}
}
