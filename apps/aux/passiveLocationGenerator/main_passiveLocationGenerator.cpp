
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

int main( int argc , const char *argv[] )
{
	if ( argc < 2 )
	{
	   fprintf(stderr,"Usage %s rate_in_messages_per_second\n", argv[0] );
		exit(0);
	}

	size_t rate = atoll( argv[1] );

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
			size_t user_id = rand()%10000;
			int cell = rand()%20000;

			snprintf( line, 20000 ,  "<?xml version=\"1.0\" encoding=\"UTF-8\"?><ns0:AMRReport xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'  xmlns:ns0='http://O2.arcanum.vitria.com'  xsi:schemaLocation='http://O2.arcanum.vitria.com AMR.xsd'>  <SubscriberReport>    <User>      <IMSI>%lu</IMSI>      <PTMSI>FB869371</PTMSI>  <CellID>%d</CellID>   <Paging>      <Location>        <LocationArea>12124</LocationArea>        <RoutingArea>134</RoutingArea>      </Location>    </Paging>  </SubscriberReport>  <Timestamp>2011-07-21T16:07:47</Timestamp></ns0:AMRReport>" , user_id , cell );

		total_size += strlen(line);
		num_messages++;


		// Emit line to the output
		std::cout << line << "\n";

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

