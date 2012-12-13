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
/**
 * \class CommandLine
 * \brief Class used to parse a string containing a command with multiple flags and arguments
 * The advantage of this class is that is supports running-time definition of possible flags.
 * Example: command arg1 -f input.txt -g 2 arg2
 * Depending on how flags "f" and "g" are defined, this will parse in a different way
 * Flags can be defined being of types boolean, double, int, string or uint64.
 * For each flag type:
 *   bool:       It is a true/false flag. It does not take any parameter
 *   double:     Takes a double parameter.              Example: -threshold 5.67
 *   int:        Takes an integer parameter with it.    Example: -n 12
 *   string:     Takes a string parameter.              Example: -file andreu.txt
 *   uint64:     Takes an unsigned parameter.           Example: -size 12G , -size 100
 * In case of multiple assignments on the same flag (if it is of string type), its behaviour can be controlled
 * with the collisition_resolution field:
 *   kCollisionAddToPrevious:        Add new value (numeric) to the previous one
 *   kCollisionConcatenateAtEnd:     Append new value (string) to the end of the previous one (separated by a dot ("."))
 *   kCollisionIgnore:               Ignore new value (create a warning)
 *   kCollisionInsertAtBegin:        Insert new value (string) at the beginning of the previous one
 *   kCollisionOverwrite:            Overwrite the flag value with the last detected, forgetting previous ones
 *   kCollisionSubtractFromPrevious: Subtract new value (numeric) from the previous one
 * The basic usage has three steps:
 *   * Use command set_flag_X( ) methods to define flags and types
 *   * Use one of the parse(.) methods to parse the incoming command line
 *   * Use get_flag_X methods to access content *
 */

class CommandLine {
public:
  /**< Possible values for the flag type field */
  enum TypeValues {
    kTypeBool = 0,  /**< boolean */
    kTypeDouble,    /**< double */
    kTypeInt,       /**< integer */
    kTypeString,    /**< string */
    kTypeUInt64     /**< unsigned 64 bits integer */
  };
  /**< Possible values for the "collision_resolution" field */
  enum CollisionResolutionValues {
    kCollisionAddToPrevious = 0,     /**< Add new value (numeric) to the previous one */
    kCollisionConcatenateAtEnd,      /**< Append new value (string) to the end of the previous one */
    kCollisionIgnore,                /**< Ignore new value (create a warning) */
    kCollisionInsertAtBegin,         /**< Insert new value (string) at the beginning of the previous one */
    kCollisionOverwrite,             /**< Overwrite the flag value with the last detected, forgetting previous ones */
    kCollisionSubtractFromPrevious   /**< Subtract new value (numeric) from the previous one */
  };
  /**< Struct to keep flag information */
  typedef struct CommandLineFlag {
    TypeValues type;
    std::string default_value;
    CollisionResolutionValues collision_resolution;
    std::string value;
  } CommandLineFlag;

  static const std::string kUnknownValue;  /**< Assigned to reseted flags */
  static const std::string kUnknownFlag;   /**< Returned when the flag name is not found in the list of flags */
  static const std::string kNoArgument;    /**< No argument at the requested position */
  static const std::string kWrongType;     /**< Wrong type for a string flag */
  static const std::string kTrue;          /**< String for true value */
  static const std::string kFalse;         /**< String for false value */

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
   * \param[in] In script mode, warning messages are shown
   */
  void Parse(const std::string& command, bool strict = true);

  /**
   * \brief Parse a command line using the previously defined flags
   * \param[in] number of arguments
   * \param[in] array of pointers to the arguments
   * \param[in] In script mode, warning messages are shown
   */
  void Parse(int args, const char *argv[], bool strict = true);

