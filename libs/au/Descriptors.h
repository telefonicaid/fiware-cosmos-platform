

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
 *      of them are the same. This helps you to group equal strings to display a unified message
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_AU_DESCRIPTORS
#define _H_AU_DESCRIPTORS

#include <string>
#include <sstream>
#include "au/map.h"       // au::map

namespace au {
    
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
        
        std::string str();
        
    };
}

#endif