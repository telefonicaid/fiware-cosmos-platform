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

#include "gtest/gtest.h"
#include "au/CommandLine.h"
#include "au/CounterCollection.h"
#include "au/StringComponents.h"

void a_main()
{
}

TEST(CounterCollection, simple ) 
{
    au::CounterCollection<std::string> counter_collection;

    ASSERT_TRUE( counter_collection.appendAndGetCounterFor( "Hola" ) == 1 );
    ASSERT_TRUE( counter_collection.appendAndGetCounterFor( "Hola" ) == 2 );
    ASSERT_TRUE( counter_collection.appendAndGetCounterFor( "Hola" ) == 3 );

    ASSERT_TRUE( counter_collection.appendAndGetCounterFor( "Adios" ) == 1 );
    
}

TEST(StringComponents, simple ) 
{
    const char* line = "This is a line|to test|somehing|with StringComponents";
    au::StringComponents string_components;
    
    size_t ans = string_components.process_line( line , strlen(line) , '|' );
    
    ASSERT_TRUE( ans == strlen(line) );
    ASSERT_TRUE( string_components.components.size() == 4);
    ASSERT_TRUE( strcmp("This is a line", string_components.components[0] ) == 0);
    ASSERT_TRUE( strcmp("with StringComponents", string_components.components[3] ) == 0);
    
}

