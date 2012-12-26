/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

#ifndef _H_SAMSON_SYSTEM_KEYVALUE
#define _H_SAMSON_SYSTEM_KEYVALUE

#include "au/string.h"
#include "au/containers/vector.h"
#include "au/StringComponents.h"
#include "au/Tokenizer.h"

#include <samson/module/samson.h>
#include "samson_system/Value.h"


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
            
            // Constructor
            KeyValue()
            {
                key = NULL;
                value = NULL;
            }
            KeyValue( samson::system::Value* _key , samson::system::Value* _value );
            
        };

    }
}

#endif
