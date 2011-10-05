

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#include "au/string.h" // au::str()
#include "au/Cronometer.h"  // au::Cronometer
#include "au/CommandLine.h" // au::CommandLine

#define WORD_LENGTH     9
#define ALFABET_LENGTH 10

int main( int args , char*argv[] )
{


  srand( time(NULL));
  if ( args < 2 )
  {
    fprintf(stderr, "Usage: %s num_words [time_to_repeat]\n" , argv[0] );
    exit(1);
  }

  au::Cronometer cronometer;

  size_t num_lines = atoll( argv[1] );
  size_t repeate_time;
  
  if( args > 2 )
	 repeate_time = atoll( argv[2] );
  else
	 repeate_time = 0;



  char word[100];
  word[WORD_LENGTH] = '\0';


  size_t total_num = 0;
  size_t total_size = 0;


  while( true )
  {

	 size_t local_num_lines = 0;

	 while( local_num_lines < num_lines )
	 {
		for ( int i = 0 ; i < WORD_LENGTH ; i++ )
		   word[i] = 48 + rand()%ALFABET_LENGTH;
		
		total_size += printf("%s\n",word);
		total_num++;

		local_num_lines++;
		
	 }


	 if( repeate_time == 0 )
		exit(1);
	 else
	 {
		sleep( repeate_time );
		size_t total_seconds = cronometer.diffTimeInSeconds();
		fprintf(stderr,"%s: Generated %s lines ( %s bytes ) in %s. Rate: %s / %s. Now sleeping %d seconds\n" 
				, argv[0] , au::str(total_num).c_str() , au::str(total_size).c_str(), au::time_string( total_seconds ).c_str() ,
 au::str( (double)total_num/(double)total_seconds ,"Lines/s" ).c_str() , au::str( (double)total_size/(double)total_seconds,"Bps").c_str() , (int)repeate_time  );
	 }

  }

}
