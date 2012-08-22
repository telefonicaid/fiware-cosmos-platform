
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
* The advantadge of this class is that is supports running-time definiton of possible flags.
*
*
* Example: command arg1 -f input.txt -g 2 arg2
*
* Depending on how you define flags "f" and "g" this will parse in a different way
*
* You can define flags as "bool", "double", "int" "string" "uint64".
* The properties of each element are:
*   bool:       It is a true/false flag. It does not take any parameter
*   int:        Takes an interguer parameter with it.   Example: -n 12
*   double:     Takes a double parameter.               Example: -threshold 5.67
*   uint64:     Takes an unsigned parameter.            Example: -size 12G , -size 100
*   string:     Takes an atring parameter.              Example: -file andreu.txt
*
* The basic usage has three steps:
*   * Use command set_flag_X( ) methods to define flags and types
*   * Use one of the parse(.) methods to parse the incoming command line
*   * Use get_flag_X methods to access content *
*
* COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
*
* ****************************************************************************/

#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

#include <cstdlib>
#include <map>
#include <sstream>
#include <stdio.h>             // fprintf, stderr, ...
#include <string>
#include <vector>



namespace au {
typedef struct CommandLineFlag {
  std::string type;
  std::string default_value;
  std::string value;
} CommandLineFlag;

class CommandLine {
public:

  /**
   * Simple constructor
   */

  CommandLine() {
  }

  /**
   * Constructor with the provided string or argument list
   * In this case no flags are considered
   */

  CommandLine(std::string _command);
  CommandLine(int argc, const char *argv[]);

  /**
   * Parse a command line in the classical C-style way
   */

  void Parse(int args, const char *argv[]);
  void Parse(std::string _command);

  /**
   * Defining flags
   */

  void ResetFlags();

  void SetFlagBoolean(std::string name);
  void SetFlagInt(std::string name, int default_value);
  void SetFlagString(std::string name, std::string default_value);
  void SetFlagUint64(std::string name, std::string default_value);
  void SetFlagUint64(std::string name, size_t default_value);
  void SetFlagDouble(std::string name, double default_value);

  /**
   * Acces information about arguments
   */

  int get_num_arguments();
  std::string get_argument(int index);

  /**
   * Specialed access to parametes
   */

  bool GetFlagBool(std::string flag_name);
  int GetFlagInt(std::string flag_name);
  double GetFlagDouble(std::string flag_name);
  std::string GetFlagString(std::string flag_name);
  size_t GetFlagUint64(std::string flag_name);

  // Original command+
  std::string command();

private:

  /**
   * Access to flags
   */

  std::string GetFlagValue(std::string flag_name);
  std::string GetFlagType(std::string flag_name);

  /**
   * Functions to transform values
   */

  static int getIntValue(std::string value);
  static size_t getUint64Value(std::string value);
  static double getDoubleValue(std::string value);
  static bool getBoolValue(std::string value);

  // Internal function to parse content
  void ClearValues();
  void ParseTockens(std::vector<std::string> &tockens);

  std::string command_;                                                                          // !<< Incoming General command
  std::map< std::string, CommandLineFlag > flags_;       // !<< Map of flags
  std::vector<std::string> arguments_;                                                           // !<< Command line arguments separated in "words"
};
}

#endif  // ifndef COMMAND_LINE_H
