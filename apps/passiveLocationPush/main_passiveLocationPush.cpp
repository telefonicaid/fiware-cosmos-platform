
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
	au::CommandLine cmd;
	cmd.set_flag_int( "timeOut" , 0 );       // 0 value for timeOut means no timeout
	cmd.set_flag_double( "rate" , 1.0 );       // 0 value for timeOut means no timeout
	cmd.set_flag_string("controller", "localhost");
	cmd.set_flag_string("file", "generator");
	cmd.parse( argc , argv );

	FILE *fp = NULL;

	if ( cmd.get_num_arguments() < 2 )
	{
		fprintf(stderr,"Usage %s queue [ -controller controller_ip]  [ -rate rate_in_megabytes_per_second ]  [ -timeOut timeout_in_seconds ] [-file input_file]\n" , argv[0] );
		exit(0);
	}


	std::string queue_name = cmd.get_argument(1);
	std::string controller = cmd.get_flag_string("controller");
	std::string file = cmd.get_flag_string("file");

	int timeOut = cmd.get_flag_int("timeOut");
	double rate = cmd.get_flag_double("rate");


	struct tm tm;
	time_t timeStamp = 0;
	time_t firstTimeStamp = 0;
	time_t offset_secs = 0;

	// Instance of the client to connect to SAMSON system
	samson::SamsonClient client;

	// Set 1G RAM for uploading content
	client.setMemory( 1024*1024*1024 );


	std::cerr << "Connecting to " << controller <<  " ..." ;
	std::cerr.flush();

	// Init connection

	if( !client.init( controller ) )
	{
		fprintf(stderr, "Error connecting with samson cluster (controller_ip:%s): %s\n" , controller.c_str(), client.getErrorMessage().c_str() );
		exit(0);
	}

	std::cerr << "OK\n";


	samson::SamsonPushBuffer *pushBuffer = new samson::SamsonPushBuffer( &client , queue_name , timeOut );

	std::cerr << "----------------------------------------\n";
	std::cerr << "Rate " << rate <<  " MBs\n";
	if( timeOut > 0)
		std::cerr << "TimeOut " << timeOut <<  " seconds\n";
	std::cerr << "----------------------------------------\n";

	// Small mini-buffer to generate
	char line[2048];

	// Control of time and size
	au::Cronometer cronometer;
	size_t total_size = 0;
	size_t num_messages = 0;



	if (!file.compare("generator"))
	{
		if ((fp = fopen(file.c_str(), "r")) == NULL)
		{
			fprintf(stderr, "Error opening file:'%s', errno:%d\n", file.c_str(), errno);
			perror(file.c_str());
			exit(1);
		}
	}

	while( true )
	{
		char *pTime;
		char *p_sep;

		if (!file.compare("generator"))
		{

			size_t used_id = rand()%40000000;
			int cell = rand()%20000;

			//snprintf( line, 1024 , "<message> <id> %lu </id> <cell> %d </cell> <time> %s </time>\n", used_id , cell , au::todayString().c_str()  );
			snprintf( line , sizeof( line ) , "<?xml version=\"1.0\" encoding=\"UTF-8\"?><ns0:AMRReport xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'  xmlns:ns0='http://O2.arcanum.vitria.com'  xsi:schemaLocation='http://O2.arcanum.vitria.com AMR.xsd'>  <SubscriberReport>    <User>      <IMSI>%lu</IMSI>      <IMEI>3563190407579307</IMEI>    </User>    <Authentication>      <Location>        <LocationArea>12115</LocationArea>        <CellID>%d</CellID>        <RoutingArea>134</RoutingArea>      </Location>    </Authentication>  </SubscriberReport>  <Timestamp>%s</Timestamp></ns0:AMRReport>\n", used_id , cell , au::todayString().c_str()  );
		}
		else
		{
			if (fgets(line, sizeof( line ), fp) == NULL)
			{
				rewind(fp);
				if (firstTimeStamp != 0)
				{
					offset_secs += (timeStamp - firstTimeStamp);
				}
				continue;
			}
#define STR_TIMESTAMP_BEGIN "<Timestamp>"
			if ((pTime = strstr(line, STR_TIMESTAMP_BEGIN)) != NULL)
			{
				pTime += strlen(STR_TIMESTAMP_BEGIN);
				if ((p_sep = strchr(pTime, '<')) != NULL)
				{
					*p_sep = '\0';
					strptime(pTime, "%Y-%m-%dT%H:%M:%S", &tm);
					timeStamp = mktime(&tm);
					if (offset_secs != 0)
					{
						timeStamp += offset_secs;
						localtime_r(&timeStamp, &tm);
						strftime(pTime, 20, "%Y-%m-%dT%H:%M:%S", &tm);
					}
					else if (firstTimeStamp == 0)
					{
						firstTimeStamp = timeStamp;
					}
					*p_sep = '<';
				}
			}
		}

		size_t line_length  = strlen(line) + 1;
		total_size += line_length;
		num_messages++;


				pushBuffer->push( line, line_length );




		// Detect if we need to sleep....
		int theoretical_seconds = ( (double) total_size / ( (double) (1024*1024) * (double) rate ) );
		int ellapsed_seconds = cronometer.diffTimeInSeconds();

		// Sleep some time to simulate a particular rate
		if( ellapsed_seconds < theoretical_seconds )
		{
			int sleep_seconds = theoretical_seconds - ellapsed_seconds;
			std::cerr << "Sleeping " << sleep_seconds << " seconds... num messages " << au::str(num_messages) << " size " << au::str( total_size , "bytes") << " time " << au::time_string(ellapsed_seconds) << " theoretical time " << au::time_string(theoretical_seconds)<<"\n";
			sleep( sleep_seconds );
		}

	}

}