  /**
   * \brief Reset all the added flags (do not remove them)
   * Just remove the referred value
   */
  void ResetFlags(void);

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
   * \brief Add a new flag of type int
   * \param[in] name of the flag
   * \param[in] default value for the flag (value to be found when flag does not appear in the command line)
   */
  void SetFlagInt(const std::string& name, int default_value, CollisionResolutionValues collision_strategy);
  /**
   * \brief Add a new flag of type string
   * For string type flags, the behaviour when multiple appearances can be configured.
   * In this case, it is defaulted to overwrite
   * \param[in] name of the flag
   * \param[in] default value for the flag (value to be found when flag does not appear in the command line)
   * \param[in] behaviour when multiple appearances of flag in command line. Options defined as enum items
   */
  void SetFlagString(const std::string& name, const std::string& default_value);
  /**
   * \brief Add a new flag of type string
   * For string type flags, the behaviour when multiple appearances can be configured.
   * \param[in] name of the flag
   * \param[in] default value for the flag (value to be found when flag does not appear in the command line)
   * \param[in] behaviour when multiple appearances of flag in command line. Options defined as enum items
   */
  void SetFlagString(const std::string& name, const std::string& default_value,
                     CollisionResolutionValues collision_strategy);
  /**
   * \brief Add a new flag of type uint64
   * \param[in] name of the flag
   * \param[in] default value (as a string) for the flag (value to be found when flag does not appear in the command line)
   */
  void SetFlagUint64(const std::string& name, const std::string& default_value);
  /**
   * \brief Add a new flag of type uint64
   * \param[in] name of the flag
   * \param[in] default value (as a string) for the flag (value to be found when flag does not appear in the command line)
   * \param[in] behaviour when multiple appearances of flag in command line. Options defined as enum items
   */
  void SetFlagUint64(const std::string& name, const std::string& default_value,
                     CollisionResolutionValues collision_strategy);
  /**
   * \brief Add a new flag of type uint64
   * \param[in] name of the flag
   * \param[in] default value (as a uint64) for the flag (value to be found when flag does not appear in the command line)
   */
  void SetFlagUint64(const std::string& name, size_t default_value);
  /**
   * \brief Add a new flag of type uint64
   * \param[in] name of the flag
   * \param[in] default value (as a uint64) for the flag (value to be found when flag does not appear in the command line)
   * \param[in] behaviour when multiple appearances of flag in command line. Options defined as enum items
   */
  void SetFlagUint64(const std::string& name, size_t default_value, CollisionResolutionValues collision_strategy);
  /**
   * \brief Add a new flag of type double
   * \param[in] name of the flag
   * \param[in] default value for the flag (value to be found when flag does not appear in the command line)
   */
  void SetFlagDouble(const std::string& name, double default_value);
  /**
   * \brief Add a new flag of type double
   * \param[in] name of the flag
   * \param[in] default value for the flag (value to be found when flag does not appear in the command line)
   * \param[in] behaviour when multiple appearances of flag in command line. Options defined as enum items
   */
  void SetFlagDouble(const std::string& name, double default_value, CollisionResolutionValues collision_strategy);

  /**
   * \brief Number of arguments found in the command line
   * \return number of arguments in the command line
   */
  int get_num_arguments(void) const;

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
   * \brief Add values according to the flag type
   * \param[in] first value to be added
   * \param[in] second value to be added
   * \param[in] flag type
   * \return    the addition in string format, according to the flag type
   */
  static std::string AddFlagValues(const std::string& addend1, const std::string& addend2, TypeValues type);

  /**
   * \brief Substract values according to the flag type
   * Special case for uint64: If subtrahend is larger than minuend, 0 is returned
   * \param[in] minuend
   * \param[in] subtrahend
   * \param[in] flag type
   * \return    the subtraction in string format, according to the flag type
   */
  static std::string SubtractFlagValues(const std::string& minuend, const std::string& subtrahend, TypeValues type);

  /**
   * \brief Command with all arguments and flags
   * \return string with the reconstructed command
   */
  std::string command(void) const;

private:
  /**
   * Access to flags
   */

  std::string GetFlagValue(const std::string& flag_name) const;
  TypeValues GetFlagType(const std::string& flag_name) const;

  /**
   * Functions to transform values
   */

  static int getIntValue(const std::string& value);
  static size_t getUint64Value(const std::string& value);
  static double getDoubleValue(const std::string& value);
  static bool getBoolValue(const std::string& value);

  // Internal function to parse content
  void ClearValues(void);
  void ParseTockens(std::vector<std::string> &tockens, bool strict = true);

  std::string command_;                                  // !<< Incoming General command
  std::map< std::string, CommandLineFlag > flags_;       // !<< Map of flags
  std::vector<std::string> arguments_;                   // !<< Command line arguments separated in "words"
};
}

#endif  // ifndef COMMAND_LINE_H
