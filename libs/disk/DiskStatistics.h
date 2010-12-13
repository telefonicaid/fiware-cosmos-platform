#ifndef _H_DISK_STATISTICS
#define _H_DISK_STATISTICS

#include <sys/time.h>
#include <string>				// std::string

#define STATISTICS_BUFFER_SIZE 100

namespace ss {

	class DiskStatisticsItem
	{

	public:

		// Buffer of samples
		
		size_t size[ STATISTICS_BUFFER_SIZE ];
		size_t time[ STATISTICS_BUFFER_SIZE ];
		
		int pos;		// Position writing information
		int length;		// Number of samples
		
		DiskStatisticsItem();
		
		void add( size_t _size , size_t _mili_seconds );

		std::string getStatus();
		
		
	};	


	class DiskStatistics
	{

	public:
		
		DiskStatisticsItem item_read;
		DiskStatisticsItem item_write;
		DiskStatisticsItem item_total;

		typedef enum 
		{
			write,
			read
		}Operation;
		
		void add( Operation type, size_t size , size_t micro_seconds );
		
		std::string getStatus();
		
		static size_t timevaldiff(struct timeval *starttime, struct timeval *finishtime);

	};
}

#endif

