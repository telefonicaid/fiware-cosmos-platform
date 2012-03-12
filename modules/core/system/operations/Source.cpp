

#include "SourceFunction.h"

#include "Source.h" // Own interface

namespace samson {
    namespace system {

        Source *getSource( au::token::TokenVector* token_vector , au::ErrorManager* error )
        {
            // ----------------------------------------------------------------
            // Vector 
            // ----------------------------------------------------------------
            
            if( token_vector->popNextTokenIfItIs("[") )
            {
                // Vector source
                au::vector<Source> source_components;
                
                while( true )
                {
                    au::token::Token* token = token_vector->getNextToken();
                    if( !token )
                    {
                        source_components.clearVector();
                        error->set("Unfinish vector");
                        return NULL;
                    }
                    else if( token->is("]") )
                    {
                        // end of vector
                        token_vector->popToken();
                        return new SourceVector( source_components );
                    }
                    
                    Source* tmp = getSource(token_vector, error);
                    if( error->isActivated() )
                    {
                        source_components.clearVector();
                        return NULL;
                    }
                    else
                        source_components.push_back(tmp);
                }
            }
            
            // ----------------------------------------------------------------
            // Map 
            // ----------------------------------------------------------------
            
            if( token_vector->popNextTokenIfItIs("{") )
            {
                // Vector source
                au::vector<Source> source_keys;
                au::vector<Source> source_values;
                
                while( true )
                {
                    au::token::Token* token = token_vector->getNextToken();
                    if( !token )
                    {
                        source_keys.clearVector();
                        source_values.clearVector();
                        error->set("Unfinish map");
                        return NULL;
                    }
                    else if( token->is("}") )
                    {
                        // end of vector
                        token_vector->popToken();
                        return new SourceMap( source_keys , source_values );
                    }
                    
                    Source* tmp_key = getSource(token_vector, error);
                    if( error->isActivated() )
                    {
                        source_keys.clearVector();
                        source_values.clearVector();
                        return NULL;
                    }
                    
                    if( !token_vector->popNextTokenIfItIs(":") )
                    {
                        error->set("Wrong map format");
                        source_keys.clearVector();
                        source_values.clearVector();
                        return NULL;
                    }
                    
                    Source* tmp_value = getSource(token_vector, error);
                    if( error->isActivated() )
                    {
                        source_keys.clearVector();
                        source_values.clearVector();
                        return NULL;
                    }
                    
                    // Accumulate components for this map
                    source_keys.push_back(tmp_key);
                    source_values.push_back(tmp_value);
                }
                
                
            }
            
            // ----------------------------------------------------------------
            // Functions 
            // ----------------------------------------------------------------
            
            // Generic function interface
            if( token_vector->checkNextNextTokenIs("(") )
            {
                // Recover the name of the function
                au::token::Token *token_function_name = token_vector->popToken();
                if( !token_function_name->isNormal() )
                {
                    error->set("Non valid function name");
                    return NULL;
                }
                
                token_vector->popToken(); // Skip "("
                
                au::vector<Source> source_components;
                while( true )
                {
                    au::token::Token* token = token_vector->getNextToken();
                    if( !token )
                    {
                        source_components.clearVector();
                        error->set("Unfinish function call");
                        return NULL;
                    }
                    else if( token->is(")") )
                    {
                        // end of vector
                        token_vector->popToken();
                        return SourceFunction::getSourceForFunction(token_function_name->content , source_components , error);
                    }
                    
                    Source* tmp = getSource(token_vector, error);
                    if( error->isActivated() )
                    {
                        source_components.clearVector();
                        return NULL;
                    }
                    else
                        source_components.push_back(tmp);
                }                    
                
            }            
            
            // ---------------------------------------------------------
            // Get the next element key, value , key[X]:[name] 
            // ---------------------------------------------------------
            
            au::token::Token * token = token_vector->popToken();
            if( !token )
            {
                error->set("Missing element");
                return NULL;
            }
            
            // Literal Constant
            if( token->isLiteral() )
                return new SourceStringConstant( token->content );
            
            
            
            
            // Key-word
            Source * main = NULL;
            
            if( token->is("key") )
                main =  new SourceKey();
            if( token->is("value") )
                main =  new SourceValue();
            
            
            // ---------------------------------------------------------
            // key or value
            // ---------------------------------------------------------
            if( main )
            {
                while( true )
                {
                    
                    if( token_vector->popNextTokenIfItIs("[") )
                    {
                        
                        Source* index = getSource(token_vector, error);
                        if( error->isActivated() )
                        {
                            delete main;
                            return NULL;
                        }
                        
                        if( !token_vector->popNextTokenIfItIs("]") )
                        {
                            delete main;
                            error->set("Wrong index for vector");
                            return NULL;
                        }
                        
                        // Cumulative source vectors
                        main = new SourceVectorComponent(main, index);
                    }
                    else if( token_vector->popNextTokenIfItIs(":[") )
                    {
                        
                        Source* index = getSource(token_vector, error);
                        if( error->isActivated() )
                        {
                            delete main;
                            return NULL;
                        }
                        
                        if( !token_vector->popNextTokenIfItIs("]") )
                        {
                            delete main;
                            error->set("Wrong index for vector");
                            return NULL;
                        }
                        
                        // Cumulative source vectors
                        main = new SourceMapComponent(main, index);
                    }
                    else
                    {
                        return main;
                    }
                    
                    
                }
            }
            
            
            // ---------------------------------------------------------
            // Constant
            // ---------------------------------------------------------
            if( token->isNumber() )
                return new SourceNumberConstant( atof( token->content.c_str() ) );
            else
                return new SourceStringConstant( token->content );
        }
    }
}
