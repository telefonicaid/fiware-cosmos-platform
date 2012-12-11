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
#include <stdio.h>             // fprintf, stderr, ...

#include <cstdlib>
#include <map>
#include <sstream>
#include <string>
#include <vector>

/**
 * \file CommandLine.h
 * \mainpage CommandLine
 * \author Andreu Urruela
 * \date Septembre 2011
 *
 * Tool to configure and parse command line arguments
 *
 */

namespace au {
typedef struct CommandLineFlag {
  std::string type;
  std::string default_value;
  std::string collision_resolution;
  std::string value;
} CommandLineFlag;

/**
 * \class CommandLine
 * \brief Class used to parse a string containing a command with multiple flags and arguments
 * The advantage of this class is that is supports running-time definition of possible flags.
 * Example: command arg1 -f input.txt -g 2 arg2
 * Depending on how flags "f" and "g" are defined, this will parse in a different way
 * Flags can be defined being of types "bool", "double", "int" "string" "uint64".
 * For each flag type:
 *   bool:       It is a true/false flag. It does not take any parameter
 *   double:     Takes a double parameter.              Example: -threshold 5.67
 *   int:        Takes an integer parameter with it.    Example: -n 12
 *   string:     Takes a string parameter.              Example: -file andreu.txt
 *   uint64:     Takes an unsigned parameter.           Example: -size 12G , -size 100
 * In case of multiple assignments on the same flag (if it is of string type), its behaviour can be controlled
 * with the collisition_resolution field:
 *   kCollisionBegin:       Put the new value at the beginning (separated by a dot ("."))
 *   kCollisionEnd:         Put the new value at the end (separated by a dot ("."))
 *   kCollisionIgnore:      Ignore the new value and write a warning
 *   kCollisionOverwrite:   Forget previous flag value
 * The basic usage has three steps:
 *   * Use command set_flag_X( ) methods to define flags and types
 *   * Use one of the parse(.) methods to parse the incoming command line
 *   * Use get_flag_X methods to access content *
 */

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
   * \brief Parse a command line using the previously defined flags
   * \param[in] command line to be parsed
   */
  void Parse(const std::string& command);

  /**
   * \brief Parse a command line using the previously defined flags
   * \param[in] number of arguments
   * \param[in] array of pointers to the arguments
   */
  void Parse(int args, const char *argv[]);

  /**
   * \brief Reset all the added flags (do not remove them)
   * Just remove the referred value
   */
  void ResetFlags();

  /**
   * \brief Add a new flag of type boolean
   * \param[in] name of the flag
   */
  void SetFlagBoolean(const std::string& name);
  /**
   * \brief Add a new flag of type int
   * \param[in] name of the flag
   * \param[in] default value for the flag (value to be found when flag does not appear in the command line)
   */
  void SetFlagInt(const std::string& name, int default_value);
  /**
   * \brief Add a new flag of type string
   * For string type flags, the behaviour when multiple appearances can be configured.
   * In this case, it is defaulted to overwrite
   * \param[in] name of the flag
   * \param[in] default value for the flag (value to be found when flag does not appear in the command line)
   */
  void SetFlagString(const std::string& name, const std::string& default_value);
  /**
   * \brief Add a new flag of type string
   * For string type flags, the behaviour when multiple appearances can be configured.
   * \param[in] name of the flag
   * \param[in] default value for the flag (value to be found when flag does not appear in the command line)
   * \param[in] behaviour when multiple appearances of flag in command line. Options defined as constants
   */
  void SetFlagString(const std::string& name, const std::string& default_value, const std::string& collision);
  /**
   * \brief Add a new flag of type uint64
   * \param[in] name of the flag
   * \param[in] default value (as a string) for the flag (value to be found when flag does not appear in the command line)
   */
  void SetFlagUint64(const std::string& name, const std::string& default_value);
  /**
   * \brief Add a new flag of type uint64
   * \param[in] name of the flag
   * \param[in] default value (as a uint64) for the flag (value to be found when flag does not appear in the command line)
   */
  void SetFlagUint64(const std::string& name, size_t default_value);
  /**
   * \brief Add a new flag of type double
   * \param[in] name of the flag
   * \param[in] default value for the flag (value to be found when flag does not appear in the command line)
   */
  void SetFlagDouble(const std::string& name, double default_value);

  /**
   * \brief Number of arguments found in the command line
   * \return number of arguments in the command line
   */
  int get_num_arguments() const;

  /**
   * \brief Access one argument by index
   * \param[in] index of the argument
   * \return    string with the value of the argument at index position
   */
  std::string get_argument(int index) const;

  /**
   * \brief Access the value of flag of type boolean
   * \param[in] name of the flag
   * \return    string with the value detected in the command lines
   */
  bool GetFlagBool(const std::string& flag_name) const;
  /**
   * \brief Access the value of flag of type int
   * \param[in] name of the flag
   * \return    value detected in the command lines
   */
  int GetFlagInt(const std::string& flag_name) const;
  /**
   * \brief Access the value of flag of type double
   * \param[in] name of the flag
   * \return    value detected in the command lines
   */
  double GetFlagDouble(const std::string& flag_name) const;
  /**
   * \brief Access the value of flag of type string
   * \param[in] name of the flag
   * \return    value detected in the command lines
   */
  std::string GetFlagString(const std::string& flag_name) const;
  /**
   * \brief Access the value of flag of type uint64
   * \param[in] name of the flag
   * \return    value detected in the command lines
   */
  size_t GetFlagUint64(const std::string& flag_name) const;

  /**
   * \brief Command with all arguments and flags
   * \return string with the reconstructed command
   */
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
