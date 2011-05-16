

#include <stdio.h>
#include <cstring>
#include <stdlib.h>

size_t randNumber()
{
  size_t num;
  char *_num = (char*) &num;
  for (int i = 0 ; i < 8 ; i++ )
    _num[i] = rand()%255;

  return num;

}

int main( int argC, char *argV[] )
{

  if( argC < 2 )
    {
      printf("Usage: %s size\n", argV[0] );
      exit(0);
    }

  size_t max_size = atoll( argV[1] );

  size_t total_size = 0;

  while( total_size < max_size )
  {
    size_t value = randNumber();
    total_size += printf("%lu\n", value );   
  }

  fprintf( stderr , "Total generated %lu bytes\n", total_size );

  return 0;
}
