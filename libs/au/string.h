
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

#include <stdio.h>             /* sprintf */
#include <string>				// std::string
#include <sys/time.h>           // struct timeval
#include <sstream>              // std::ostringstream
#include "logMsg/logMsg.h"             // LM_W

#include "au/CommandLine.h"
#include "au/ErrorManager.h"        // au::ErrorManager
#include "au/containers/map.h"                 // au::map

#include "au/namespace.h"


NAMESPACE_BEGIN(au)

typedef enum 
{
    normal,
    purple,
    red
}Color;


// String with some tabs
std::string str_tabs(int t);

// String with time information
std::string str_time( size_t seconds );
std::string str_time_simple( size_t seconds );
std::string str_timestamp( time_t t );

// String with percentdge information
std::string str_percentage( double value , double total );
std::string str_percentage( double p );

// Progress bar
std::string str_progress_bar( double p , int len );
std::string str_progress_bar( double p , int len, char c , char c2 );
std::string str_double_progress_bar( double p1 , double p2 , char c1 ,char c2 , char c3, int len );


// Tokenize and split a string
std::vector<std::string> simpleTockenize( std::string txt );
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);  


// Old functions to be reviews
std::string getRoot( std::string& path );
std::string getRest( std::string& path );


// Check is char is one of the possible values
bool isOneOf( char c , std::string s );

// Getting strings with format
std::string str(const char* format, ...);
std::string str( double value );
std::string str_detail( size_t value );


// Strings in color
std::string str( Color color, const char* format, ...);

// String with size_t content 
std::string str( double value , std::string postfix );
std::string str_detail( size_t value , std::string postfix );

// Indent a string text
std::string str_indent( std::string txt );
std::string str_indent( std::string txt , int num_spaces );

// Get the width of the terminal
int getTerminalWidth();

// Get a string where every line has a maximum length ( "..." is added if more length
std::string strWithMaxLineLength( std::string& txt , int max_line_length );
// Get a string with lines that fit into console...
std::string strToConsole( std::string& txt );

// Get a line with the size of console
std::string lineInConsole( char c );

// Backward search of sequences
const char *laststrstr(const char *source , const char *target );
const char *laststrstr(const char *source , size_t source_length , const char *target );

// Get common number of chars
int getCommonChars( std::string& txt , std::string& txt2 );    

// Check if char is in range
bool isCharInRange( char c , char lower , char higher );

//Check if two string begin with the same chars..
bool strings_begin_equal( std::string txt , std::string txt2 );

// Get information about terminal size
int get_term_size (int fd, int *x, int *y);

// Get a fill path from a directory and file name
std::string path_from_directory( std::string directory , std::string file );

// Find and replace 
void find_and_replace( std::string &source, const std::string find, std::string replace );

// Add color escape codes to a string
std::string string_in_color(std::string txt , std::string color );

// Checks prefix and postfix of strings
bool string_begins( std::string& str , std::string prefix );
bool string_ends( std::string& str , std::string postfix );
bool string_begins_and_ends( std::string& str , std::string prefix , std::string postfix );
std::string substring_without_prefix_and_posfix( std::string& str , std::string prefix , std::string postfix );


// Reverse order of lines
std::string reverse_lines( std::string& txt );


NAMESPACE_END


#endif
