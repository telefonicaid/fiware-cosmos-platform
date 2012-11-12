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


#ifndef _H_SAMSON_TOPIC_MONITOR_COMMON
#define _H_SAMSON_TOPIC_MONITOR_COMMON

#include <string>

class Topic
{
public:
    
    std::string concept;
    size_t num;
    
    Topic( std::string _concept , size_t _num )
    {
        concept = _concept;
        num = _num;
    }
    
};

#endif