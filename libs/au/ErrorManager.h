
/* ****************************************************************************
 *
 * FILE            ErrorManager.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 * Simple but usefull error mesasage information
 * Whatever sistem that can report an error can activate this error providing a message
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_AU_ERROR
#define _H_AU_ERROR

#include <set>
#include <sstream>		// std::ostringstream

#include "au/au_namespace.h"


NAMESPACE_BEGIN(au)

class ErrorManager
{
    bool error;
    std::string message;
    
public:
    
    ErrorManager();
    
    void set( std::string _message );
    
    bool isActivated();
    std::string getMessage(); 
    
    void set( ErrorManager *otherError );
    
    std::string str(); 
    
    // Get XML info for debuggin
    void getInfo( std::ostringstream& output);
    
};

NAMESPACE_END


#endif
