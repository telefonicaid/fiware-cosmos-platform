
#ifndef _H_SAMSON_SYSTEM_KEYVALUE
#define _H_SAMSON_SYSTEM_KEYVALUE

#include "au/string.h"
#include "au/vector.h"
#include "au/StringComponents.h"
#include "au/Tokenizer.h"

#include <samson/module/samson.h>
#include <samson/modules/system/Value.h>


namespace samson{
    namespace system{
        
        // -----------------------------------------------------------------------
        // KeyValue : system.Value system.Value
        // -----------------------------------------------------------------------
        
        class KeyValue
        {
            
        public:
            
            samson::system::Value* key;
            samson::system::Value* value;
            
            KeyValue( samson::system::Value* _key , samson::system::Value* _value )
            {
                key = _key;
                value = _value;
            }
            
        };

    }
}

#endif
