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

#include <stdlib.h>
#include "gtest/gtest.h"

#include "samson_system/Value.h"
#include "samson_system/ValueContainer.h"


void  check_serialization( samson::system::Value* value , char* line , size_t max_size )
{

   size_t s = value->serialize( line );

   EXPECT_TRUE( s <= max_size ) << au::str( "Excesive size to serialize value %s ( used %lu when max is %lu) " , value->str().c_str() , s , max_size ).c_str();   


   samson::system::Value* value2 = samson::system::Value::getInstance();
   size_t s2 = value2->parse( line );

   EXPECT_TRUE( s == s2 ) << au::str( "Different serialization size for value %s (write %lu read %lu)" , value->str().c_str() , s , s2 ).c_str();      
   EXPECT_TRUE( *value == *value2 ) << au::str( "Different write and read value %s != %s" , value->str().c_str() , value2->str().c_str() ).c_str();   

   samson::system::Value::reuseInstance( value2 );

}


TEST( samson_system , ser_string )
{
   char *line = (char*) malloc( sizeof(char) * 64000 );
   
   samson::system::ValueContainer value_container;

   // Check simple serialization
   value_container.value->set_string("Pepe");
   check_serialization( value_container.value , line , 6 );

   value_container.value->set_double(1);
   check_serialization( value_container.value , line , 6 );

   value_container.value->set_double(1.5);
   check_serialization( value_container.value , line , 6 );

   value_container.value->set_string_for_map("app","top");
   check_serialization( value_container.value , line , 100 );

   value_container.value->set_string_for_map("app","top");
   check_serialization( value_container.value , line , 100 );


   value_container.value->set_as_vector();
   value_container.value->add_value_to_vector()->set_string( "Hola" );
   value_container.value->add_value_to_vector()->set_string( "Que tal?" );
   value_container.value->add_value_to_vector()->set_string( "Adios" );
   check_serialization( value_container.value , line , 100 );

   free( line );
}


TEST( samson_system , ser_vector )
{
   char *line = (char*) malloc( sizeof(char) * 64000 );

   samson::system::ValueContainer value_container;

   value_container.value->set_as_vector();
   value_container.value->add_value_to_vector()->set_string( "Hola" );
   value_container.value->add_value_to_vector()->set_string( "Que tal?" );
   value_container.value->add_value_to_vector()->set_string( "Adios" );
   check_serialization( value_container.value , line , 100 );


   value_container.value->set_as_vector();

   samson::system::Value* v1 = value_container.value->add_value_to_vector();
   v1->set_string_for_map("map1","value");
   v1->set_string_for_map("map2","value2");

   samson::system::Value* v2 = value_container.value->add_value_to_vector();
   v2->set_string_for_map("map1","value");
   v2->set_string_for_map("map2","value2");

   value_container.value->add_value_to_vector()->set_string( "Que tal?" );

   check_serialization( value_container.value , line , 100 );

   free( line );
}


TEST( samson_system , ser_full )
{
   char *line = (char*) malloc( sizeof(char) * 64000 );

   samson::system::ValueContainer value_container;

   value_container.value->set_as_vector();

   samson::system::Value* v1 = value_container.value->add_value_to_vector();
   v1->set_string_for_map("map1","value");
   v1->set_string_for_map("map2","value2");

   samson::system::Value* v2 = value_container.value->add_value_to_vector();
   v2->set_string_for_map("map1","value");
   v2->set_string_for_map("map2","value2");

   value_container.value->add_value_to_vector()->set_string( "Que tal?" );

   check_serialization( value_container.value , line , 100 );

   free( line );
}
