
#include "au/Format.h"		// Own interface
#include <sys/stat.h>	// stat(.)
#include <stdarg.h>             /* va_start, va_arg, va_end                  */
#include <string.h>             // strchr
#include <sstream>              // std::ostringstream

namespace au
{
	std::string Format::percentage_string( double value , double total )
	{
		if ( total == 0)
			return percentage_string( 0 );
		else
			return percentage_string( value / total);
	}
	
	std::string Format::percentage_string( double p )
	{
		char line[2000];
		sprintf(line, "%04.1f%%",p*100);
		return std::string(line);
		
	}
	
	std::string Format::string( double value, char  letter )
	{
		char line[2000];
		
		if ( value < 10 )
			sprintf(line, "%4.2f%c",value,letter);
		else if ( value < 100 )
			sprintf(line, "%4.1f%c",value,letter);
		else 
			sprintf(line, "%4.0f%c",value,letter);
		
		
		return std::string( line );
	}
	
	
	std::string Format::string( size_t memory )
	{
		
		if (memory < 1000)
			return string( (double)memory , ' ' );
		else if( memory < 10000000)
			return string( (double)memory/ 1000.0 , 'K');
		else if( memory < 1000000000)
			return string( (double)memory/ 1000000.0 , 'M');
		else 
			return string( (double)memory/ 1000000000.0 , 'G');
		
	}
	
	std::string Format::string( size_t memory , std::string postfix )
	{
		
		if (memory < 1000)
			return string( (double)memory , ' ' ) + postfix;
		else if( memory < 10000000)
			return string( (double)memory/ 1000.0 , 'K')+ postfix;
		else if( memory < 1000000000)
			return string( (double)memory/ 1000000.0 , 'M')+ postfix;
		else 
			return string( (double)memory/ 1000000000.0 , 'G')+ postfix;
		
	}
	
	
	std::string Format::int_string(int value, int digits)
	{
		
		char line[100];
		char format[100];
		
		
		sprintf(format, "%%%dd",digits);
		sprintf(line, format,value ); 
		return std::string(line);
	}
	
	std::string Format::time_string( int seconds )
	{
		int minutes = seconds/60;
		seconds -= minutes*60;
		
		int hours = minutes/60;
		minutes -= hours*60;
		
		char line[1000];
		sprintf(line, "%02d:%02d:%02d" , hours , minutes , seconds	);
		return std::string(line);
		
	}
	
	
	size_t Format::sizeOfFile( std::string fileName )
	{
		struct ::stat info;
		if( stat(fileName.c_str(), &info) == 0)
			return info.st_size;
		else
		{
			return 0;
		}
	}		
	
	int Format::ellapsedSeconds(struct timeval* init_time)
	{
		struct timeval finish_time;
		gettimeofday(&finish_time, NULL);
		return finish_time.tv_sec - init_time->tv_sec;
	}
	
	std::string Format::time_string_ellapsed( struct timeval *init_time )
	{
		return time_string( ellapsedSeconds( init_time ) );
	}
	
    std::string Format::string(const char* format, ...)
    {
        va_list        args;
        char           vmsg[2048];
        
        /* "Parse" the varible arguments */
        va_start(args, format);
        
        /* Print message to variable */
        vsnprintf(vmsg, sizeof(vmsg), format, args);
        //vmsg[2047] = 0;
        va_end(args);
        
        return std::string(vmsg);
    }        
    
    std::string Format::progress_bar( double p , int len )
    {
        std::ostringstream output;
    
        
        if( p < 0 )
            p = 0;
        if( p > 1 )
            p = 1;

        int pos = len * p;
        
        output << " [ ";
        for (int s = 0 ; s < pos ; s++ )
        output << "*";
        for (int s = pos ; s < len ; s++ )
        output << ".";
        output << " ] ";
        
        return  output.str();
    }
    
    
	
}
