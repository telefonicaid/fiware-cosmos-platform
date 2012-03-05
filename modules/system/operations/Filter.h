
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_SYSTEM_FILTER
#define _H_SAMSON_SYSTEM_FILTER

#include "au/string.h"

#include <samson/module/samson.h>
#include <samson/modules/system/Value.h>


namespace samson{
    namespace system{
        
        
        class Filter
        {
            
        public:
            
            Filter* next; // Pointer to the next filter
            
            
            Filter( )
            {
                next = NULL;
            }
            
            virtual void run( samson::system::Value* key , samson::system::Value* value )=0;
            virtual std::string str()=0;
        };
        
        
        
        class FilterEmit : public Filter
        {
            int channel;
            samson::KVWriter *writer;
            
        public:
            
            FilterEmit( const std::string& description , samson::KVWriter *_writer )
            {
                //printf("Filter emit with '%s'\n" , description.c_str());
                writer = _writer;
                channel = atoi( description.c_str() );
            }
            
            void run( samson::system::Value* key , samson::system::Value* value )
            {
                //printf("Running emit with '%s'-'%s'\n" , key->str().c_str() , value->str().c_str() );
                writer->emit(channel, key, value );
            }
            
            std::string str()
            {
                return au::str("Emit to channel %d" , channel );
            }
            
            
        };
        
        
        // | select  key:0,key:1  |
        
        class FilterSelectSource
        {
            
        public:
            
            typedef enum
            {
                source_key,     // Selecting something in the key
                source_value,   // Selecting something in the value
                
                source_vector,  // Selecting a vector of things
                
                source_constant, // Constant value
                
                source_error    // Error understanding what to select
                
            } Source;
            
            // Used in direct selections
            Source source;
            int position;
            
            // Used in vector-based selection
            std::vector<FilterSelectSource*> vector_filter_select_source;
            
            // Used in constant value
            samson::system::Value value_constant;
            
            // key // value          Simple selection of key and value
            // key:1 // value:3      Selection of a field in a vector element
            // Future key:map:user   Selection of a entry in a map element
            
            FilterSelectSource( std::string description )
            {
                //printf("Creating FilterSelectSource with '%s'\n" , description.c_str() );
                
                if( description.length() == 0)
                {
                    source = source_error;
                    return;
                }
                
                // Vector selections
                if( description.substr(0,1) == "[")
                {
                    //printf("Creating vector FilterSelectSource....\n");
                    if( description.substr( description.length()-1,1 ) != "]" )
                    {
                        //printf("error Creating vector FilterSelectSource....\n");
                        source = source_error;
                        return;
                    }
                    
                    source = source_vector;
                    std::vector<std::string> vector_components = au::split( description.substr( 1 , description.length() - 2 ) , ';' );
                    for( size_t i = 0 ; i < vector_components.size() ; i++ )
                        vector_filter_select_source.push_back( new FilterSelectSource( vector_components[i] ) );
                    
                    return;
                }
                
                // Constant
                if( description.substr(0,1) == "<")
                {
                    if( description.substr( description.length()-1,1 ) != ">" )
                    {
                        //printf("Error terminating constant '%s'\n" , description.substr( description.length()-1,1 ).c_str() );
                        source = source_error;
                        return;
                    }
                    
                    source = source_constant;
                    value_constant.set_string( description.substr( 1 , description.length() - 2 ).c_str() );
                    return;
                }            
                
                // Normal selector
                std::vector<std::string> components = au::split(description, ':' );
                size_t num_components = components.size();
                if( ( num_components != 1 ) && ( num_components != 2 ) )
                {
                    source = source_error;
                    return;
                }
                
                // Select position ( if specified )
                position = -1;
                if( components.size() == 2 )
                    position = atoi( components[1].c_str() );
                
                if( components[0] == "key" )
                    source = source_key;
                else if( components[0] == "value" )
                    source = source_value;
                else
                    source = source_error;
                
                //printf("Created Filter %s\n" , str().c_str());
                
            }
            
