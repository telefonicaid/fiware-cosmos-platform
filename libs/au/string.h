
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
#include "au/map.h"                 // au::map

#include "au/namespace.h"


NAMESPACE_BEGIN(au)

typedef enum 
{
    normal,
    purple,
    red
}Color;

std::string tabs(int t);

std::string percentage_string( double value , double total );
std::string percentage_string( double p );
std::string int_string(int value, int digits);
std::string time_string( size_t seconds );


std::string time_string_ellapsed( struct timeval *init_time );	

std::string progress_bar( double p , int len );
std::string progress_bar( double p , int len, char c , char c2 );
std::string double_progress_bar( double p1 , double p2 , char c1 ,char c2 , char c3, int len );


std::string getRoot( std::string& path );
std::string getRest( std::string& path );

// Tokenize a string
std::vector<std::string> simpleTockenize( std::string txt );

// Check is char is one of the possible values
bool isOneOf( char c , std::string s );

// Getting strings with format
std::string str(const char* format, ...);
std::string str( size_t value );
std::string str_detail( size_t value );


// Strings in color
std::string str( Color color, const char* format, ...);


std::string str( size_t value , std::string postfix );
std::string str_detail( size_t value , std::string postfix );

std::string str_rate( size_t value , size_t time );
std::string str_rate( size_t value , size_t time , std::string postLabel );

// Indent a string text
std::string indent( std::string txt );
std::string indent( std::string txt , int num_spaces );

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);  

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

std::string string_in_color(std::string txt , std::string color );



NAMESPACE_END


#endif
