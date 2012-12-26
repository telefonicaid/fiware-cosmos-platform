/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */


#include "au/containers/StringVector.h" // Own interface


namespace au {
    
    StringVector::StringVector()
    {
    }
    
    StringVector::StringVector( std::string value )
    {
        push_back(value);
    }
    
    StringVector::StringVector( std::string value , std::string value2 )
    {
        push_back(value);
        push_back(value2);
    }
    
    StringVector::StringVector( std::string value , std::string value2 , std::string value3 )
    {
        push_back(value);
        push_back(value2);
        push_back(value3);
    }
    
    StringVector::StringVector( std::string value , std::string value2 , std::string value3 , std::string value4 )
    {
        push_back(value);
        push_back(value2);
        push_back(value3);
        push_back(value4);
    }

    StringVector::StringVector( std::string value , std::string value2 , std::string value3 , std::string value4 , std::string value5 )
    {
        push_back(value);
        push_back(value2);
        push_back(value3);
        push_back(value4);
        push_back(value5);
    }

    StringVector::StringVector( std::string value , std::string value2 , std::string value3 , std::string value4 , std::string value5 , std::string value6  )
    {
        push_back(value);
        push_back(value2);
        push_back(value3);
        push_back(value4);
        push_back(value5);
        push_back(value6);
    }

    StringVector::StringVector( std::string value , std::string value2 , std::string value3 , std::string value4 , std::string value5 , std::string value6 , std::string value7  )
    {
        push_back(value);
        push_back(value2);
        push_back(value3);
        push_back(value4);
        push_back(value5);
        push_back(value6);
        push_back(value7);
    }
    
    StringVector::StringVector( std::set<std::string>& values )
    {
        std::set<std::string>::iterator it_values;
        for (it_values = values.begin() ; it_values != values.end() ; it_values++ )
            push_back(*it_values);
    }
    
    StringVector StringVector::parseFromString( std::string line , char separator )
    {
        StringVector string_vector;
        if( line != "" )
            au::split(line, separator , string_vector );
        return string_vector;
    }
    
    StringVector StringVector::StringVector::parseFromString( std::string line )
    {
        return parseFromString(line , ' ');
    }
    
    void StringVector::copyFrom( StringVector& values )
    {
        for ( size_t i = 0 ; i < values.size() ; i++ )
            push_back( values[i] );
    }
    
    std::string StringVector::get(size_t pos)
    {
        if( pos >= size() )
            return "";
        return (*this)[pos];
    }
    
    void StringVector::unique()
    {
        std::set<std::string> unique_values;
        for ( size_t i = 0 ; i < size() ; i++ )
            unique_values.insert( (*this)[i] );
        
        clear();
        std::set<std::string>::iterator it_unique_values;
        for ( it_unique_values = unique_values.begin() ; it_unique_values != unique_values.end() ; it_unique_values++ )
            push_back(*it_unique_values);
    }
    
    std::string StringVector::str()
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
    
    std::string StringVector::str_vector()
    {
        std::ostringstream output;
        output << "[ ";
        for ( size_t i = 0 ; i < size() ; i++ )
        {
            output << (*this)[i];
            output << " ";
        }
        output << "]";
        return output.str();
    }

}

