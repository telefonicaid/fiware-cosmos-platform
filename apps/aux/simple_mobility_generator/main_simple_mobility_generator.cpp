

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#include "au/string.h" // au::str()
#include "au/Cronometer.h"  // au::Cronometer
#include "au/CommandLine.h" // au::CommandLine


typedef struct 
{
   int x;
   int y;
	
	void init ()
	{
		x=500;
		y=500;
	}

   void limits( int* var )
   {
	  if( *var <  0)
		 *var = 0;
	  if( *var > 1000 )
		 *var = 1000;
   }

	void step()
	{
	   x+= rand()%3 - 1;
	   y+= rand()%3 - 1;

	   limits(&x);
	   limits(&y);

	}
	
} User;

User *users;

int main( int args , char*argv[] )
{

  srand( time(NULL));
  if ( args < 2 )
  {
    fprintf(stderr, "Usage: %s num_users [rate_in_events_per_seconds] ( default 10Kevents/sec )  [max_time (default 0 - no limit) ]\n" , argv[0] );
    exit(1);
  }

  size_t num_users = atoll( argv[1] );
  size_t rate;
  
  if( args > 2 )
	 rate = atoll( argv[2] );
  else
	 rate = 10000;


  size_t max_time = 0;
  if( args > 3 )
  {
	 max_time = atoll( argv[3] );
  }

  // Init users
  users = (User*) malloc( sizeof( User) * num_users );
  for ( size_t i = 0 ; i < num_users ; i++ )
	 users[i].init();

  size_t total_num = 0;
  size_t total_size = 0;


  au::Cronometer cronometer;
  size_t theoretical_seconds = 0;

  while( true )
  {

	 theoretical_seconds += 5; // 5 seconds generating data

	 for ( size_t i = 0 ; i < 5*rate ; i++ )
	 {
		 size_t user = rand()%num_users;

		 users[user].step();
 		 total_size += printf("%lu CDR %d %d %lu\n", user , users[user].x , users[user].y , time(NULL) );
		 total_num++;
	 }

	 if( max_time > 0 )
		if( theoretical_seconds > max_time )
		{
		   fprintf(stderr,"%s: Finish generating data for time limit\n" , argv[0] );
		   return 0;
		}
	 
	 size_t total_seconds = cronometer.diffTimeInSeconds();

	 if( total_seconds < theoretical_seconds )
	 {
		int seconds_to_sleep = (int) theoretical_seconds - total_seconds;
		fprintf(stderr,"%s: Sleeping %d seconds to keep rate %s\n", argv[0] , seconds_to_sleep , au::str( rate , "Events/sec" ).c_str() );
		sleep( seconds_to_sleep );
	 }

	 if( (theoretical_seconds%10) == 0)
	 {
		fprintf(stderr,"%s: Generated %s lines ( %s bytes ) in %s. Rate: %s / %s.\n" 
			 , argv[0] , au::str(total_num).c_str() , au::str(total_size).c_str(), au::time_string( total_seconds ).c_str() ,
			 au::str( (double)total_num/(double)total_seconds ,"Lines/s" ).c_str() , au::str( (double)total_size/(double)total_seconds,"Bps").c_str()  );
	 }


  }

}
