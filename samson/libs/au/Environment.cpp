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

#include "au/string.h"          // au::split

#include "Environment.h"        // OWn interface

NAMESPACE_BEGIN(au)


void Environment::clearEnvironment()
{
    environment.clear();
}

std::string Environment::get( std::string name , std::string default_value )
{
    std::map<std::string, std::string>::iterator iter = environment.find(name);
    
    if( iter == environment.end() )
        return default_value;
    else
        return iter->second;
}

void Environment::set( std::string name , std::string value )
{
    std::map<std::string, std::string>::iterator iter = environment.find(name);
    
    if( iter == environment.end() )
        environment.insert ( std::pair<std::string, std::string>( name , value ) );
    else
        iter->second = value;
    
}

void Environment::unset( std::string name )
{
    std::map<std::string, std::string>::iterator iter = environment.find(name);
    
    if( iter != environment.end() )
        environment.erase(iter );
    
}


bool Environment::isSet( std::string name )
{
    std::map<std::string, std::string>::iterator iter = environment.find(name);
    
    if( iter == environment.end() )
        return false;
    else
        return true;
    
}

void Environment::copyFrom( Environment *other )
{
    std::map<std::string, std::string>::iterator iter;
    for ( iter = other->environment.begin() ; iter != other->environment.end() ; iter++)
        environment.insert ( std::pair<std::string, std::string>( iter->first , iter->second ) );
}


std::string Environment::toString()
{
    std::ostringstream o;
    
    std::map<std::string, std::string>::iterator iter;
    for (iter = environment.begin() ; iter != environment.end() ; iter++)
        o << iter->first << " : " << iter->second << "\n";
    return o.str();
}


void Environment::appendInt( std::string name , int value )
{
    int current_value = getInt( name , 0 );
    current_value += value;
    setInt( name , current_value ); 
    
}

void Environment::appendSizeT( std::string name , int value )
{
    size_t current_value = getSizeT( name , 0 );
    current_value += value;
    setSizeT( name , current_value ); 
    
}

void Environment::setInt( std::string name  , int value)
{
    std::ostringstream v;
    v << value;
    set( name , v.str() ); 
}	
void Environment::setSizeT( std::string name  , size_t value)
{
    std::ostringstream v;
    v << value;
    set( name , v.str() ); 
}	

void Environment::setDouble( std::string name  , double value)
{
    std::ostringstream v;
    v << value;
    set( name , v.str() ); 
}	


int Environment::getInt( std::string name , int defaultValue)
{
    if( !isSet(name) )
        return defaultValue;
    return atoi( get( name , "0" ).c_str() );
}

size_t Environment::getSizeT( std::string name , size_t defaultValue)
{
    if( !isSet(name) )
        return defaultValue;
    return atoll( get( name , "0" ).c_str() );
}

double Environment::getDouble( std::string name , double defaultValue)
{
    if( !isSet(name) )
        return defaultValue;
    return atof( get( name , "0" ).c_str() );
}

// Description

std::string Environment::getEnvironmentDescription()
{
    std::ostringstream output;
    output << "(";
    for( std::map<std::string,std::string>::iterator i = environment.begin() ; i != environment.end() ; )	
    {
        output << i->first << "=" << i->second;
        i++;
        if ( i!= environment.end() )
            output << ",";
    }
    output << ")";
    
    return output.str();
}

// Save and recover from string
std::string Environment::saveToString()
{
    std::ostringstream output;
    
    std::map<std::string,std::string>::iterator e;
    for ( e = environment.begin() ; e != environment.end() ;  )
    {
        output << e->first << "=" << e->second;
        e++;
        if( e != environment.end() )
            output << ",";
    }
    
    return output.str();
}

void Environment::recoverFromString(std::string input)
{
    std::vector<std::string> values;
    split(input, ',', values);
    
    for ( size_t v = 0 ; v < values.size() ; v++ )
    {
        std::vector<std::string> property_value;
        split(values[v], '=', property_value);
        
        if( property_value.size() == 2 )
            set( property_value[0] , property_value[1] );
    }
    
}

NAMESPACE_END