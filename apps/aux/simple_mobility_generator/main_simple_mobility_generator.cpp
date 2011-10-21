

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
		x=0;
		y=0;
	}

	void step()
	{
	   x+= rand()%11 - 5;
	   y+= rand()%11 - 5;
	}
	
} User;

User *users;

int main( int args , char*argv[] )
{

  srand( time(NULL));
  if ( args < 2 )
  {
    fprintf(stderr, "Usage: %s num_users [time_to_repeat_in_secs ( (default 10s) ]\n" , argv[0] );
    exit(1);
  }


  size_t num_users = atoll( argv[1] );
  size_t repeate_time;
  
  if( args > 2 )
	 repeate_time = atoll( argv[2] );
  else
	 repeate_time = 10;


  // Init users
  users = (User*) malloc( sizeof( User) * num_users );
  for ( size_t i = 0 ; i < num_users ; i++ )
	 users[i].init();

  size_t total_num = 0;
  size_t total_size = 0;


  au::Cronometer cronometer;


  while( true )
  {

	 for ( size_t i = 0 ; i < num_users ; i++ )
	 {
		//size_t user = rand()%num_users;
		 size_t user = i;

		 users[user].step();
 		 total_size += printf("%lu CDR %d %d\n", user , users[user].x , users[user].y );
		 total_num++;
	 }



	 size_t total_seconds = cronometer.diffTimeInSeconds();

	 fprintf(stderr,"%s: Generated %s lines ( %s bytes ) in %s. Rate: %s / %s.\n" 
			 , argv[0] , au::str(total_num).c_str() , au::str(total_size).c_str(), au::time_string( total_seconds ).c_str() ,
			 au::str( (double)total_num/(double)total_seconds ,"Lines/s" ).c_str() , au::str( (double)total_size/(double)total_seconds,"Bps").c_str()  );

	 sleep( repeate_time );	 

  }

}
