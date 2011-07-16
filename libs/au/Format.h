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
		static std::string string( double value, char  letter );
		static std::string string( size_t memory );
		static std::string string( size_t memory , std::string postfix );
		static std::string int_string(int value, int digits);
		static std::string time_string( int seconds );
		
		static size_t sizeOfFile( std::string fileName );
		static int ellapsedSeconds(struct timeval *init_time);

		static std::string time_string_ellapsed( struct timeval *init_time );	
        static std::string string(const char* format, ...);
        
        static std::string progress_bar( double p , int len );
        static std::string progress_bar( double p , int len, char c , char c2 );
        static std::string double_progress_bar( double p1 , double p2 , char c1 ,char c2 , char c3, int len );
    
        static std::string indent( std::string txt );
        
        
        static std::string getRoot( std::string& path );
        static std::string getRest( std::string& path );
        
        
	};
    
}

#endif
