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


#include "samson_system/Value.h"
#include "au/string.h"

int main( int argC , const char*argV[] )
{

    double a = -0.000000000000000001234;
    for( int i = 0 ; i < 40 ; i ++ )
    {
        printf("%s\n" , au::str(a).c_str() );
        a*=10;
    }
    a = 0;
    printf("%s\n" , au::str( a ).c_str() );
    a = 0.000000000000000001234;
    for( int i = 0 ; i < 40 ; i ++ )
    {
        printf("%s\n" , au::str(a).c_str() );
        a*=10;
    }

    
}
