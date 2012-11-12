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
 * FILE            Descriptors.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *      Descriptors solves the situation where you have a list of strings where some
 *      of them are repeated. This helps you to group equal strings to display a unified message
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_AU_DESCRIPTORS
#define _H_AU_DESCRIPTORS

#include <string>
#include <sstream>
#include "au/containers/map.h"       // au::map

#include "au/namespace.h"

NAMESPACE_BEGIN(au)


class DescriptorsCounter
{
    std::string description;
    int counter;
    
public:
    
    DescriptorsCounter( std::string _description);
    void increase();
    std::string str();
    
};

class Descriptors
{
    
    au::map< std::string , DescriptorsCounter> concepts;
    
public:
    
    ~Descriptors();
    
    void add( std::string txt );

    size_t size();
    
    std::string str();
    
};

NAMESPACE_END

#endif