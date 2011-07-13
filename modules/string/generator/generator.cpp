

#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include <time.h>

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
  srand( time(NULL) );

  if( argC < 2 )
    {
      printf("Usage: %s size\n", argV[0] );
      exit(0);
    }

  size_t max_size = atoll( argV[1] );

  size_t total_size = 0;

  while( total_size < max_size )
  {

    char line[1024];

    // 6 letter words
    int line_length = 6;
    for (int i = 0 ; i < line_length ; i++ )
      line[i] = 97 + rand()%20;
    line[line_length]='\0';

    total_size += printf("%s\n", line );   
  }

  fprintf( stderr , "Total generated %lu bytes\n", total_size );

  return 0;
}
