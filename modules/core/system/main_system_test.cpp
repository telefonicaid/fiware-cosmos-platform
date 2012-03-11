

#include <stdio.h>
#include "au/string.h"

#include <samson/module/DataInstance.h>
#include <samson/modules/system/Value.h>

#include "operations/Filter.h"
#include "operations/map_value_to_value.h"



void test( std::string txt )
{

    printf("-----------------------------------\n");
    printf("Testing %s\n" , txt.c_str());
    printf("-----------------------------------\n");

    au::ErrorManager error;
    samson::system::SamsonTokenVector token_vector;
    token_vector.parse(txt);
    
    printf("Tokens > %s\n" , token_vector.str().c_str() );
    
    samson::system::Source* source = samson::system::getSource( &token_vector, &error);

    printf("-----------------------------------\n");

    if( error.isActivated() )
        printf("Error %s\n" , error.getMessage().c_str() );
    else
    {
        printf( "%s\n" , source->str().c_str() );
    }
    printf("-----------------------------------\n\n\n");
    
    
}

int main()
{

    test("key");    
    test("key[0]");
    test("key:[andreu]");
    test("key:['andreu']");
 
    test("[ value:['name'] key[0] key ]");
 
    test("key:[key[key:[pepe]]]");

    test("{ name: key[0] 'value': value }");
    test("{ name:key:[key[key:[pepe]]]  surname:1 }");

    test("{ name:key:[key[key:[pepe]]]  surname:str(key[0]) }");
    return 0;
   
    
}
