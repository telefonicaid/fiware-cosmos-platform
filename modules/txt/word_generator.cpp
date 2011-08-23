

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define WORD_LENGTH    10
#define ALFABET_LENGTH 50

int main( int args , char*argv[] )
{

  srand( time(NULL));
  if ( args < 2 )
  {
    fprintf(stderr, "Usage: %s size\n" , argv[0] );
    exit(1);
  }

  size_t total_size = atoll( argv[1] );
  size_t size = 0;

  char word[100];
  word[WORD_LENGTH] = '\0';

  while( size < total_size )
  {
    for ( int i = 0 ; i < WORD_LENGTH ; i++ )
      word[i] = 48 + rand()%ALFABET_LENGTH;

    size += printf("%s\n",word);

  }
  

}
