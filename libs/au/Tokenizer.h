
/* ****************************************************************************
 *
 * FILE            Tokenizer.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            2012
 *
 * DESCRIPTION
 *
 * Classes used to tokenize a string with a particular command
 *
 * COPYRIGTH       Copyright 2012 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/


#ifndef _H_AU_TOKENIZER
#define _H_AU_TOKENIZER

#include <string>

#include "logMsg/logMsg.h"

#include "au/vector.h"
#include "au/string.h"

namespace au 
{
    namespace token
    {
        
        class Token
        {
            
        public:
            
            typedef enum
            {
                normal,
                separator,
                literal
            } Type;
            
            
            std::string content;      // Content of the token
            int position;             // Position in the string
            Type type;
            
            // Constructor
            Token( std::string _content , Type _type , int _position );
            
            // Check if this component is a parituclar sequence
            bool is( std::string _content );
            
            // Check if this token is a numerical value
            bool isNumber();            
            
            // Debug str
            std::string str();
            
            bool isLiteral()
            {
                return (type == literal);
            }

            bool isSeparator()
            {
                return (type == separator);
            }
            
            bool isNormal()
            {
                return (type == normal);
            }

            
        };
        
        
        class TokenVector : public std::vector<Token>
        {
            size_t position;                  // Position inside the vector
            
            std::vector<std::string> tokens;  // Vector with the considered tokens
            
        public:
            
            
            // Add spetial tokens
            void addSingleCharTokens( std::string tokens ); 

            // General function to add spetial tokens
            void addToken( std::string token ); 
            
            // Main function to parse the provided command
            void parse( std::string command );
            
            // Functions to deserialize the provided command

            Token* getNextToken();
            Token* popToken();
                        
            bool popNextTokenIfItIs( std::string content );
            bool checkNextTokenIs( std::string content );
            bool checkNextNextTokenIs( std::string content );

            // Check end of the provided command
            bool eof();
            
            // Debug function 
            std::string str();
            
            // Auxiliar function to set the error.
            void set_error( au::ErrorManager *error , std::string error_message );
            
            
           
        };
        

        
    }
}

#endif