#ifndef SAMSON_FORMAT_H
#define SAMSON_FORMAT_H

#include <stdio.h>             /* sprintf */
#include <string>				// std::string
#include <sys/time.h>           // struct timeval
#include "logMsg/logMsg.h"             // LM_W

namespace au {

    // Fase methods
    class F
    {
        
    public:
        
        static std::string tabs(int t);
        
    };
    

	/**
	 Class to help the format of strings
	 */
	
	class Format
	{
		
	public:
        
		static std::string percentage_string( double value , double total );
		static std::string percentage_string( double p );
		static std::string int_string(int value, int digits);
		static std::string time_string( int seconds );
		
		static size_t sizeOfFile( std::string fileName );
		static int ellapsedSeconds(struct timeval *init_time);

		static std::string time_string_ellapsed( struct timeval *init_time );	
        
        static std::string progress_bar( double p , int len );
        static std::string progress_bar( double p , int len, char c , char c2 );
        static std::string double_progress_bar( double p1 , double p2 , char c1 ,char c2 , char c3, int len );
    
        
        static std::string getRoot( std::string& path );
        static std::string getRest( std::string& path );
        
	};

    
    // Getting strings with format
    std::string str(const char* format, ...);
    std::string str( size_t value );
    std::string str( size_t value , std::string postfix );

    
    
    // Indent a string text
    std::string indent( std::string txt );
    std::string indent( std::string txt , int num_spaces );

    
    // Working with files and directories
    void removeDirectory(std::string path);
    bool isDirectory(char path[]);
    bool isRegularFile(char path[]);
    
}

#endif
