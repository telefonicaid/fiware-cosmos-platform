#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H
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

/* ****************************************************************************
*
* FILE            CommandLine.h
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            Septembre 2011
*
* DESCRIPTION
*
* CommandLine
*
* Class used to parse a string containing a command with multiple flags and arguments
* The advantage of this class is that is supports running-time definition of possible flags.
*
*
* Example: command arg1 -f input.txt -g 2 arg2
*
* Depending on how you define flags "f" and "g" this will parse in a different way
*
* You can define flags as type "bool", "double", "int" "string" "uint64".
* The properties of each element are:
*   bool:       It is a true/false flag. It does not take any parameter
*   int:        Takes an integer parameter with it.   Example: -n 12
*   double:     Takes a double parameter.               Example: -threshold 5.67
*   uint64:     Takes an unsigned parameter.            Example: -size 12G , -size 100
*   string:     Takes a string parameter.              Example: -file andreu.txt
*
* In case of multiple assignments on the same flag (if it is of string type), its behaviour can be controlled
* with the string collisition_resolution:
*   overwrite:   Forget previous flag value
*   begin:       Put the new value at the beginning (separated by a dot ("."))
*   end:         Put the new value at the end (separated by a dot ("."))
*   ignore:      Ignore the new value and write a warning
*
* The basic usage has three steps:
*   * Use command set_flag_X( ) methods to define flags and types
*   * Use one of the parse(.) methods to parse the incoming command line
*   * Use get_flag_X methods to access content *
*
* ****************************************************************************/

#include <stdio.h>             // fprintf, stderr, ...

#include <cstdlib>
#include <map>
#include <sstream>
#include <string>
#include <vector>



namespace au {
typedef struct CommandLineFlag {
  std::string type;
  std::string default_value;
  std::string collision_resolution;
  std::string value;
} CommandLineFlag;

class CommandLine {
public:
  // Constant strings for the "collision_resolution" field
  static const std::string kCollisionBegin;
  static const std::string kCollisionEnd;
  static const std::string kCollisionIgnore;
  static const std::string kCollisionOverwrite;

  static const std::string kUnknownValue;

  /**
   * Simple constructor
   */

  CommandLine() {
  }

  /**
   * Constructor with the provided string or argument list
   * In this case no flags are considered
   */

  CommandLine(const std::string& _command);
  CommandLine(int argc, const char *argv[]);

  /**
   * Parse a command line in the classical C-style way
   */

  void Parse(const std::string& _command);
  void Parse(int args, const char *argv[]);

  /**
   * Defining flags
   */

  void ResetFlags();

  void SetFlagBoolean(const std::string& name);
  void SetFlagInt(const std::string& name, int default_value);
  void SetFlagString(const std::string& name, const std::string& default_value);
  void SetFlagString(const std::string& name, const std::string& default_value, const std::string& collision);
  void SetFlagUint64(const std::string& name, const std::string& default_value);
  void SetFlagUint64(const std::string& name, size_t default_value);
  void SetFlagDouble(const std::string& name, double default_value);

  /**
   * Access information about arguments
   */

  int get_num_arguments() const;
  std::string get_argument(int index) const;

  /**
   * Specialized access to parameters
   */

  bool GetFlagBool(const std::string& flag_name) const;
  int GetFlagInt(const std::string& flag_name) const;
  double GetFlagDouble(const std::string& flag_name) const;
  std::string GetFlagString(const std::string& flag_name) const;
  size_t GetFlagUint64(const std::string& flag_name) const;

  // Original command+
  std::string command() const;

private:
  // Constant strings for the "type" field
  static const std::string kTypeBool;
  static const std::string kTypeDouble;
  static const std::string kTypeInt;
  static const std::string kTypeString;
  static const std::string kTypeUInt64;

  /**
   * Access to flags
   */

  std::string GetFlagValue(const std::string& flag_name) const;
  std::string GetFlagType(const std::string& flag_name) const;

  /**
   * Functions to transform values
   */

  static int getIntValue(const std::string& value);
  static size_t getUint64Value(std::string value);
  static double getDoubleValue(const std::string& value);
  static bool getBoolValue(const std::string& value);

  // Internal function to parse content
  void ClearValues();
  void ParseTockens(std::vector<std::string> &tockens);

  std::string command_;                                                                          // !<< Incoming General command
  std::map< std::string, CommandLineFlag > flags_;       // !<< Map of flags
  std::vector<std::string> arguments_;                                                           // !<< Command line arguments separated in "words"
};
}

#endif  // ifndef COMMAND_LINE_H
