
#include "au/log/Log.h"
#include "LogFormatter.h" // Own interface

namespace au 
{
    
    LogFormatter::LogFormatter( std::string definition )
    {
        au::token::TokenVector token_vector;
        
        // Att all reserved words
        size_t i = 0;
        while( log_reseved_words[i] != NULL )
            token_vector.addToken( log_reseved_words[i++] );
        
        
        
        token_vector.parse( definition );
        
        
        au::token::Token* token = token_vector.getNextToken();
        while( token )
        {
            fields.push_back( token->content );
            token_vector.popToken();
            token = token_vector.getNextToken();
        }
    }
    
    std::string LogFormatter::get( Log* log )
    {
        std::string output;
        for( size_t i = 0 ; i < fields.size() ; i++ )
            output.append(  log->get( fields[i] ) );
        return output;
    }
    
}