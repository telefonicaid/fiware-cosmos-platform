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
#ifndef _H_AU_UTILS
#define _H_AU_UTILS

#include <cstring>
#include <string>
#include "au/namespace.h"

NAMESPACE_BEGIN(au)

template<typename T,typename T2>
void replaceIfLower(T &t,T2 v)
{
    if( (T)v < t )
        t = v;
}

template<typename T,typename T2>
void replaceIfHiger(T &t,T2 v)
{
    if( (T)v > t )
        t = v;
}

int getColumns();
void clear_line();


// Random 64 bit number
size_t code64_rand();
bool code64_is_valid( size_t v );
std::string code64_str( size_t );


// Remove '\n's at the end of the line
void remove_return_chars( char* line );


NAMESPACE_END

#endif
