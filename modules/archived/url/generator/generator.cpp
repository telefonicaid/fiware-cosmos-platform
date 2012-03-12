

#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include <time.h>

#define  NUM_SERVERS   150000000  // 150M domains
#define  NUM_USERS     400000000

size_t randNumber( size_t max_num )
{
  size_t num;
  char *_num = (char*) &num;
  for (int i = 0 ; i < 8 ; i++ )
    _num[i] = rand()%255;

  return num%max_num;

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

  size_t time=0;
  int counter=0;

  while( total_size < max_size )
  {
    size_t user   = randNumber( NUM_USERS );
    size_t server = randNumber( NUM_SERVERS );

    // Server 100 selected to be the top one
	if( rand()%100 == 0 )
	   server = 100;

    if( counter++ == 20000000 ) // Simulating 20M url / second
    {
      counter = 0;
      time++;
    }

    total_size += printf("http://domain_server_%lu/some_path_to_some_page/index.html %lu %lu\n", server , time,  user );   

  }

  fprintf( stderr , "Total generated %lu bytes\n", total_size );

  return 0;
}
