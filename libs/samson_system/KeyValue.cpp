

#include "KeyValue.h"

namespace samson 
{
    namespace system
    {
        KeyValue::KeyValue( samson::system::Value* _key , samson::system::Value* _value )
        {
            key = _key;
            value = _value;
        }
        
    }
}