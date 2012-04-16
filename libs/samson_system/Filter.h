
#ifndef _H_SAMSON_SYSTEM_FILTER
#define _H_SAMSON_SYSTEM_FILTER

#include "au/string.h"
#include "au/containers/vector.h"
#include "au/StringComponents.h"
#include "au/Tokenizer.h"
#include "au/charset.h"

#include <samson/module/samson.h>

#include "samson_system/Value.h"
#include "samson_system/KeyValue.h"
#include "samson_system/Source.h"
#include "samson_system/SourceFunction.h"

namespace samson{
    namespace system{
        
        // -----------------------------------------------------------------------
        // Filter for map-like operations over system.Value system.Value queues
        // -----------------------------------------------------------------------
        
        class Filter
        {
            
        public:
            
            Filter* next; // Pointer to the next filter
            
            
            Filter( )
            {
                next = NULL;
            }
                        
            virtual ~Filter()
            {
            }
            
            virtual void run( KeyValue kv )=0;
            virtual std::string str()=0;
        };
        
        // --------------------------------------------------------
        // FilterEmit
        // --------------------------------------------------------
        
        class FilterEmit : public Filter
        {
            int channel;
            samson::KVWriter *writer;
            
        public:
            
            FilterEmit( int _channel , samson::KVWriter *_writer )
            {
                writer = _writer;
                channel = _channel;
            }
            
            virtual void run( KeyValue kv )
            {
                //printf("Running emit with '%s'-'%s'\n" , key->str().c_str() , value->str().c_str() );
                writer->emit(channel, kv.key, kv.value );
            }
            
            std::string str()
            {
                return au::str("Emit to channel %d" , channel );
            }
            
            
        };

        // --------------------------------------------------------
        // FilterEmitTxt
        // --------------------------------------------------------
        
        class FilterEmitTxt : public Filter
        {
            samson::TXTWriter *writer;
            
        public:
               
            FilterEmitTxt( samson::TXTWriter *_writer )
            {
                writer = _writer;
            }
            
            virtual void run( KeyValue kv )
            {
                std::string output = au::str("%s %s\n" , kv.key->str().c_str() , kv.value->str().c_str() );
                writer->emit(output.c_str() , output.length() );
            }
            
            std::string str()
            {
                return "Emit txt";
            }
            
        };        

        // --------------------------------------------------------
        // FilterEmitTxt
        // --------------------------------------------------------
        
        class FilterEmitKeyTxt : public Filter
        {
            samson::TXTWriter *writer;
            
        public:
            
            FilterEmitKeyTxt( samson::TXTWriter *_writer )
            {
                writer = _writer;
            }
            
            virtual void run( KeyValue kv )
            {
                std::string output = au::str("%s\n" , kv.key->str().c_str() );
                writer->emit(output.c_str() , output.length() );
            }
            
            std::string str()
            {
                return "Emit key";
            }
            
        };
        
        // --------------------------------------------------------
        // FilterEmitTxt
        // --------------------------------------------------------
        
        class FilterEmitValueTxt : public Filter
        {
            samson::TXTWriter *writer;
            
        public:
            
            FilterEmitValueTxt( samson::TXTWriter *_writer )
            {
                writer = _writer;
            }
            
            virtual void run( KeyValue kv )
            {
                std::string output = au::str("%s\n" , kv.value->str().c_str() );
                writer->emit(output.c_str() , output.length() );
            }
            
            std::string str()
            {
                return "Emit value";
            }
            
        };
        // --------------------------------------------------------
        // parse -  parse line
        // --------------------------------------------------------
        
        class FilterParser : public Filter
        {
            
            typedef enum{
                string,
                number
            } Type;
            
            // Field codification
            std::vector<Type> fields;
            
            // Separator
            char separator;
            
            // Key key used to "emit" to the next filter
            samson::system::Value key;
            
            FilterParser()
            {
                separator = ' ';
            }
            
        public:
            
            virtual ~FilterParser()
            {
                
            }
            
            // parse field0 field1 field2 -separator X 
            