            std::string str()
            {
                if( source == source_vector )
                {
                    std::ostringstream output;
                    output << "Vector [ ";
                    for( size_t i = 0 ; i < vector_filter_select_source.size() ; i++ )
                        output << vector_filter_select_source[i]->str() << " ";
                    output << "]";
                    return output.str();
                }
                if( source == source_constant )
                    return "Constant " + value_constant.str() ;
                if( source == source_error )
                    return "Error";
                if( source == source_key )
                    return au::str("Key:%d" , position );
                if( source == source_value )
                    return au::str("Value:%d" , position );
                
                return "Error";
            }
            
            
            bool set( samson::system::Value* target , samson::system::Value* key , samson::system::Value* value )
            {
                printf("Running selector '%s' from key %s and value %s\n" , str().c_str() , key->str().c_str() , value->str().c_str() );
                
                // Source value
                samson::system::Value* _source_for_target = NULL;
                
                switch ( source ) 
                {
                    case source_error:
                        return false;
                        break;
                        
                    case source_constant:
                        target->copyFrom( &value_constant );
                        return true;
                        break;
                        
                    case source_vector:
                    {
                        target->set_as_vector();
                        for ( size_t i = 0 ; i < vector_filter_select_source.size() ; i++ )
                            if( ! vector_filter_select_source[i]->set(  target->add_value_to_vector() , key, value ) )
                                return false;
                        return true;
                    }
                        
                    case source_key:
                        _source_for_target = key;
                        break;
                        
                    case source_value:
                        _source_for_target = value;
                        break;
                        
                        
                }
                
                if( position == -1 )
                {
                    target->copyFrom( _source_for_target );
                    return true;
                }
                else
                {
                    samson::system::Value* _internal_source_for_target = _source_for_target->get_value_from_vector( position );
                    
                    
                    
                    if( _internal_source_for_target )
                    {
                        target->copyFrom( _internal_source_for_target );
                        //printf("Final Source %s  // %s\n" , _internal_source_for_target->str().c_str() , target->str().c_str() );
                        return true;
                    }
                    else
                        return false;
                }
                
            }
            
            
        };
        
        
        class FilterSelect : public Filter
        {
            
            FilterSelectSource* source_key;
            FilterSelectSource* source_value;
            
            samson::system::Value _key;
            samson::system::Value _value;
            
        public:
            
            // | select key:0,value:6 |
            
            FilterSelect( const std::string& description )
            {
                //printf("Creating FilterSelect  with '%s'\n" , description.c_str());
                
                std::vector<std::string> components = au::split(description, ',' );
                
                if( components.size() != 2 )
                {
                    source_key = NULL;
                    source_value = NULL;
                    return;
                }
                else
                {
                    source_key    = new FilterSelectSource( components[0] );
                    source_value  = new FilterSelectSource( components[1] );
                }
                
            }
            
            ~FilterSelect()
            {
                if( source_key )
                    delete source_key;
                
                if( source_value )
                    delete source_value;
            }
            
            static Filter* getFilter( std::string command  , samson::KVWriter *writer )
            { 
                // Line of filters in this line
                std::vector<Filter*> filters;
                
                std::vector<std::string> components = au::split(command, '|' );
                for( size_t i = 0 ; i < components.size() ; i++ )
                {
                    //printf("Processing component '%s'\n"  , components[i].c_str() );
                    
                    au::CommandLine cmdLine;
                    cmdLine.parse( components[i] );
                    
                    if( cmdLine.get_num_arguments() < 2 )
                        continue;
                    
                    std::string main_command = cmdLine.get_argument(0);
                    
                    //printf("Processing main command '%s'\n"  , main_command.c_str() );
                    
                    if ( main_command == "select" )
                        filters.push_back( new FilterSelect( cmdLine.get_argument(1) ) );
                    else if( main_command == "emit" )
                        filters.push_back( new FilterEmit( cmdLine.get_argument(1) , writer ) );
                }
                
                if( filters.size() == 0 )
                    return NULL;
                
                // List the filters
                for ( size_t i = 0 ; i < (filters.size()-1) ; i++ )
                    filters[i]->next = filters[i+1];
                
                return filters[0];
            }
            
            std::string str()
            {
                return au::str("Filter select '%s'  '%s' " , source_key?source_key->str().c_str():"NULL" , source_value?source_value->str().c_str():"NULL" );
            }
            
            void run( samson::system::Value* key , samson::system::Value* value )
            {
                //printf("Running filter with '%s'-'%s'\n" , key->str().c_str() , value->str().c_str() );
                bool r_key   = source_key->set(&_key, key, value);
                bool r_value = source_value->set(&_value, key, value);
                
                if( r_key && r_value )
                {
                    if( next )
                    {
                        next->run(&_key, &_value);
                    }
                    else
                    {
                        //printf("Error no next...\n");
                    }
                    
                }
                else
                {
                    //printf("Error selecting...\n");
                }
            }
        };
    }
}

#endif
