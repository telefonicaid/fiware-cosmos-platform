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


#include "common.h" // Own interface


namespace samson
{
    namespace comscore
    {

        bool compareOriginalDictionaryEntry(const OriginalDictionaryEntry &a, const OriginalDictionaryEntry &b)
        {
            if( a.domain != b.domain )
                return a.domain < b.domain;
            if( a.order_1 != b.order_1 )
                return a.order_1 > b.order_1;
            if( a.order_2 != b.order_2 )
                return a.order_2 > b.order_2;
            
            return 0;
        }
        
        
    }
}