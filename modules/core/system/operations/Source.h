
#ifndef _H_SAMSON_SYSTEM_SOURCE
#define _H_SAMSON_SYSTEM_SOURCE

#include "au/string.h"
#include "au/vector.h"
#include "au/StringComponents.h"
#include "au/Tokenizer.h"

#include <samson/module/samson.h>
#include <samson/modules/system/Value.h>

#include "KeyValue.h"


namespace samson{
    namespace system{
        
        // ---------------------------------------------------
        // Source
        // ---------------------------------------------------
        
        
        class Source
        {
            
        public:
            
            virtual samson::system::Value* get( KeyValue kv )=0;
            virtual std::string str()=0;
            virtual ~Source(){}
            
        };
        
        // ---------------------------------------------------
        // SourceVoid
        // ---------------------------------------------------
        
        class SourceVoid : public Source
        {
            samson::system::Value value;
            
        public:
            
            SourceVoid()
            {
                value.set_as_void();
            }
            
            virtual samson::system::Value* get( KeyValue kv )
            {
                return &value;
            }
            
            virtual std::string str()
            {
                return "<void>";
            }
            
            
        };
        
        class SourceStringConstant : public Source
        {
            Value value;
            
        public:
            
            SourceStringConstant( std::string _value )
            {
                value.set_string( _value );
            }
            
            samson::system::Value* get( KeyValue kv )
            {
                return &value;
            }
            
            std::string str()
            {
                return au::str("\"%s\"" , value.get_string().c_str() );
            }
            
        };
        
        class SourceNumberConstant : public Source
        {
            Value value;
            
        public:
            
            SourceNumberConstant( double _value )
            {
                value.set_double( _value );
            }
            
            samson::system::Value* get( KeyValue kv )
            {
                return &value;
            }
            
            std::string str()
            {
                return au::str("#%s" , value.get_string().c_str() );
            }
            
        };
        
        
        class SourceKey : public Source
        {
        public:
            
            samson::system::Value* get( KeyValue kv )
            {
                return kv.key;
            }
            
            virtual std::string str()
            {
                return "key";
            }
            
        };
        
        class SourceValue : public Source
        {
        public:
            
            samson::system::Value* get( KeyValue kv )
            {
                return kv.value;
            }
            
            virtual std::string str()
            {
                return "value";
            }
            
        };
        
        
        class SourceVectorComponent : public Source
        {
            
            Source* base;
            Source* index;
            
        public:
            
            SourceVectorComponent( Source* _base , Source* _index )
            {
                base = _base;
                index = _index;
            }
            
            samson::system::Value* get( KeyValue kv )
            {
                Value* base_value = base->get(kv);
                Value* index_value = index->get(kv);
                if( !index_value || !base_value )
                    return  NULL;
                
                return base_value->get_value_from_vector( index_value->getDouble() );
            }
            
            virtual std::string str()
            {
                return au::str( "%s[%s]" , base->str().c_str() , index->str().c_str() );
            }
            
        };
        
        class SourceMapComponent : public Source
        {
            
            Source* base;
            Source* index;
            
        public:
            
            SourceMapComponent( Source* _base,  Source* _index )
            {
                base = _base;
                index = _index;
            }
            
            samson::system::Value* get( KeyValue kv )
            {
                Value* base_value = base->get(kv);
                Value* index_value = index->get(kv);
                if( !index_value || !base_value )
                    return  NULL;
                
                std::string key = index_value->get_string();
                return base_value->get_value_from_map( key );
            }
            
            virtual std::string str()
            {
                return au::str( "%s:[%s]" , base->str().c_str() , index->str().c_str() );
            }
            
        };
        
        
        
        
        class SourceVector : public Source
        {
            au::vector<Source> source_components;
            samson::system::Value value; // To generate output
        public:
            
            SourceVector( au::vector<Source>& _source_components )
            {
                for (size_t i = 0 ; i < _source_components.size() ; i++ )
                    source_components.push_back( _source_components[i] );
            }
            
            virtual samson::system::Value* get( KeyValue kv )
            {
                value.set_as_vector();
                for ( size_t i = 0 ; i < source_components.size() ; i++ )
                {
                    samson::system::Value* tmp_value = source_components[i]->get(kv);
                    if( !tmp_value )
                        value.add_value_to_vector()->set_as_void();
                    else
                        value.add_value_to_vector()->copyFrom( tmp_value );
                }
                return &value;
            }
            
            virtual std::string str()
            {
                std::ostringstream output;
                output << "[ ";
                for ( size_t i = 0 ; i < source_components.size() ; i++ )
                    output << source_components[i]->str() << " ";
                output << "]";
                return output.str();
                
            }
        };
        
        class SourceMap : public Source
        {
            au::vector<Source> source_keys;
            au::vector<Source> source_values;
            
            samson::system::Value value; // To generate output
        public:
            
            SourceMap( au::vector<Source> _source_keys , au::vector<Source> _source_values )
            {
                if( _source_keys.size() != _source_values.size() )
                    return; // Error
                
                for (size_t i = 0 ; i < _source_keys.size() ; i++ )
                {
                    source_keys.push_back( _source_keys[i] );
                    source_values.push_back( _source_values[i] );
                }
            }
            
            virtual samson::system::Value* get( KeyValue kv )
            {
                value.set_as_vector();
                for ( size_t i = 0 ; i < source_keys.size() ; i++ )
                {
                    samson::system::Value* tmp_key   = source_keys[i]->get(kv);
                    samson::system::Value* tmp_value = source_values[i]->get(kv);
                    if( !tmp_value || !tmp_value )
                        return NULL;
                    
                    // Prepare the value
                    value.add_value_to_map( tmp_key->get_string() )->copyFrom( tmp_value );
                }
                return &value;
            }
            
            virtual std::string str()
            {
                std::ostringstream output;
                output << "{ ";
                for ( size_t i = 0 ; i < source_keys.size() ; i++ )
                {
                    output << source_keys[i]->str() << ":";
                    output << source_values[i]->str() << " ";
                }
                output << "}";
                return output.str();
                
            }
        };        
        
        
        Source *getSource( au::token::TokenVector* token_vector , au::ErrorManager* error );

        
    }
}

#endif