            static FilterParser* getFilter( au::token::TokenVector* token_vector , au::ErrorManager * error )
            {
                
                if( token_vector->eof() )
                {
                    error->set("No command provided");
                    return NULL;
                }
                
                FilterParser* filter = new FilterParser();
                filter->separator = ' ';

                
                // Fields ( if any )
                while( !token_vector->eof() )
                {
                    if( token_vector->popNextTokensIfTheyAre("-","separator") )
                    {
                        // Extract separator
                        au::token::Token * separator_token = token_vector->popToken();
                        
                        if( !separator_token )
                        {
                            error->set("Wrong separator in filter command");
                            delete filter;
                            return NULL;
                        }
                        
                        if( separator_token->content.length() != 1 )
                        {
                            error->set( au::str( "%s is a wrong separator in filter command ( only 1 char separators supported )" , separator_token->content.c_str() ) );
                            delete filter;
                            return NULL;
                        }
                        
                        filter->separator = separator_token->content[0];
                        continue;
                    }
                    
                    
                    au::token::Token* token = token_vector->popToken();
                    
                    if ( !token->isNormal() )
                    {
                        error->set( au::str("Incorrect field definition %s" , token->content.c_str() ) );
                        delete filter;
                        return NULL;
                    }
                    
                    std::string field_definition = token->content;
                    
                    if( ( field_definition == "number" ) || ( field_definition == "num" ) || ( field_definition == "n" ) )
                        filter->fields.push_back( number );
                    else if ( ( field_definition == "string" ) || ( field_definition == "s" ) )
                        filter->fields.push_back( string );
                    else
                        if ( !token->isNormal() )
                        {
                            error->set( au::str("Incorrect field definition %s" , token->content.c_str() ) );
                            delete filter;
                            return NULL;
                        }

                }
                
                return filter;
            }
            
            virtual void run( KeyValue kv )
            {
                // Key should be string for this operation
                if ( !kv.key->isString() )
                    return ;
                
                std::string line = kv.key->get_string();

                au::StringComponents string_components;
                string_components.process_line( line.c_str() , line.length() , separator );
                
                key.set_as_vector();

                if( fields.size() == 0 )
                {
                    for ( size_t i = 0 ; i < string_components.components.size() ; i++ )
                        key.add_value_to_vector()->set_string( string_components.components[i] );
                }
                else
                {
                    for ( size_t i = 0 ; i < string_components.components.size() ; i++ )
                    {
                        if( i < fields.size() )
                        {
                            samson::system::Value *v = key.add_value_to_vector();
                            v ->set_string( string_components.components[i] );
                            
                            switch (fields[i]) 
                            {
                                case string:
                                    break;
                                case number:
                                    v->convert_to_number();
                                    break;
                            }
                        }
                    }
                }
                
                // Run next filter
                if( next )
                {
                    KeyValue next_kv( &key , kv.value );
                    next->run( next_kv );
                }
                
            }
            
            std::string str()
            {
                return au::str("FilterParser: " );
            }
            
            
        };

        // --------------------------------------------------------
        // Extract words
        // --------------------------------------------------------
        
        class FilterParserWords : public Filter
        {
            // Key key used to "emit" to the next filter
            samson::system::Value key;
            
        public:
            
            virtual void run( KeyValue kv )
            {
                // Key should be string for this operation
                if ( !kv.key->isString() )
                    return ;

                // Next key-value ready to be emitted
                KeyValue next_kv( &key , kv.value );
                
                // Line to parse
                std::string line = kv.key->get_string();


                size_t pos = 0;
                size_t len = line.length();
                for ( size_t i = 0 ; i < len ; i++ )
                {
                    if( ! au::iso_8859_is_letter( line[i] ) )
                    {
                        if( pos < i )
                        {
                            // New word
                            std::string word = line.substr(pos,i-pos);
                            key.set_string(word);
                            
                            // Run next filter
                            if( next )
                                next->run( next_kv );

                        }
                        
                        // Go to the next
                        pos = i+1;

                    }
                    
                    
                }

				// Emit last word
				if( pos < line.length() )
				{
				   std::string word = line.substr(pos);
				   key.set_string(word);
				   
				   // Run next filter
				   if( next )
					  next->run( next_kv );
                }
                
            }
            
            std::string str()
            {
                return au::str("FilterParser: " );
            }
            
            
        };
        

        // --------------------------------------------------------
        // Extract chars
        // --------------------------------------------------------
        
        class FilterParserChars : public Filter
        {
            // Key key used to "emit" to the next filter
            samson::system::Value key;
            
        public:
            
