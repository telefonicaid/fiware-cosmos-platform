

#include <stdio.h>
#include "au/string.h"

#include <samson/module/DataInstance.h>
#include <samson/modules/system/Value.h>
#include "samson_system/Filter.h"




void test( std::string txt )
{

    printf("-----------------------------------\n");
    printf("Testing %s\n" , txt.c_str());
    printf("-----------------------------------\n");

    au::ErrorManager error;
	samson::system::SamsonTokenizer tokenizer;

	au::token::TokenVector token_vector = tokenizer.parse( txt );
    
    printf("Tokens > %s\n" , token_vector.str().c_str() );
    
    samson::system::Source* source = samson::system::getSource( &token_vector, &error);

    printf("-----------------------------------\n");

    if (error.IsActivated())
        printf("Error %s\n", error.GetMessage().c_str());
    else
    {
        printf( "%s\n" , source->str().c_str() );
    }
    printf("-----------------------------------\n\n\n");
    
    
}


void test_sources()
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
}


void test_selialization( double v )
{
    char data[1000];
    samson::system::Value value, value2;
    value.set_double(v);
    size_t size = value.serialize(data);
    size_t size2 = value2.parse(data);

    if( size != size2 )
        printf("Error serialiazing %f ( Sizes %lu != %lu )\n" , v , size , size2 );

    if( !value2.isNumber() )
        printf("Error serialiazing %f: Not a value recovered \n" , v );

    double v2 = value2.get_double();
    if( v != v2 )
        printf("Error serialiazing %f: Recovered %f \n" , v , v2 );
        
    
    printf("Value %f is serialized using %lu bytes\n" , v , size );
}

void test_selialization( std::string txt )
{
    char data[10000];
    samson::system::Value value, value2;
    value.set_string(txt);
    size_t size = value.serialize(data);
    size_t size2 = value2.parse(data);
    
    if( size != size2 )
    {
        printf("Error serialiazing %s ( Sizes %lu != %lu )\n" , txt.c_str() , size , size2 );
        exit(0);
    }
    
    if( !value2.isString() )
    {
        printf("Error serialiazing %s: Not a string recovered \n" , txt.c_str() );
        exit(0);
    }
    
    std::string txt2 = value2.get_string();
    if( txt != txt2 )
    {
        printf("Error serialiazing '%s (length %lu)'\n" , txt.c_str() , txt.length() );
        printf("RECOVERED: '%s'\n" , txt2.c_str() );
        exit(0);
    }
    
    printf("String %s (length %lu) is serialized using %lu bytes\n" , txt.c_str() , txt.length() , size );
    
}

void test_selialization( )
{

    for (int i = 0 ; i < 15 ; i++ )
        test_selialization(i);
    test_selialization(100);
    test_selialization(1000);
    test_selialization(10000);
    test_selialization(100000);
    test_selialization(1000000);
    test_selialization(10000000);
    test_selialization(100000000);
    test_selialization(10000000000);
    test_selialization(100000000000);
    
    test_selialization(0.1);
    test_selialization(0.12);
    test_selialization(0.123);
    test_selialization(0.1234);
    test_selialization(0.12345);
    test_selialization(0.123456);

    test_selialization(-0.1);
    test_selialization(-0.12);
    test_selialization(-0.123);
    test_selialization(-0.1234);
    test_selialization(-0.12345);
    test_selialization(-0.123456);
    
    test_selialization("Andreu");
    test_selialization("Andrés");
    test_selialization("user");
    test_selialization("log");
    test_selialization("url");

    test_selialization("En un lugar de la mancha de cuyo nombre no quiero acordarme,");

    test_selialization("http://www.google.com");
    test_selialization("Esto es una frase como qualquier otra");

    /*
    test_selialization("Esto es una prueba con un string la leche de largo porque he observado que cuando el string pasa de cierto numero de caracteres pasan cosas muy raras.Esto es una prueba con un string la leche de largo porque he observado que cuando el string pasa de cierto numero de caracteres pasan cosas muy raras.Esto es una prueba con un string la leche de largo porque he observado que cuando el string pasa de cierto numero de caracteres pasan cosas muy raras.Esto es una prueba con un string la leche de largo porque he observado que cuando el string pasa de cierto numero de caracteres pasan cosas muy raras.Esto es una prueba con un string la leche de largo porque he observado que cuando el string pasa de cierto numero de caracteres pasan cosas muy raras.Esto es una prueba con un string la leche de largo porque he observado que cuando el string pasa de cierto numero de caracteres pasan cosas muy raras.Esto es una prueba con un string la leche de largo porque he observado que cuando el string pasa de cierto numero de caracteres pasan cosas muy raras.Esto es una prueba con un string la leche de largo porque he observado que cuando el string pasa de cierto numero de caracteres pasan cosas muy raras.Esto es una prueba con un string la leche de largo porque he observado que cuando el string pasa de cierto numero de caracteres pasan cosas muy raras.Esto es una prueba con un string la leche de largo porque he observado que cuando el string pasa de cierto numero de caracteres pasan cosas muy raras.Esto es una prueba con un string la leche de largo porque he observado que cuando el string pasa de cierto numero de caracteres pasan cosas muy raras.Esto es una prueba con un string la leche de largo porque he observado que cuando el string pasa de cierto numero de caracteres pasan cosas muy raras.Esto es una prueba con un string la leche de largo porque he observado que cuando el string pasa de cierto numero de caracteres pasan cosas muy raras.Esto es una prueba con un string la leche de largo porque he observado que cuando el string pasa de cierto numero de caracteres pasan cosas muy raras.Esto es una prueba con un string la leche de largo porque he observado que cuando el string pasa de cierto numero de caracteres pasan cosas muy raras.");
    */
    
}


void test_xml_parser()
{
}


int main()
{
    //test_sources();
    //test_selialization();
    
    
    return 0;
}
