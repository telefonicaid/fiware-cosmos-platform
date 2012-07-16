

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

size_t llrand( size_t max )
{
   size_t r = 0;

   for (int i = 0; i < (int)(sizeof(long)/sizeof(size_t)); i++)
   {
	  r = r << ( sizeof(int) * 8 );
	  r |= rand();
   }

   size_t result = r%max;

   //printf("R:%lu\n", result );
   return result;;
}

int main( int args , char*argv[] )
{

  srand( time(NULL));
  if ( args < 3 )
  {
    fprintf(stderr, "Usage: %s num_nodes num_average_connections\n" , argv[0] );
    exit(1);
  }

  size_t num_nodes = atoll( argv[1] );
  size_t num_connections = atoll( argv[2] );

  for (size_t i = 0 ; i < num_nodes ; i++ )
  {
	 printf("%lu " , i);

	 for (size_t j = 0 ; j < num_connections ; j++ )
	 {
		size_t node = llrand( num_nodes );
		while( node == j)
		   node = llrand(num_nodes);

		printf("%lu" , node );

		if( j != (num_connections-1) )
		   printf(" ");

	 }

	 printf("\n");
  }


}
