/* ****************************************************************************
 *
 * FILE            DiskStatistics
 *
 * AUTHOR          Andreu Urruela
 *
 * DATE            July 2011
 *
 * DESCRIPTION
 *
 * Information about the statustics for all the input / output statistics on disk 
 * Also used to monitorize network traffic
 *
 * ****************************************************************************/

#ifndef _H_DISK_STATISTICS
#define _H_DISK_STATISTICS

#include <sys/time.h>
#include <string>                       // std::string

#include "au/Rate.h"                    // au::Rate

#include "engine/DiskOperation.h"		// engine::DiskOperationType

#define STATISTICS_BUFFER_SIZE 100

namespace engine {

	class DiskOperation;
	
	class DiskStatisticsItem
	{
        // All the information about the rate
        au::Rate rate;

	public:

		DiskStatisticsItem();
		
		void add( size_t _size );

		std::string getStatus();
        
        size_t getLastMinuteRate()
        {
            return rate.getLastMinuteRate();
        }
        
        void getInfo( std::ostringstream& output)
        {
            output << "<description>" << rate.str() << "</description>\n";
            au::xml_simple( output , "rate" , rate.getLastMinuteRate() );
        }
        
		
	};	

	class DiskStatistics
	{

	public:
		
		DiskStatisticsItem item_read;
		DiskStatisticsItem item_write;
		DiskStatisticsItem item_total;
        
		void add( DiskOperation::DiskOperationType type, size_t size );

		//void add( DiskOperation *operation );
		
		std::string getStatus();
		
		static size_t timevaldiff(struct timeval *starttime, struct timeval *finishtime);

		static size_t timeSince(struct timeval *starttime)
		{
			struct timeval now;
			gettimeofday(&now, NULL);
			return timevaldiff(starttime, &now);
		}
        
        void getInfo( std::ostringstream& output)
        {
            output << "<read>\n";
            item_read.getInfo( output );
            output << "</read>\n";
            
            output << "<write>\n";
            item_write.getInfo( output );
            output << "</write>\n";

            output << "<total>\n";
            item_total.getInfo( output );
            output << "</total>\n";
            
        }
		
	};
}

#endif

