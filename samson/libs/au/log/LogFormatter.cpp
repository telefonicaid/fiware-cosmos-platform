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

#include "au/log/Log.h"
#include "LogFormatter.h" // Own interface

namespace au 
{
    
    LogFormatter::LogFormatter( std::string _definition )
    {
        au::token::Tokenizer tokenizer;
     
        // Keep a copy of the definition string
        definition = _definition;
        
        // Att all reserved words
        size_t i = 0;
        while( log_reseved_words[i] != NULL )
            tokenizer.addToken( log_reseved_words[i++] );
        
        // Parse to get the tokens
        au::token::TokenVector token_vector = tokenizer.parse( definition );
        
        au::token::Token* token = token_vector.getNextToken();
        while( token )
        {
            fields.push_back( token->content );
            token_vector.popToken();
            token = token_vector.getNextToken();
        }
        
        LM_VV(("LogFormatter %s %lu fields" , definition.c_str() , fields.size() ));
        
    }
    
    std::string LogFormatter::get( Log* log )
    {
        
        if( definition == "all" )
            return log->str();
        
        std::string output;
        for( size_t i = 0 ; i < fields.size() ; i++ )
            output.append(  log->get( fields[i] ) );
        return output;
    }
    
}