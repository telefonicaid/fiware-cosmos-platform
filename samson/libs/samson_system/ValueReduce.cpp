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

#include "ValueReduce.h"

namespace samson
{
    namespace system
    {
        ValueList::ValueList( int _max_num_elements )
        {
            max_num_elements = _max_num_elements;
            
            values = (Value**) malloc( max_num_elements * sizeof(Value*) );
            counters = (int*) malloc( max_num_elements * sizeof(int) );
            
            for (int i = 0 ; i < max_num_elements ; i++ )
            {
                values[i] = new Value();
                counters[i] = 0; // Init counter to 0
            }
  
        }
    }
}