

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#include "au/string.h" // au::str()
#include "au/Cronometer.h"  // au::Cronometer
#include "au/CommandLine.h" // au::CommandLine

#include "au/CommandLine.h"     // au::CommandLine


#define WORD_LENGTH     9
#define ALFABET_LENGTH 10

int main( int args , const char*argv[] )
{
  au::CommandLine cmd;
  cmd.set_flag_boolean( "r");       // by default, fixed sequence of words
  cmd.set_flag_int( "t" , 0 );       // by default, 0 seconds to repeat
  cmd.parse (args, argv);


  if ( cmd.get_num_arguments() < 2 )
  {
    fprintf(stderr, "Usage: %s [-r] [-t secs] num_words\n" , argv[0] );
    fprintf(stderr, "       [-r] flag to generate randomized sequence of words\n" );
    fprintf(stderr, "       [-t secs] time_to_repeat in seconds with a new wave of words\n" );
    exit(1);
  }

  bool rand_flag = cmd.get_flag_bool("r");
  size_t repeate_time = cmd.get_flag_int("t");

  size_t num_lines = atoll( cmd.get_argument(1).c_str() );
  au::Cronometer cronometer;
  
  char word[100];
  word[WORD_LENGTH] = '\0';


  size_t total_num = 0;
  size_t total_size = 0;

  if (rand_flag)
  {
    srand( time(NULL));
  }
  else
  {
    srand(0);
  }


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
