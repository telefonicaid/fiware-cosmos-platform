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

#include "au/ErrorManager.h"      // Own interface

namespace au {
    
    ErrorManager::ErrorManager()
    {
    }
    
    void ErrorManager::set( std::string message )
    {
        errors.push_back( new ErrorMessage( item_error , contexts , message ) );
    }
    
    void ErrorManager::add_error( std::string message )
    {
        errors.push_back( new ErrorMessage( item_error , contexts , message ) );
    }

    void ErrorManager::add_warning( std::string message )
    {
        errors.push_back( new ErrorMessage( item_warning , contexts , message ) );
    }

    void ErrorManager::add_message( std::string message )
    {
        errors.push_back( new ErrorMessage( item_message , contexts , message ) );
    }

    bool ErrorManager::isActivated()
    {
        for( size_t i = 0 ; i < errors.size() ; i++ )
            if( errors[i]->getType() == item_error )
                return true;
        return false;
    }
    
    std::string ErrorManager::getMessage()
    {
        // Get one line of the last error
        if( errors.size() == 0 )
            return "No errors";
        else
            return errors.back()->getMessage();
    }
    
    std::string ErrorManager::getCompleteMessage()
    {
        std::ostringstream output;

        for( size_t i = 0 ; i < errors.size() ; i++ )
            output << errors[i]->getMultiLineMessage();
        return output.str();
    }
    
    void ErrorManager::push_context( std::string context )
    {
        contexts.push_back( context );
        
    }
    
    void ErrorManager::pop_context( )
    {
        contexts.pop_back();
    }
    
    ErrorContext::ErrorContext( ErrorManager* _error , std::string context )
    {
        error = _error;
        error->push_context( context );
    }
    
    ErrorContext::~ErrorContext()
    {
        error->pop_context();
    }

    
}