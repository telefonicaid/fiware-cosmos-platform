
#include "au/ErrorManager.h"      // Own interface


namespace au {

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

}