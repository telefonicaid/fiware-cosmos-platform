#ifndef SAMSON_FORMAT_H
#define SAMSON_FORMAT_H

#include <stdio.h>             /* sprintf */
#include <string>				// std::string
#include <sys/time.h>           // struct timeval
#include <sstream>              // std::ostringstream
#include "logMsg/logMsg.h"             // LM_W

#include "au/ErrorManager.h"        // au::ErrorManager
#include "au/map.h"                 // au::map

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

    
    // Working with files and directories
    void removeDirectory( std::string fileName , au::ErrorManager & error ); 
    bool isDirectory(char path[]);
    bool isRegularFile(char path[]);
 
    // Simple xml tags
    
    void xml_open( std::ostringstream& output , std::string name );
    void xml_close( std::ostringstream& output , std::string name );

    template< typename T>
    void xml_simple( std::ostringstream& output , std::string name , T value )
    {
        output << "<" << name << ">";
        output << value;
        output << "</" << name << ">";
    }

    template< typename T>
    void xml_single_element( std::ostringstream& output , std::string name , T* obj )
    {
        xml_open( output , name );
        obj->getInfo( output );
        xml_close( output , name );
    }
    
    
    template< typename T>
    void xml_iterate_list( std::ostringstream& output , std::string name , T& vector )
    {
        au::xml_open(output, name);
        
		//typename std::map<K, V* >::iterator iter;
		typename T::iterator iter;
        
        for (iter = vector.begin() ; iter != vector.end() ; iter++)
            (*iter)->getInfo( output );

        au::xml_close(output, name);
        
    }

    template< typename T>
    void xml_iterate_list_object( std::ostringstream& output , std::string name , T& vector )
    {
        au::xml_open(output, name);
        
		//typename std::map<K, V* >::iterator iter;
		typename T::iterator iter;
        
        for (iter = vector.begin() ; iter != vector.end() ; iter++)
            (iter)->getInfo( output );
        
        au::xml_close(output, name);
        
    }
    
    template< typename K, typename V>
    void xml_iterate_map( std::ostringstream& output , std::string name , au::map< K , V >& vector )
    {
        au::xml_open(output, name);
        
		//typename std::map<K, V* >::iterator iter;
		typename au::map< K , V >::iterator iter;
        
        for (iter = vector.begin() ; iter != vector.end() ; iter++)
            iter->second->getInfo( output );
        
        au::xml_close(output, name);
        
    }
    
    
    
    
    
}

#endif
