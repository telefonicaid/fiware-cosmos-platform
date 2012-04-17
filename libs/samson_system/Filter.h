
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
            std::string str()
            {
                std::string output = _str();
                if( next )
                {
                    output.append(" | ");
                    output.append( next->str() );
                }
                return output;
                
            }
            
            virtual std::string _str()=0;

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
            
            std::string _str()
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
            
            // Collection of fields to be used when exporting data
            au::vector<Source> fields;
            
            // Separator used at the output ( should be programable )
            std::string separator;
            
        public:
               
            FilterEmitTxt( samson::TXTWriter *_writer 
                          , au::token::TokenVector* token_vector
                          , au::ErrorManager * error )
            {
                // Keep a pointer to the writer to send data
                writer = _writer;
                
                while( ! token_vector->eof() )
                {
                    if( token_vector->getNextTokenContent() == "|" )
                        break;
                    
                    Source* source = getSource( token_vector , error );
                    if( error->isActivated() )
                        return;
                    
                    fields.push_back( source );
                }
                
                if( fields.size() == 0 )
                    error->set( "No fields specified in emit command" );
                
            }
            
            virtual void run( KeyValue kv )
            {
                
                std::ostringstream output;
                for ( size_t i = 0 ; i < fields.size() ; i++ )
                {
                    samson::system::Value* value = fields[i]->get(kv);
                    if( value )
                        output << value->get_string();
                    
                    if( i != ( fields.size() -1 ) )
                        output << separator;
                    
                }
                
                output << "\n";
                
                //std::string output = au::str("%s %s\n" , kv.key->str().c_str() , kv.value->str().c_str() );
                std::string output_str = output.str();  
                writer->emit(output_str.c_str() , output_str.length() );
            }
            
            std::string _str()
            {
                return "emit (txt)";
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
            
            std::string _str()
            {
                return au::str("parse" );
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
            
            std::string _str()
            {
                return au::str("parse_words" );
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
            
            std::string _str()
            {
                return au::str("parse_chars" );
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
            
            std::string _str()
            {
                return au::str("filter %s"  , eval_source->str().c_str() );
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

            
            std::string _str()
            {
                std::ostringstream output;
                output << "select " << source_for_key->str() << " , " << source_for_value;
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
         Collections of filter-chains to be executed
         */
        
        class FilterCollection
        {
            
            // Collections of filters
            au::vector<Filter> filters;
            
        public:
            
            
            ~FilterCollection();
            
            // String debug
            std::string str();
            
            // General command to parse
            void addFilters( std::string command , samson::KVWriter *writer , TXTWriter *txt_writer , au::ErrorManager* error );            
            
            // Run a particular key-value
            void run( KeyValue kv );

            size_t get_num_filters();
            
        private:
            
            Filter* getFilter( au::token::TokenVector *token_vector 
                              , samson::KVWriter *writer 
                              , TXTWriter *txt_writer 
                              , au::ErrorManager* error 
                              );            
            
            Filter* getFilterChain( au::token::TokenVector *token_vector  
                                   , samson::KVWriter *writer 
                                   , TXTWriter *txt_writer 
                                   ,  au::ErrorManager* error );

            
        };
        
    }
}

#endif
