
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
            
            static FilterParser* getFilter( std::string command , au::ErrorManager * error )
            {
                au::CommandLine cmdLine;
                cmdLine.set_flag_string("separator", " "); // By default space is used as separator
                cmdLine.parse( command );
                
                if( cmdLine.get_num_arguments() == 0 )
                {
                    error->set("No command provided");
                    return NULL;
                }
                
                FilterParser* filter = new FilterParser();
                
                std::string separator_param = cmdLine.get_flag_string("separator");
                if( separator_param.length() == 0 )
                    filter->separator = ' ';
                else if( separator_param.length() == 1 )
                    filter->separator = separator_param[0];
                else if ( separator_param == "tab" )
                    filter->separator = '\t';
                else
                {
                    delete filter;
                    error->set("Unknown separator. Use single character separator or tab");
                    return NULL;
                }
                
                // Fields ( if any )
                for( int i = 1 ; i < cmdLine.get_num_arguments() ; i++ )
                {
                    std::string field_definition = cmdLine.get_argument(i);
                    
                    if( ( field_definition == "number" ) || ( field_definition == "num" ) || ( field_definition == "n" ) )
                        filter->fields.push_back( number );
                    else
                        filter->fields.push_back( string );
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

        
        class SamsonTokenVector : public au::token::TokenVector
        {
            
        public:
            
            SamsonTokenVector();
            
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
            
            Filter* getFilter( std::string command , samson::KVWriter *writer , TXTWriter *txt_writer , au::ErrorManager* error )
            {
                
                au::CommandLine cmdLine;
                cmdLine.set_flag_boolean("only_key");
                cmdLine.parse( command );
                
                // Get the main command
                std::string main_command = cmdLine.get_argument(0);
                
                if ( au::string_begins(main_command, "select") )
                {
                    
                    // Rest of the command after select
                    size_t pos = command.find("select");
                    std::string rest_command = command.substr( pos + 6 );

                    // Get the token vector with all tokens
                    SamsonTokenVector token_vector;
                    token_vector.parse( rest_command );
                    
                    Source* key_source = getSource(&token_vector, error);
                    if( error->isActivated() )
                        return NULL; 

                    // Expect a ","
                    if( !token_vector.popNextTokenIfItIs(",") )
                    {
                        error->set( au::str("Expected ',' to separate key and value in a select statment. Found '%s'" 
                                            , token_vector.getNextTokenContent().c_str() ));
                        return NULL;
                    }
                    
                    Source* value_source = NULL;
                    if( !token_vector.eof() )
                    {
                        value_source = getSource(&token_vector, error);
                        if( error->isActivated() )
                            return NULL;
                    }
                    else
                        value_source = new SourceVoid();
                        
                    return new FilterSelect( key_source , value_source );                    

                }
                else if ( main_command == "parse_words" )
                {
                    return new FilterParserWords();
                }
                else if ( main_command == "parse_chars" )
                {
                    return new FilterParserChars();
                }
                else if ( main_command == "parse" )
                {
                    // Parse kind of filter
                    return FilterParser::getFilter(  command , error );
                }
                else if( main_command == "emit" )
                {
                    
                    if( writer )
                    {
                    
                        if ( cmdLine.get_num_arguments() > 1 )
                            return new FilterEmit( atoi( cmdLine.get_argument(1).c_str() ) , writer );
                        else
                            return new FilterEmit( 0 , writer ); // Default channel "0"
                    }
                    else if ( txt_writer )
                    {
                        return new FilterEmitTxt( txt_writer );                        
                    }
                    
                }
                else if( main_command == "emit_key" )
                {
                    
                    if ( !txt_writer )
                    {
                        error->set( "emit_key is only valid in parseOut operations like system.str ");
                        return NULL;
                    }
                    
                    return new FilterEmitKeyTxt( txt_writer );                        
                    
                }
                else if( main_command == "emit_value" )
                {
                    if ( !txt_writer )
                    {
                        error->set( "emit_value is only valid in parseOut operations like system.str ");
                        return NULL;
                    }
                    
                    return new FilterEmitValueTxt( txt_writer );                        
                }
                
                else if ( main_command == "filter" )
                {
                    // Rest of the command after select
                    size_t pos = command.find("filter");
                    std::string rest_command = command.substr( pos + 6 );

                    SamsonTokenVector token_vector;
                    token_vector.parse( rest_command );
                    
                    printf("DEBUG: Token vector %s\n" , token_vector.str().c_str() );
                    
                    Source* eval_source = getSource(&token_vector, error );
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

            void addFilter( std::string command  , samson::KVWriter *writer , TXTWriter *txt_writer ,  au::ErrorManager* error )
            {
                //printf("adding filter '%s'\n" , command.c_str() );
                
                // Line of filters for this command...
                au::vector<Filter> tmp_filters;
                
                std::vector<std::string> components = au::split(command, '|' );
                for( size_t i = 0 ; i < components.size() ; i++ )
                {
                    Filter * filter = getFilter( components[i] , writer , txt_writer ,  error );

                    
                    if( !filter )
                    {
					   if( !error->isActivated() )
						  error->set( au::str("Error building  component from command '%s'", command.c_str() ) );
					   tmp_filters.clearVector();
                       return; // Error in the filter creation...
                    }
                    else
                        tmp_filters.push_back(filter);
                    
                }
                
                if( tmp_filters.size() == 0 )
                    return;
                
                // Link the filters
                for ( size_t i = 0 ; i < (tmp_filters.size()-1) ; i++ )
                    tmp_filters[i]->next = tmp_filters[i+1];

                // Add the filter line
                filters.push_back( tmp_filters[0] );                
            }
            
            
            void addFilters( std::string command  , samson::KVWriter *writer , TXTWriter *txt_writer , au::ErrorManager* error )
            { 
                std::vector<std::string> commands = au::split(command, ';' );
                for( size_t i = 0 ; i < commands.size() ; i++ )
                    addFilter( commands[i] , writer , txt_writer , error );

            }
            
        };
        
    }
}

#endif
