
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

namespace au {
    
    
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
    std::string str( size_t value , std::string postfix );
    
    // Indent a string text
    std::string indent( std::string txt );
    std::string indent( std::string txt , int num_spaces );
    
    std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
    std::vector<std::string> split(const std::string &s, char delim);  
        
}


#endif