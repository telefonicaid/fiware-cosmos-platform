
#ifndef _H_SAMSON_SYSTEM_FILTER
#define _H_SAMSON_SYSTEM_FILTER

#include "au/string.h"
#include "au/vector.h"
#include "au/StringComponents.h"
#include "au/Tokenizer.h"

#include <samson/module/samson.h>
#include <samson/modules/system/Value.h>

#include "KeyValue.h"
#include "Source.h"
#include "SourceFunction.h"

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
                    if( !isalpha( line[i] ) )
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
        
        typedef enum
        {
            equal,                      // ==
            greater_than,               // >
            less_than,                  // <
            greater_or_equal_than,      // >=
            less_or_equal_than,         // <=
            different_than,             // !=
            unknown
        } Comparisson;
        
        const char* str_Comparisson( Comparisson c );
        Comparisson comparition_from_string( std::string c );
           
        class FilterCondition : public Filter
        {
            
            Comparisson c;            // Type of comparisson
            Source* select_source;    // Left Source 
            Source* select_value;     // Rigth source
            
            double value_reference;
            
        public:
            
            // filter key:2 = 4.56
            FilterCondition( Source* _select_source , Comparisson _c , Source* _select_value  )
            {
                select_source = _select_source;
                c = _c;
                select_value = _select_value;
            }
            
            bool test( KeyValue kv )
            {
                Value * v1 = select_source->get(kv);
                Value * v2 = select_value->get(kv);
                
                if( !v1 || !v2 )
                    return  false; // Not possible to get one of the values

                if( v1->isNumber() )
                {
                    
                    double v1_value = v1->getDouble();
                    double v2_value = v2->getDouble();
                    
                    //printf("Comparing %f - %f\n" , tmp_value , value_reference );
                    
                    switch (c) 
                    {
                        case equal:                 return ( v1_value == v2_value );
                        case greater_than:          return ( v1_value >  v2_value );
                        case less_than:             return ( v1_value <  v2_value );
                        case greater_or_equal_than: return ( v1_value >= v2_value );
                        case less_or_equal_than:    return ( v1_value <= v2_value );
                        case different_than:        return ( v1_value != v2_value );
                        case unknown: return false;
                    }
                }
                else if ( v1->isString() )
                {
                    
                    std::string v1_value = v1->get_string();
                    std::string v2_value = v2->get_string();
                    
                    //printf("Comparing %f - %f\n" , tmp_value , value_reference );
                    
                    switch (c) 
                    {
                        case equal:                 return ( v1_value == v2_value );
                        case greater_than:          return ( v1_value >  v2_value );
                        case less_than:             return ( v1_value <  v2_value );
                        case greater_or_equal_than: return ( v1_value >= v2_value );
                        case less_or_equal_than:    return ( v1_value <= v2_value );
                        case different_than:        return ( v1_value != v2_value );
                        case unknown: return false;
                    }
                }
                else
                    return false; // No comparisio for vector of map
    
                
                LM_X(1, ("Internal error"));
                return  false;
                
            }
            
            void run( KeyValue kv )
            {
                if( test(kv) )
                    if( next )
                        next->run( kv );
            }
            
            std::string str()
            {
                return au::str("FilterCondition %s %s %s"  , select_source->str().c_str() , str_Comparisson(c) , select_value->str().c_str() );
            }
            
        };
        
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
                samson::system::Value* source_key = source_for_key->get(kv);
                samson::system::Value* source_value = source_for_value->get(kv);
                
                if( source_key )
                    key.copyFrom( source_key );
                else
                    key.set_as_void();

                if( source_value )
                    value.copyFrom( source_value );
                else
                    value.set_as_void();
                
                if( next )
                {
                    KeyValue kv2( &key , &value );
                    next->run(kv2);
                }
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
            
            Filter* getFilter( std::string command , samson::KVWriter *writer , au::ErrorManager* error )
            {
                
                au::CommandLine cmdLine;
                cmdLine.parse( command );
                
                // Get the main command
                std::string main_command = cmdLine.get_argument(0);
                
                if ( au::string_begins(main_command, "select") )
                {
                    
                    // Rest of the command after select
                    size_t pos = command.find("select");
                    std::string rest_command = command.substr( pos + 6 );
                    
                    SamsonTokenVector token_vector;
                    token_vector.parse( rest_command );
                    
                    Source* key_source = getSource(&token_vector, error);
                    if( error->isActivated() )
                        return NULL; 
                    
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
                    if ( cmdLine.get_num_arguments() > 1 )
                        return new FilterEmit( atoi( cmdLine.get_argument(1).c_str() ) , writer );
                    else
                        return new FilterEmit( 0 , writer ); // Default channel "0"
                }
                else if ( main_command == "filter" )
                {
                    // Rest of the command after select
                    size_t pos = command.find("filter");
                    std::string rest_command = command.substr( pos + 6 );

                    SamsonTokenVector token_vector;
                    token_vector.parse( rest_command );
                    
                    Source* left_source = getSource(&token_vector, error );
                    if( error->isActivated() )
                        return NULL;
                    if( !left_source )
                    {
                        error->set("Not valid left selector for filter command");
                        return NULL;
                    }
                    if( token_vector.eof() )
                    {
                        error->set("Unfinished filter command");
                        return NULL;
                    }
                    
                    // Get the symbol
                    std::string s = token_vector.popToken()->content;
                    Comparisson c = comparition_from_string(s);
                    
                    if( c == unknown )
                    {
                        error->set("filter clause with unknown comparison selector");
                        delete left_source;
                        return NULL;
                    }

                    // Get the left side
                    Source* rigth_source = getSource(&token_vector, error );
                    if( error->isActivated() )
                        return NULL; 
                    if( !rigth_source )
                    {
                        error->set("Not valid left selector for filter command");
                        return NULL;
                    }
                    return new FilterCondition( left_source , c , rigth_source );
                }
                return NULL;
                
            }
            
            
            // filter key = 67 | select key:1,value | emit 0 / filter key = 56 | select key:1,value | emit 1

            void addFilter( std::string command  , samson::KVWriter *writer , au::ErrorManager* error )
            {
                //printf("adding filter '%s'\n" , command.c_str() );
                
                // Line of filters for this command...
                au::vector<Filter> tmp_filters;
                
                std::vector<std::string> components = au::split(command, '|' );
                for( size_t i = 0 ; i < components.size() ; i++ )
                {
                    Filter * filter = getFilter( components[i] , writer , error );

                    
                    if( !filter )
                    {
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
            
            
            void addFilters( std::string command  , samson::KVWriter *writer , au::ErrorManager* error )
            { 
                std::vector<std::string> commands = au::split(command, ';' );
                for( size_t i = 0 ; i < commands.size() ; i++ )
                    addFilter( commands[i] , writer , error );

            }
            
        };
        
    }
}

#endif