            virtual void run( KeyValue kv )
            {
                // Key should be string for this operation
                if ( !kv.key->isString() )
                    return ;
                
                // Next key-value ready to be emitted
                KeyValue next_kv( &key , kv.value );
                
                // Line to parse
                std::string line = kv.key->get_string();
                
                size_t len = line.length();
                for ( size_t i = 0 ; i < len ; i++ )
                {
                    // New letter
                    std::string letter = line.substr(i,1);
                    key.set_string(letter);
                    
                    // Run next filter
                    if( next )
                        next->run( next_kv );
                }
            }
            
            std::string str()
            {
                return au::str("FilterParser: " );
            }
            
            
        };
        
        // ----------------------------------------------------
        // FilterCondition
        // ----------------------------------------------------
           
        class FilterCondition : public Filter
        {
            
            Source* eval_source; 
            
        public:
            
            // filter key:2 = 4.56
            FilterCondition( Source* _eval_source  )
            {
                eval_source = _eval_source;
            }
            
            bool test( KeyValue kv )
            {
                samson::system::Value * v = eval_source->get(kv);
                
                if( !v )
                    return false;

                return ( v->getDouble() != 0 );
            }
            
            void run( KeyValue kv )
            {
                if( test(kv) )
                    if( next )
                        next->run( kv );
            }
            
            std::string str()
            {
                return au::str("FilterCondition %s"  , eval_source->str().c_str() );
            }
            
        };
        
        // ----------------------------------------------------
        // FilterSelect
        // ----------------------------------------------------
        
        class FilterSelect : public Filter
        {
            
            Source* source_for_key;
            Source* source_for_value;
            
            samson::system::Value key;
            samson::system::Value value;

        public:
            
            
            FilterSelect( Source* _source_for_key , Source* _source_for_value )
            {
                source_for_key = _source_for_key;
                source_for_value = _source_for_value;
            }
            
            ~FilterSelect()
            {
                delete source_for_key;
                delete source_for_value;
            }

            
            std::string str()
            {
                std::ostringstream output;
                output << "Filter select " << source_for_key->str() << " , " << source_for_value;
                return output.str();
            }
            
            void run( KeyValue kv )
            {
                samson::system::Value* value_for_key   = source_for_key->get(kv);
                samson::system::Value* value_for_value = source_for_value->get(kv);
                
                if( value_for_key )
                    key.copyFrom( value_for_key );
                else
                    key.set_as_void();

                if( value_for_value )
                    value.copyFrom( value_for_value );
                else
                    value.set_as_void();

                // Run next element
                if( next )
                    next->run( KeyValue( &key , &value ) );
            }
        };

        
        class SamsonTokenizer : public au::token::Tokenizer
        {
        public:
            SamsonTokenizer();
        };
        
        
        
        /*
         Collections of filter - chains to be executed
         */
        
        class FilterCollection
        {
            
        public:
            
            au::vector<Filter> filters;
            
            ~FilterCollection()
            {
                // Remove defined filters
                filters.clearVector();
            }
            
            std::string str()
            {
                std::ostringstream output;
                for( size_t i = 0 ; i < filters.size() ; i++ )
                    output << filters[i]->str();
                return output.str();
            }
            
