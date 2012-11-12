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

/* ****************************************************************************
 *
 * FILE            Enviroment.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *  Collection of enviroment variables. It is a key-value colection usign strings
 *  Convenient functions are provided to use values as integers, doubles, etc...
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_ENGINE_ENVIRONMENT
#define _H_ENGINE_ENVIRONMENT

#include <math.h>
#include <sstream>         // std::ostringstream
#include <map>			   // std::map
#include <stdlib.h>

#include "au/namespace.h"

NAMESPACE_BEGIN(au)

class Environment
{
    
public:
    
    std::map<std::string,std::string> environment;	
    
    void clearEnvironment();
    
    std::string get( std::string name , std::string default_value );
    void set( std::string name , std::string value );
    void unset( std::string name );
    bool isSet( std::string name );
    
    void copyFrom( Environment *other );
    
    template<typename T>
    void set( std::string name  , T value)
    {
        std::ostringstream v;
        v << value;
        set( name , v.str() ); 
    }	
    
    void setInt( std::string name  , int value);
    void setSizeT( std::string name  , size_t value);
    void setDouble( std::string name  , double value);
    
    int getInt( std::string name , int defaultValue);
    size_t getSizeT( std::string name , size_t defaultValue);
    double getDouble( std::string name , double defaultValue);
    
    void appendInt( std::string name , int value );
    void appendSizeT( std::string name , int value );
    
    // Description
    std::string toString();
    std::string getEnvironmentDescription();
    
    // Save and restore from string
    std::string saveToString();
    void recoverFromString( std::string input );
    
    
};	

NAMESPACE_END

#endif
