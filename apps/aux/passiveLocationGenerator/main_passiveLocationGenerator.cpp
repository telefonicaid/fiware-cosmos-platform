
/*

 passiveLocationPush

 Example app using samsonClient lib
 It generates random xml documents simulating information from OSS Passive Location pilot

 AUTHOR: Andreu Urruela

 */


#include <stdio.h>      // printf
#include <stdlib.h>     // exit
#include <string.h>     // memcpy
#include <iostream>     // std::cout
#include <time.h>       // strptime, struct tm


#include "au/time.h"            // au::todatString()
#include "au/string.h"          // au::str()
#include "au/Cronometer.h"      // au::Cronometer
#include "au/CommandLine.h"     // au::CommandLine

#include "samson/client/SamsonClient.h"         // samson::SamsonClient

bool random_user = false;
size_t last_user = 0;


int main( int argc , const char *argv[] )
{
   au::CommandLine cmd;
   cmd.set_flag_int( "users" , 100000 );
   cmd.parse( argc , argv );

   int num_users = cmd.get_flag_int("users");

   if( cmd.get_num_arguments() < 2 )
   {
	   fprintf(stderr,"Usage %s rate_in_messages_per_second -users <num_users>\n", argv[0] );
		exit(0);
   }

	size_t rate = atoll( argv[1] );
	size_t max_kvs = 0;
	if( argc > 2 )
	  max_kvs = atoll( argv[2] );

	// Small mini-buffer to generate
	char *line = (char*) malloc( 20000 );

	// Control of time and size
	au::Cronometer cronometer;

	size_t total_size = 0;
	size_t num_messages = 0;

	size_t theoretical_seconds = 0;

	while( true )
	{
	   // Generat 5 seconds data
	   fprintf(stderr,"Generatoing %d messages\n",  (int)(5 * rate) );
	   for( int i = 0 ; i < (int)(5 * rate); i++)
	   {

		  size_t user_id;
		  if( random_user )
			 user_id = rand()%num_users;
		  else
		  {
			 last_user = (last_user+1)%num_users;
			 user_id = last_user;
		  }


		  int cell = 65528;
		 
		  if( (time(NULL)%200)> 100 )
			 cell = 65534;
		  
			snprintf( line, 20000 ,  "<?xml version=\"1.0\" encoding=\"UTF-8\"?><ns0:AMRReport xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'  xmlns:ns0='http://O2.arcanum.vitria.com'  xsi:schemaLocation='http://O2.arcanum.vitria.com AMR.xsd'>  <SubscriberReport>    <User>      <IMSI>%lu</IMSI>      <PTMSI>FB869371</PTMSI>  <CellID>%d</CellID>   <Paging>      <Location>        <LocationArea>12124</LocationArea>        <RoutingArea>134</RoutingArea>      </Location>    </Paging>  </SubscriberReport>  <Timestamp>2011-07-21T16:07:47</Timestamp></ns0:AMRReport>" , user_id , cell );

		total_size += strlen(line);
		num_messages++;


		// Emit line to the output
		std::cout << line << "\n";

	   }

	   if( max_kvs > 0 )
	     if( num_messages > max_kvs )
	       {
		 fprintf(stderr,"Finish since a limit of %lu kvs was specified. Generated %lu key-vaue\n", max_kvs ,num_messages);

		 return 0;
	       }

		// Detect if we need to sleep....
		theoretical_seconds += 5;

		size_t ellapsed_seconds = cronometer.diffTimeInSeconds();

		// Sleep some time to simulate a particular rate
		if( ellapsed_seconds < theoretical_seconds )
		{
			int sleep_seconds = theoretical_seconds - ellapsed_seconds;
			std::cerr << "Sleeping " << sleep_seconds << " seconds... num messages " << au::str(num_messages) << " size " << au::str( total_size , "bytes") << " time " << au::time_string(ellapsed_seconds) << " theoretical time " << au::time_string(theoretical_seconds)<<"\n";
			sleep( sleep_seconds );
		}

	}

}