            Filter* getFilter( au::token::TokenVector *token_vector 
                              , samson::KVWriter *writer 
                              , TXTWriter *txt_writer 
                              , au::ErrorManager* error 
                              )
            {

                // Check if there are tokens to be read
                if( token_vector->eof() )
                {
                    error->set("Filter name not specified");
                    return NULL;
                }
                
                // Get the next token
                au::token::Token* token = token_vector->popToken(); 
                
                if ( token->content == "select" )
                {
                    
                    Source* key_source = getSource(token_vector, error);
                    if( error->isActivated() )
                        return NULL; 

                    // Expect a ","
                    if( !token_vector->popNextTokenIfItIs(",") )
                    {
                        error->set( au::str("Expected ',' to separate key and value in a select statment. Found '%s'" 
                                            , token_vector->getNextTokenContent().c_str() ));
                        return NULL;
                    }
                    
                    Source* value_source = NULL;
                    if( !token_vector->eof() )
                    {
                        value_source = getSource(token_vector, error);
                        if( error->isActivated() )
                            return NULL;
                    }
                    else
                        value_source = new SourceVoid();
                        
                    return new FilterSelect( key_source , value_source );                    

                }
                else if ( token->content == "parse_words" )
                {
                    return new FilterParserWords();
                }
                else if ( token->content == "parse_chars" )
                {
                    return new FilterParserChars();
                }
                else if ( token->content == "parse" )
                {
                    // Parse kind of filter
                    return FilterParser::getFilter( token_vector , error );
                }
                else if ( token->content == "emit" )
                {
                    
                    if( writer )
                    {
                        
                        if( token_vector->eof() )
                            return new FilterEmit( 0 , writer ); // Default channel "0"

                        
                        au::token::Token* number = token_vector->popToken();
                        if( !number->isNumber() )
                        {
                            error->set( au::str("Channel '%s' not valid in emit command. It should be a number"
                                                , number->content.c_str() ));
                            return NULL;
                        }
                        
                        int channel = atoi( number->content.c_str() );
                        return new FilterEmit( channel , writer );
                        
                    }
                    else if ( txt_writer )
                    {
                        return new FilterEmitTxt( txt_writer );                        
                    }
                    
                }
                else if ( token->content == "emit_key" )
                {
                    
                    if ( !txt_writer )
                    {
                        error->set( "emit_key is only valid in parseOut operations like system.str ");
                        return NULL;
                    }
                    
                    return new FilterEmitKeyTxt( txt_writer );                        
                    
                }
                else if ( token->content == "emit_value" )
                {
                    if ( !txt_writer )
                    {
                        error->set( "emit_value is only valid in parseOut operations like system.str ");
                        return NULL;
                    }
                    
                    return new FilterEmitValueTxt( txt_writer );                        
                }
                else if ( token->content == "filter" )
                {
                    
                    Source* eval_source = getSource(token_vector, error );
                    if( error->isActivated() )
                        return NULL;
                    if( !eval_source )
                    {
                        error->set("Not valid condition statment in filter command");
                        return NULL;
                    }
                    
                    return new FilterCondition( eval_source );
                    
                }
                return NULL;
                
            }
            
            
            // filter key = 67 | select key:1,value | emit 0 / filter key = 56 | select key:1,value | emit 1

            Filter* getFilterChain( au::token::TokenVector *token_vector  
                           , samson::KVWriter *writer 
                           , TXTWriter *txt_writer 
                           ,  au::ErrorManager* error )
            {
                
                // Line of filters for this command...
                au::vector<Filter> tmp_filters;

                while ( !token_vector->eof() ) 
                {
                    // Get the "sub" token vector for each line
                    au::token::TokenVector sub_token_vector = token_vector->getTokensUntil( "|" );
                    
                    // Get a filter from this token_vector
                    Filter * filter = getFilter( &sub_token_vector , writer , txt_writer , error );
                    
                    // If there is an error, just return
                    if( error->isActivated() )
                    {
                        tmp_filters.clearVector();
                        return NULL;
                    }
                    else
                    {
                        // Add the new filter
                        tmp_filters.push_back( filter );
                    }
                }                
                
                
                if( tmp_filters.size() == 0 )
                    return  NULL;
            
                // Link the filters
                for ( size_t i = 0 ; i < (tmp_filters.size()-1) ; i++ )
                    tmp_filters[i]->next = tmp_filters[i+1];
                
                // Add the filter line
                return tmp_filters[0];
            }
            
            // General command to parse
            void addFilters( std::string command , samson::KVWriter *writer , TXTWriter *txt_writer , au::ErrorManager* error )
            { 
                // Tokenice the entire command 
                // --------------------------------------------------------------------
                SamsonTokenizer tokenizer;
                au::token::TokenVector token_vector = tokenizer.parse(command);
                
                
                while ( !token_vector.eof() ) 
                {
                    // Get the "sub" token vector for each line
                    au::token::TokenVector sub_token_vector = token_vector.getTokensUntil( ";" );

                    // Get a filter from this token_vector
                    Filter * filter = getFilter( &sub_token_vector , writer , txt_writer , error );

                    // If there is an error, just return
                    if( error->isActivated() )
                    {
                        filters.clearVector();
                        return;
                    }
                    else
                    {
                        // Add the new filter
                        filters.push_back( filter );
                    }
                }
                
            }
            
        };
        
    }
}

#endif
