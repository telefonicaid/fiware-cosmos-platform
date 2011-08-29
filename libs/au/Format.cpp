
#include "au/Format.h"		// Own interface
#include <sys/stat.h>	// stat(.)
#include <stdarg.h>             /* va_start, va_arg, va_end                  */
#include <string.h>             // strchr
#include <sstream>              // std::ostringstream
#include <dirent.h>
#include <string>

namespace au
{
    
    std::string F::tabs(int t)
    {
        std::ostringstream output;
        for ( int i = 0 ; i < t ; i ++)
            output << "\t";
        return output.str();
    }
   
    
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
		sprintf(line, "%05.1f%%",p*100);
		return std::string(line);
		
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

    std::string Format::progress_bar( double p , int len , char c , char c2 )
    {
        std::ostringstream output;
        
        
        if( p < 0 )
            p = 0;
        if( p > 1 )
            p = 1;
        
        int pos = len * p;
        
        output << " [ ";
        for (int s = 0 ; s < pos ; s++ )
            output << c;
        for (int s = pos ; s < len ; s++ )
            output << c2;
        output << " ] ";
        
        return  output.str();
    }    
    
    std::string Format::double_progress_bar( double p1 , double p2 , char c1 ,char c2 , char c3, int len )
    {
        std::ostringstream output;
        
        
        if( p1 < 0 )
            p1 = 0;
        if( p1 > 1 )
            p1 = 1;

        if( p2 < 0 )
            p2 = 0;
        if( p2 > 1 )
            p2 = 1;
        
        if( p2 < p1 )
            p2 = p1;    // no sense
        
        
        int pos1 = (double)len * p1;
        int pos2 = (double)len * p2;
        
        output << " [ ";
        
        for (int s = 0 ; s < pos1 ; s++ )
            output << c1;
        
        for (int s = pos1 ; s < pos2 ; s++ )
            output << c2;
        
        for (int s = pos2 ; s < len ; s++ )
            output << c3;
        
        output << " ] ";
        
        return  output.str();
    }
    
    void find_and_replace( std::string &source, const std::string find, std::string replace ) {
        size_t pos = 0;
        
        //LM_M(("Finding string of %d bytes at position %lu of a string with length %lu" , find.length() , pos , source.length() ));
        pos = source.find( find , pos );
        //LM_M(("Position found %lu bytes" , find.length() ));
        
        while(pos != std::string::npos )
        {
            source.replace( pos, find.length(), replace );

            // Go forward in the input string
            pos += replace.length();

            //LM_M(("Finding string of %d bytes at position %lu of a string with length %lu" , find.length() , pos , source.length() ));
            pos = source.find( find , pos );
            //LM_M(("Position found %lu bytes" , find.length() ));
            
            
        }
    }
    
    std::string Format::getRoot( std::string& path )
    {
        size_t pos = path.find( "." , 0 );
        
        if( pos == std::string::npos )
            return path;
        
        return path.substr( 0 , pos );
    }
    
    std::string Format::getRest( std::string& path )
    {
        size_t pos = path.find( "." , 0 );
        
        if( pos == std::string::npos )
            return "";
        
        return path.substr( pos+1 , path.length() );
    }    
    
    
    std::string indent( std::string txt )
    {
        // Replace all "returns" by "return and tab"
        find_and_replace( txt , "\n" , "\n\t" );
        
        // Insert the first tab
        txt.insert(0, "\t");
        return txt;
    }
    
    std::string indent( std::string txt , int num_spaces )
    {
        std::string sep;
        for (int i = 0 ; i < num_spaces ; i++ )
            sep.insert(0, " ");
        
        // Replace all "returns" by "return and tab"
        find_and_replace( txt , "\n" , "\n" + sep );
        
        // Insert the first tab
        txt.insert(0, sep);
        return txt;
        
    }
    
    
    // Working with directories and files
    
    
    bool isDirectory( std::string fileName ) 
    {
        struct stat buf;
        stat( fileName.c_str() , &buf );
        return S_ISDIR(buf.st_mode);
    }

    bool isRegularFile( std::string fileName ) 
    {
        struct stat buf;
        stat( fileName.c_str() , &buf );
        return S_ISREG(buf.st_mode);
    }
    
    void removeDirectory( std::string fileName ) 
    {
        if( isRegularFile( fileName ) )
        {
            // Just remove
            remove( fileName.c_str() );
            return;
        }
        
        // Navigate in the directory
        if( !isDirectory( fileName ) )
            return;
        
        // first off, we need to create a pointer to a directory
        DIR *pdir = opendir (fileName.c_str()); // "." will refer to the current directory
        struct dirent *pent = NULL;
        if (pdir != NULL) // if pdir wasn't initialised correctly
        {
            while ((pent = readdir (pdir))) // while there is still something in the directory to list
                if (pent != NULL)
                {
                    
                    if( strcmp( "." , pent->d_name ) )
                        break;
                    if( strcmp( ".." , pent->d_name ) )
                        break;
                    
                    std::ostringstream localFileName;
                    localFileName << fileName << "/" << pent->d_name;
                    
                    removeDirectory( localFileName.str() );
                    
                }
            // finally, let's close the directory
            closedir (pdir);						
        }
        
        // Remove the directory properly
        rmdir(fileName.c_str());
        
    }
    
    
    // Strings with format
    
    std::string str( double value, char  letter )
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
	
	
	std::string str( size_t value )
	{
		
		if (value < 1000)
			return au::str( (double)value , ' ' );
		else if( value < 1000000)
			return au::str( (double)value/ 1000.0 , 'K');
		else if( value < 1000000000)
			return au::str( (double)value/ 1000000.0 , 'M');
		else if( value < 1000000000000)
			return au::str( (double)value/ 1000000000.0 , 'G');
		else if( value < 1000000000000000)
			return au::str( (double)value/ 1000000000000.0 , 'T');
		else 
			return au::str( (double)value/ 1000000000000000.0 , 'P');
		
	}
	
	std::string str( size_t value , std::string postfix )
	{
		return str( value ) + postfix;
	}
    
    std::string str(const char* format, ...)
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
    
#pragma mark XML functions
    
    void xml_open( std::ostringstream& output , std::string name )
    {
        output << "<" << name << ">\n";
    }
    
    void xml_close( std::ostringstream& output , std::string name )
    {
        output << "</" << name << ">\n";
    }
	
}
