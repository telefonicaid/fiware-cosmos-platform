

#ifndef _H_AU_STRING_VECTOR
#define _H_AU_STRING_VECTOR

#include <stdio.h>             /* sprintf */
#include <string>				// std::string
#include <sys/time.h>           // struct timeval
#include <sstream>              // std::ostringstream
#include "logMsg/logMsg.h"             // LM_W

#include "au/CommandLine.h"
#include "au/simple_map.h"
#include "au/ErrorManager.h"        // au::ErrorManager
#include "au/map.h"                 // au::map
#include "au/string.h"              // au::str(...)
#include "au/ConsoleAutoComplete.h"

#include "au/namespace.h"


NAMESPACE_BEGIN(au)

class StringVector : public std::vector<std::string>
{
public:
    
    StringVector()
    {
    }
    
    StringVector( std::string value )
    {
        push_back(value);
    }
    
    StringVector( std::string value , std::string value2 )
    {
        push_back(value);
        push_back(value2);
    }
    
    StringVector( std::string value , std::string value2 , std::string value3 )
    {
        push_back(value);
        push_back(value2);
        push_back(value3);
    }
    
    StringVector( std::string value , std::string value2 , std::string value3 , std::string value4 )
    {
        push_back(value);
        push_back(value2);
        push_back(value3);
        push_back(value4);
    }
    
    StringVector( std::set<std::string>& values )
    {
        std::set<std::string>::iterator it_values;
        for (it_values = values.begin() ; it_values != values.end() ; it_values++ )
            push_back(*it_values);
    }
    
    static StringVector parseFromString( std::string line , char separator )
    {
        StringVector string_vector;
        if( line != "" )
            au::split(line, separator , string_vector );
        return string_vector;
    }
    
    static StringVector parseFromString( std::string line )
    {
        return parseFromString(line , ' ');
    }
    
    void copyFrom( StringVector& values )
    {
        for ( size_t i = 0 ; i < values.size() ; i++ )
            push_back( values[i] );
    }
    
    std::string get(size_t pos)
    {
        if( pos >= size() )
            return "";
        return (*this)[pos];
    }
    
    void unique()
    {
        std::set<std::string> unique_values;
        for ( size_t i = 0 ; i < size() ; i++ )
            unique_values.insert( (*this)[i] );
        
        clear();
        std::set<std::string>::iterator it_unique_values;
        for ( it_unique_values = unique_values.begin() ; it_unique_values != unique_values.end() ; it_unique_values++ )
            push_back(*it_unique_values);
    }
    
    std::string str()
    {
        std::ostringstream output;
        
        for ( size_t i = 0 ; i < size() ; i++ )
        {
            output << (*this)[i];
            if( i != ( size() - 1 ) )
                output << " ";
        }
        return output.str();
    }
    
};

NAMESPACE_END

#endif
