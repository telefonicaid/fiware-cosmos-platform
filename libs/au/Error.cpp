
#include "Error.h"      // Own interface


namespace au {

    Error::Error()
    {
        error = false;
    }
    
    void Error::set( std::string _message )
    {
        error = true;
        message = _message;
    }
    
    bool Error::isActivated()
    {
        return error;
    }
    
    std::string Error::getMessage()
    {
        return message;
    }
    
    void Error::set( Error *otherError )
    {
        if( otherError->isActivated() )
            set( otherError->getMessage() );
    }
    
    std::string Error::str()
    {
        if( !error )
            return "No error";
        else
            return "Error:" + message;
    }

}