

#include "SourceFunction.h"

#include "Source.h" // Own interface

namespace samson {
    namespace system {
        
        Source *getSingleSource( au::token::TokenVector* token_vector , au::ErrorManager* error )
        {
            
            // ----------------------------------------------------------------
            // Braquets to group operations 
            // ----------------------------------------------------------------
            
            if( token_vector->popNextTokenIfItIs("(") )
            {

                Source* source = getSource(token_vector, error);
                if( !source || error->IsActivated() )
                    return NULL;
                
                if( !token_vector->popNextTokenIfItIs(")") )
                {
                    error->set("Parentheses not closed");
                }

                return source;
            }
            
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
                    if( error->IsActivated() )
                    {
                        source_components.clearVector();
                        return NULL;
                    }
                    else
                        source_components.push_back(tmp);
                    
                    // Read the mandatory "," if it is not the end of the vector
                    if( !token_vector->checkNextTokenIs("]") )
                        if( !token_vector->popNextTokenIfItIs(",") )
                        {
                            error->set(  au::str("Wrong map format (expected ',' instead of %s)" , token_vector->getNextTokenContent().c_str() ) );
                            source_components.clearVector();
                            return NULL;
                        }

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
                        // end of map
                        token_vector->popToken();
                        return new SourceMap( source_keys , source_values );
                    }
                    
                    Source* tmp_key = getSource(token_vector, error);
                    if( error->IsActivated() )
                    {
                        source_keys.clearVector();
                        source_values.clearVector();
                        return NULL;
                    }
                    
                    if( !token_vector->popNextTokenIfItIs(":") )
                    {
                        error->set(  au::str("Wrong map format (expected ':' instead of %s)" , token_vector->getNextTokenContent().c_str() ) );
                        source_keys.clearVector();
                        source_values.clearVector();
                        return NULL;
                    }
                    
                    Source* tmp_value = getSource(token_vector, error);
                    if( error->IsActivated() )
                    {
                        source_keys.clearVector();
                        source_values.clearVector();
                        return NULL;
                    }
                    
                    // Accumulate components for this map
                    source_keys.push_back(tmp_key);
                    source_values.push_back(tmp_value);
                    
                    // Read the mandatory "," if it is not the end of the map
                    if( !token_vector->checkNextTokenIs("}") )
                        if( !token_vector->popNextTokenIfItIs(",") )
                        {
                            error->set(  au::str("Wrong map format (expected ',' instead of %s)" , token_vector->getNextTokenContent().c_str() ) );
                            source_keys.clearVector();
                            source_values.clearVector();
                            return NULL;
                        }

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
                        token_vector->popToken(); // skip ")"
                        return SourceFunction::getSourceForFunction(token_function_name->content , source_components , error);
                    } 
                    else if ( (source_components.size()==0) || (token->is(",") ) )
                    {
                        if( (source_components.size()==0) && token->is(",") )
                        {
                            error->set("Non valid first parameter for function call");
                            source_components.clearVector();
                            return NULL;
                        }
                        
                        if ( token->is(",") )
                            token_vector->popToken(); // Skip ","
                        
                        // Another component
                        Source* tmp = getSource(token_vector, error);
                        if( error->IsActivated() )
                        {
                            source_components.clearVector();
                            return NULL;
                        }
                        else
                            source_components.push_back(tmp);
                    }
                    else
                    {
                        error->set(au::str("Non valid function call. Found %s when expecting ) or ," , token->content.c_str()));
                        return NULL;
                    }
                    
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
                        if( error->IsActivated() )
                        {
                            delete main;
                            return NULL;
                        }
                        
                        if( !token_vector->popNextTokenIfItIs("]") )
                        {
                            delete main;
                            
                            std::string token_content = token_vector->getNextTokenContent();
                            error->set(au::str("Wrong index for vector. Expeted ] and found %s" , token_content.c_str() ));
                            return NULL;
                        }
                        
                        // Cumulative source vectors
                        main = new SourceVectorComponent(main, index);
                    }
                    else if( token_vector->popNextTokenIfItIs(".") )
                    {
                        
                        Source* index = getSource(token_vector, error);
                        if( error->IsActivated() )
                        {
                            delete main;
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

            // Negative numbers
            
            if( token->is("-") )
            {
                // Get next token that has to be a number
                au::token::Token *next_token = token_vector->popToken();
                if( !next_token )
                {
                    error->set( au::str("Extected a number after sign '-' but found nothing" ));
                    return  NULL;
                }
                
                if( !token->isNumber() )
                {
                    error->set( au::str("Extected a number after sign '-' but found %s" , token->content.c_str() ) );
                    return NULL;
                }
                
                
                return new SourceNumberConstant( - atof( next_token->content.c_str() ) );
                
            }
            
            
            // ---------------------------------------------------------
            // Constant
            // ---------------------------------------------------------
            if( token->isNumber() )
                return new SourceNumberConstant( atof( token->content.c_str() ) );
            else
                return new SourceStringConstant( token->content );
        }
            
        Source *getSource( au::token::TokenVector* token_vector , au::ErrorManager* error )
        {
            
            Source * source = NULL;
            
            while( true )
            {
                if( !source )
                {
                    source = getSingleSource( token_vector , error );
                    if( !source || error->IsActivated() )
                        return NULL;
                }
                
                // Check if there is something to continue "< > <= >= != + - * /
                au::token::Token* token = token_vector->getNextToken();
                if( !token )
                    return source; // No more tokens
                
                
                if( token->isComparator() )
                {
                    // Skip the comparator
                    std::string comparator = token->content;
                    token_vector->popToken();
                    
                    Source * _source = getSingleSource(token_vector, error);
                    if( !_source || error->IsActivated() )
                    {
                        delete source;
                        return NULL;
                    }
                    
                    // Source Comparision
                    SourceCompare::Comparisson c = SourceCompare::comparition_from_string( comparator );
                    source = new SourceCompare( source , _source , c );
                    
                    // Search for more operations...
                    continue;
                }

                if( token->isOperation() )
                {
                    // Skip the comparator
                    std::string operation_string = token->content;
                    token_vector->popToken();
                    
                    Source * _source = getSingleSource(token_vector, error);
                    if( !_source || error->IsActivated() )
                    {
                        delete source;
                        return NULL;
                    }
                    
                    // Source Comparision
                    SourceOperation::Operation o = SourceOperation::operation_from_string( operation_string );
                    source = new SourceOperation( source , _source , o );
                    
                    // Search for more operations...
                    continue;
                }
                
                
                if( token_vector->popNextTokenIfItIs("?") )
                {
                    
                    Source * first_source = getSingleSource(token_vector, error);
                    if( !first_source || error->IsActivated() )
                    {
                        delete source;
                        return NULL;
                    }

                    
                    if( !token_vector->popNextTokenIfItIs(":") )
                    {
                        delete source;
                        delete first_source;
                        error->set( au::str("Statement '?' without ':'. Expected ':' but found %s" 
                                            , token_vector->getNextTokenContent().c_str() ) );
                        return NULL;
                    }
                    
                    Source * second_source = getSingleSource(token_vector, error);
                    if( !second_source || error->IsActivated() )
                    {
                        delete source;
                        return NULL;
                    }

                    // Create a source X ? X : X
                    source = new SourceCompareSelector(source, first_source, second_source );
                    
                }
                
                
                return source;
                
            }
            
        }
            
            
    }
}
