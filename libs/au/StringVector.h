

#ifndef _H_AU_STRING_VECTOR
#define _H_AU_STRING_VECTOR

#include <stdio.h>             /* sprintf */
#include <string>				// std::string
#include <sys/time.h>           // struct timeval
#include <sstream>              // std::ostringstream
#include "logMsg/logMsg.h"             // LM_W

#include "au/CommandLine.h"
#include "au/containers/simple_map.h"
#include "au/ErrorManager.h"        // au::ErrorManager
#include "au/containers/map.h"                 // au::map
#include "au/string.h"              // au::str(...)
#include "au/console/ConsoleAutoComplete.h"

#include "au/namespace.h"


NAMESPACE_BEGIN(au)

class StringVector : public std::vector<std::string>
{
public:
    
    StringVector();
    StringVector( std::string value );
    StringVector( std::string value , std::string value2 );
    StringVector( std::string value , std::string value2 , std::string value3 );
    StringVector( std::string value , std::string value2 , std::string value3 , std::string value4 );
    StringVector( std::string value , std::string value2 , std::string value3 , std::string value4 , std::string value5  );
    StringVector( std::string value , std::string value2 , std::string value3 , std::string value4 , std::string value5 , std::string value6);
    StringVector( std::string value , std::string value2 , std::string value3 , std::string value4 , std::string value5 , std::string value6 , std::string value7 );
    StringVector( std::set<std::string>& values );

    
    static StringVector parseFromString( std::string line , char separator );
    static StringVector parseFromString( std::string line );
    
    void copyFrom( StringVector& values );
    std::string get(size_t pos);
    
    void unique();
    
    // Add different values
    
    void push( size_t v )
    {
        push_back( au::str("%lu",v ) );
    }

    void push( char v )
    {
        push_back( au::str("%c",v ) );
    }
    
    std::string str();
    std::string str_vector();
};

class Uint64Vector : public std::vector<size_t>
{

public:
  
    Uint64Vector()
    {
    }
    
    Uint64Vector(  std::vector<size_t> v )
    {
        copyFrom(v);
    }
    
    size_t getNumberOfTimes( size_t t )
    {
        size_t num = 0;
        for ( size_t i = 0 ; i < size() ; i++)
            if( (*this)[i] == t )
                num++;
        return num;
    }
    
    std::set<size_t> getSet()
    {
        std::set<size_t> set;
        for ( size_t i = 0 ; i < size() ; i++)
            set.insert( (*this)[i] );
        return set;
    }

    void copyFrom(  std::vector<size_t> &v)
    {
        std::set<size_t> set;
        for ( size_t i = 0 ; i < v.size() ; i++)
            push_back( v[i] );
    }

};

NAMESPACE_END

#endif
