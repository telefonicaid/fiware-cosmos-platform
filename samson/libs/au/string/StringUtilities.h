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
* FILE            string.h
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            Septembre 2011
*
* DESCRIPTION
*
*      useful functions to work with strings
*
* ****************************************************************************/

#ifndef _H_AU_STRING
#define _H_AU_STRING

#include "logMsg/logMsg.h"     // LOG_SW
#include <set>
#include <sstream>             // std::ostringstream
#include <stdio.h>             /* sprintf */
#include <string>              // std::string
#include <sys/time.h>          // struct timeval
#include <vector>

#include "au/CommandLine.h"
#include "au/ErrorManager.h"   // au::ErrorManager
#include "au/containers/map.h"  // au::map


namespace au {
enum Color {
  Normal,
  Black,
  Red,
  Green,
  Yellow,
  Blue,
  Magenta,
  Cyan,
  White,
  BoldBlack,
  BoldRed,
  BoldGreen,
  BoldYellow,
  BoldBlue,
  BoldMagenta,
  BoldCyan,
  BoldWhite,
};

// ------------------------------------------------------------------
// Formating test
// ------------------------------------------------------------------

// String with some tabs
std::string str_tabs(int t);

// String with time information
std::string str_time(size_t seconds);
std::string str_time_simple(size_t seconds);
std::string str_timestamp(time_t t);

// String with percentdge information
std::string str_percentage(double value, double total);
std::string str_simple_percentage(double value, double total);
std::string str_percentage(double p);
std::string str_simple_percentage(double p);

// Getting strings with format
std::string str(const char *format, ...);
std::string str(double value);
std::string str_detail(size_t value);

std::string str(const std::vector<std::string>& hosts);
std::string str_grouped(const std::vector<std::string>& names);


// Strings in color
std::string str(Color color, const char *format, ...);
std::string str(Color color, const std::string& message);

// String with size_t content
std::string str(double value, const std::string& postfix);
std::string str_detail(size_t value, const std::string& postfix);

// Indent a string text
std::string str_indent(const std::string& txt);
std::string str_indent(const std::string& txt, int num_spaces);

// Add color escape codes to a string
std::string string_in_color(const std::string& txt, const std::string& color);

// Get color code by name
Color GetColor(const std::string color_name);

// ------------------------------------------------------------------
// Tokenization of strings
// ------------------------------------------------------------------

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);
std::vector<std::string> split_using_multiple_separators(const std::string& input, const std::string& separators);
std::vector<char *> SplitInWords(char *line, char separator);

// ------------------------------------------------------------------
// Checking content of trings
// ------------------------------------------------------------------

// Check is char is one of the possible values
bool IsOneOf(char c, const std::string& s);

// Get common number of chars
int GetCommonChars(const std::string& txt, const std::string& txt2);

// Check if char is in range
bool IsCharInRange(char c, char lower, char higher);

// Checks prefix and postfix of strings
bool CheckIfStringsBeginWith(const std::string& str, const std::string& prefix);
bool CheckIfStringsEndsWith(const std::string& str, const std::string& postfix);
bool CheckIfStringBeginsAndEndsWith(const std::string& str, const std::string& prefix, const std::string& postfix);

// ------------------------------------------------------------------
// Console string commands
// ------------------------------------------------------------------

/**
 * \brief Get the width of the terminal
 */
int GetTerminalWidth();

/**
 * \brief Clear current line in the terminal
 */
void ClearTerminalLine();


// Get a string where every line has a maximum length ( "..." is added if more length
std::string StringWithMaxLineLength(const std::string& txt, int max_line_length);

// Get a string with lines that fit into console...
std::string StringInConsole(const std::string& txt);

// Get a line with the size of console
std::string StringRepeatingCharInConsole(char c);

// Get information about terminal size
int GetTerminalSize(int fd, int *x, int *y);

// ------------------------------------------------------------------
// Search in strings
// ------------------------------------------------------------------

// Backward search of sequences
const char *laststrstr(const char *source, const char *target);
const char *laststrstr(const char *source, size_t source_length, const char *target);

// Extract substrings (patterns) from a line
const char *strnstr_limitpattern(const char *text, const char *pattern, size_t max_length);

// Check simple regular expressions in string
bool MatchPatterns(const char *inputString, const char *pattern, char wildcard);

// Find and replace
void FindAndReplaceInString(std::string &source, const std::string& find, const std::string& replace);

/**
 * \brief Remove trailing spaces, tabs at the beginning or end of the command and spaces,tabs and return at the end
 */

std::string StripString(const std::string& line);


// ------------------------------------------------------------------
// Miscelania
// ------------------------------------------------------------------

// Reverse order of lines
std::string ReverseLinesOrder(const std::string& txt);

// hash function (same as the method in system::String and system::Value
int HashString(const std::string& str, int max_num_partitions);

// Progress bar
std::string str_progress_bar(double p, int len);
std::string str_progress_bar(double p, int len, char c, char c2);
std::string str_double_progress_bar(double p1, double p2, char c1, char c2, char c3, int len);


// Transform a vector of things to string
template <typename C>
std::string str(const std::vector<C>& vector) {
  std::ostringstream output;

  output << "[ ";
  for (size_t i = 0; i < vector.size(); i++) {
    output << vector[i] << " ";
  }
  output << "]";
  return output.str();
}

template<typename C>
std::string str(const std::set<C>& elements) {
  std::ostringstream output;

  output << "[ ";
  typename std::set<C>::const_iterator it;
  for (it = elements.begin(); it != elements.end(); ++it) {
    output << *it << " ";
  }
  output << "]";
  return output.str();
}

template<typename C>
std::string str(const std::list<C>& elements) {
  std::ostringstream output;

  output << "[ ";
  typename std::list<C>::const_iterator it;
  for (it = elements.begin(); it != elements.end(); ++it) {
    output << *it << " ";
  }
  output << "]";
  return output.str();
}
}


#endif  // ifndef _H_AU_STRING
