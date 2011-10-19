

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#include "parseArgs/parseArgs.h"
#include "logMsg/logMsg.h"

#include "au/string.h" // au::str()
#include "au/Cronometer.h"  // au::Cronometer
#include "au/CommandLine.h" // au::CommandLine

#define BUFFER_SIZE 10000000


size_t full_read( int fd , char* data , size_t size)
{
  size_t read_size = 0;

  while( read_size < size )
    {
      ssize_t t = read( fd , data+read_size , size - read_size );

      if( t==-1)
	LM_X(1,("Error reading input data"));

      if( t == 0)
	break;
      else
	read_size+=t;
    }

  return read_size;
}



int main( int args , char*argv[] )
{

  if ( args < 2 )
  {
    fprintf(stderr, "Usage: %s limit(bytes/second)\n" , argv[0] );
    exit(1);
  }

  au::Cronometer cronometer;


  size_t max_rate = atoll( argv[1] );

  fprintf(stderr, "%s: max rate %s\n", argv[0] , au::str( max_rate , "Bs").c_str() );

  char *buffer = (char*) malloc( BUFFER_SIZE );

  size_t total = 0;
  ssize_t tmp_size = 0;

  while( true )
  {

    size_t r = full_read( 0 , buffer , BUFFER_SIZE );

    if( r == 0)
      break;

    size_t theoretical_time = (double)total / (double) max_rate;
    size_t real_time = cronometer.diffTimeInSeconds();
    int diff_seconds = theoretical_time - real_time;

    if ( real_time == 0 )
      real_time = 1;



    if( diff_seconds > 0 )
      {
	size_t current_rate = (double) total / (double) ( real_time + diff_seconds );  // Current rate with correction
        fprintf(stderr,"%s: Sleeping %d seconds to keep max rate %s ( Current rate %s ) \n", argv[0] , diff_seconds , au::str( max_rate ,"Bs" ).c_str() , au::str(current_rate).c_str() );
	sleep( diff_seconds );
      }

    // Write at the output
    total += r;
    tmp_size = write( 1 , buffer , r );

  }
}
