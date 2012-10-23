
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
*      Usefull functions to work with strings
*
* COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
*
* ****************************************************************************/

#ifndef _H_AU_STRING
#define _H_AU_STRING

#include "logMsg/logMsg.h"     // LM_W
#include <sstream>             // std::ostringstream
#include <stdio.h>             /* sprintf */
#include <string>              // std::string
#include <sys/time.h>          // struct timeval

#include "au/CommandLine.h"
#include "au/ErrorManager.h"   // au::ErrorManager
#include "au/containers/map.h"  // au::map




namespace au {
typedef enum {
  normal,
  purple,
  red
}Color;


// String with some tabs
std::string str_tabs(int t);

// String with time information
std::string str_time(size_t seconds);
std::string str_time_simple(size_t seconds);
std::string str_timestamp(time_t t);

// String with percentdge information
std::string str_percentage(double value, double total);
std::string str_percentage(double p);

// Progress bar
std::string str_progress_bar(double p, int len);
std::string str_progress_bar(double p, int len, char c, char c2);
std::string str_double_progress_bar(double p1, double p2, char c1, char c2, char c3, int len);


// Tokenize and split a string
std::vector<std::string> simpleTockenize(const std::string& txt);
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);
std::vector<std::string> split_using_multiple_separators(const std::string& input, const std::string& separators);
void SplitInWords(char *line, std::vector<char *>& words, char separator);


// Old functions to be reviews
std::string getRoot(const std::string& path);
std::string getRest(const std::string& path);


// Check is char is one of the possible values
bool isOneOf(char c, const std::string& s);

// Getting strings with format
std::string str(const char *format, ...);
std::string str(double value);
std::string str_detail(size_t value);

std::string str(const std::vector<std::string>& hosts);

// Strings in color
std::string str(Color color, const char *format, ...);

// String with size_t content
std::string str(double value, const std::string& postfix);
std::string str_detail(size_t value, const std::string& postfix);

// Indent a string text
std::string str_indent(const std::string& txt);
std::string str_indent(const std::string& txt, int num_spaces);

// Get the width of the terminal
int getTerminalWidth();

// Get a string where every line has a maximum length ( "..." is added if more length
std::string strWithMaxLineLength(const std::string& txt, int max_line_length);
// Get a string with lines that fit into console...
std::string strToConsole(const std::string& txt);

// Get a line with the size of console
std::string lineInConsole(char c);

// Backward search of sequences
const char *laststrstr(const char *source, const char *target);
const char *laststrstr(const char *source, size_t source_length, const char *target);

// Extract substrings (patterns) from a line
const char *strnstr_limitpattern(const char *text, const char *pattern, size_t max_length);

// Check simple regular expressions in string
bool MatchPatterns(const char *inputString, const char *pattern, char wildcard);

// Get common number of chars
int getCommonChars(const std::string& txt, const std::string& txt2);

// Check if char is in range
bool isCharInRange(char c, char lower, char higher);

// Get information about terminal size
int get_term_size(int fd, int *x, int *y);

// Get a fill path from a directory and file name
std::string path_from_directory(const std::string& directory, const std::string& file);

// Find and replace
void find_and_replace(std::string &source, const std::string& find, const std::string& replace);

// Add color escape codes to a string
std::string string_in_color(const std::string& txt, const std::string& color);

// Checks prefix and postfix of strings
bool strings_begin_equal(const std::string& txt, const std::string& txt2);
bool string_begins(const std::string& str, const std::string& prefix);
bool string_ends(const std::string& str, const std::string& postfix);
bool string_begins_and_ends(const std::string& str, const std::string& prefix, const std::string& postfix);
std::string substring_without_prefix_and_posfix(const std::string& str, const std::string& prefix, const std::string& postfix);


// Reverse order of lines
std::string reverse_lines(const std::string& txt);

// hash function (same as the method in system::String and system::Value
int HashString(const std::string& str, int max_num_partitions);

// Trasnform a vector of things to string
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
}


#endif // ifndef _H_AU_STRING
