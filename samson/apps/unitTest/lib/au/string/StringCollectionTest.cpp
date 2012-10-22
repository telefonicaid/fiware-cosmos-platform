
#include "au/string/StringCollection.cpp"
#include "gtest/gtest.h"


TEST(au_string_StringCollection , basic) {

   au::StringCollection string_collection;

   int a = string_collection.Add("a");
   int b = string_collection.Add("b");
   int c = string_collection.Add("ccc");

   int a2 = string_collection.Add("a");
   int b2 = string_collection.Add("b");
   int c2 = string_collection.Add("ccc");

   EXPECT_NE( a , b );
   EXPECT_NE( a , c );
   EXPECT_NE( b , c );

   EXPECT_EQ( a , a2 );
   EXPECT_EQ( b , b2 );
   EXPECT_EQ( c , c2 );
   
}


TEST( au_string_StringCollection , write_and_read )
{

   au::StringCollection string_collection;

   int a = string_collection.Add("a");
   int b = string_collection.Add("b");
   int c = string_collection.Add("ccc");

   // Write to file
   size_t s = string_collection.GetSize();
   FILE* file = fopen("/tmp/string_collection_test" , "w" );
   EXPECT_EQ( string_collection.Write( file ) , s );
   fclose( file );
   
   // Read from file
   au::StringCollection string_collection2;
   FILE* file2 = fopen("/tmp/string_collection_test" , "r" );
   string_collection2.Read( file2 , s );

  EXPECT_TRUE( strcmp( string_collection2.Get(a) , "a" ) == 0 );
  EXPECT_TRUE( strcmp( string_collection2.Get(b) , "b" ) == 0 );
  EXPECT_TRUE( strcmp( string_collection2.Get(c) , "ccc" ) == 0 );

}
