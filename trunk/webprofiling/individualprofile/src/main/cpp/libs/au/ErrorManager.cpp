
#include "au/ErrorManager.h"      // Own interface

NAMESPACE_BEGIN(au)

ErrorManager::ErrorManager()
{
    error = false;
}

void ErrorManager::set( std::string _message )
{
    error = true;
    message = _message;
}

bool ErrorManager::isActivated()
{
    return error;
}

std::string ErrorManager::getMessage()
{
    return message;
}

void ErrorManager::set( ErrorManager *otherError )
{
    if( otherError->isActivated() )
        set( otherError->getMessage() );
}

std::string ErrorManager::str()
{
    if( !error )
        return "No error";
    else
        return "Error:" + message;
}

void ErrorManager::getInfo( std::ostringstream& output)
{
    output << "<error>";
    
    if( error )
        output << "<activated>YES</activated>\n";
    else
        output << "<activated>NO</activated>\n";
    
    output << "<message>" << message << "</message>";
    
    output << "</error>";
    
}

NAMESPACE_END