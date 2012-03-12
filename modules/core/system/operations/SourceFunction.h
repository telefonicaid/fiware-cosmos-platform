
#ifndef _H_SAMSON_SYSTEM_SOURCE_FUNCTION
#define _H_SAMSON_SYSTEM_SOURCE_FUNCTION

#include "au/string.h"
#include "au/vector.h"
#include "au/StringComponents.h"
#include "au/Tokenizer.h"

#include <samson/module/samson.h>
#include <samson/modules/system/Value.h>

#include "KeyValue.h"
#include "Source.h"


namespace samson{
    namespace system{

        
        class SourceFunction : public Source
        {
            
            std::string name;
        protected:
            
            au::vector<Source> input_sources;
            samson::system::Value value; // output
            
        public:
            
            static SourceFunction* getSourceForFunction( std::string function_name , au::vector<Source>& _input_sources ,au::ErrorManager * error );
            
            
            SourceFunction( std::string _name )
            {
                name = _name;
            }
            
            void setInputSource( au::vector<Source>& _input_sources   )
            {
                for (size_t i = 0 ; i < _input_sources.size() ; i++ )
                    input_sources.push_back( _input_sources[i] );
            }
            
            virtual std::string str()
            {
                std::ostringstream output;
                output << name << "( ";
                for ( size_t i = 0 ; i < input_sources.size() ; i++ )
                    output << input_sources[i]->str() << " ";
                output << ")";
                return output.str();
            }
            
        };
        
        class SourceFunctionisAlpha : public SourceFunction
        {
            
        public:
            
            SourceFunctionisAlpha() : SourceFunction( "isAlpha" )
            {
                
            }
            
            samson::system::Value* get( KeyValue kv )
            {
                if( input_sources.size() == 0 )
                    return NULL;
                
                samson::system::Value *source_value = input_sources[0]->get(kv);
                if( !source_value )
                    return NULL;
                
                if( !source_value->isString() )
                    return NULL;
                
                std::string line = source_value->get_string();
                
                
                for ( size_t i = 0 ; i < line.length() ; i++ )
                    if( !isalpha( line[i] ) )
                    {
                        value.set_double(0);
                        return &value;
                    }
                
                // Return true
                value.set_double(1);
                return &value;
            }            
        };
        
        
        class SourceFunctionStr : public SourceFunction
        {
            
        public:
            
            SourceFunctionStr() : SourceFunction( "str" )
            {
                
            }
            
            samson::system::Value* get( KeyValue kv )
            {
                if( input_sources.size() == 0 )
                    return NULL;
                
                
                std::string str;
                
                for ( size_t i = 0 ; i < input_sources.size() ; i++ )
                {
                    samson::system::Value *source_value = input_sources[i]->get(kv);
                    if( !source_value )
                        return NULL;
                    str.append( source_value->get_string() );
                }
                value.set_string(str);
                return &value;
            }            
        };
        
        class SourceFunctionStrlen : public SourceFunction
        {
            
        public:
            
            SourceFunctionStrlen() : SourceFunction( "strlen" )
            {
                
            }
            
            samson::system::Value* get( KeyValue kv )
            {
                // Lenght of the string...
                if( input_sources.size() == 0 )
                    return  NULL;
                else if( input_sources.size() == 1 )
                {
                    Value* v = input_sources[0]->get(  kv );
                    if(!v)
                        return NULL;
                    
                    // Set the value
                    value.set_double( v->get_string().length() );
                }
                else
                {
                    value.set_as_vector();
                    for ( size_t i = 0 ; i < input_sources.size() ; i++ )
                    {
                        Value* v = input_sources[0]->get(  kv );
                        if (v) 
                            value.add_value_to_vector()->set_double( v->get_string().length() );
                        else
                            value.add_value_to_vector()->set_double( 0 );
                    }
                    
                }
                
                
                return &value;
                
            }
            
        };

    }
}

#endif

