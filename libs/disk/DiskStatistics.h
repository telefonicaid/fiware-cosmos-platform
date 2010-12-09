#ifndef _H_DISK_STATISTICS
#define _H_DISK_STATISTICS

#include <sys/time.h>


class DiskStatisticsItem
{

public:
	
	size_t total_size;		// Total size in bytes	
	size_t average;			// Average speed in bytes / second	
	int total;				// Number os samples
	
	DiskStatisticsItem()
	{
		total = 0;
		total_size = 0;
		average = 0;
	}	
	
	void add( size_t size , size_t mili_seconds )
	{
		
		total++;
		total_size += size;
		size_t rate =  (size_t)( (double)1000.0 * ((double)size / (double)mili_seconds) ); 
		
		if( total == 1)
			average = rate;
		else
		{
			double factor;	// size > 10Mb --> 1
			if( size < 5*10*1024*1024 )
				factor = 0.2;
			else
			{
				factor = ( (double) 10*1024*1024 / (double) size );
			}
			
			average = (factor)*average + (1.0-factor)*rate;
		}
	}
	std::string getStatus()
	{
		std::ostringstream output;
		output << "[";
		output << "F:" << total << " ";
		output << "S:" << au::Format::string( total_size ,"b" ) << " ";
		output << "R: " << au::Format::string( average ,"bps" );
		output << "]";
		
		return output.str();
	}
	
	
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
	
	void add( Operation type, size_t size , size_t micro_seconds )
	{
		switch (type) {
			case read:
				item_read.add( size, micro_seconds);
				break;
			case write:
				item_write.add( size, micro_seconds);
				break;
		}
		
		item_total.add( size, micro_seconds);
	}
	
	std::string getStatus()
	{
		std::ostringstream output;
		output << "Total:" << item_total.getStatus();
		output << " Read:" << item_read.getStatus();
		output << " Write:" << item_write.getStatus();
		return	 output.str();
	}
	
	static size_t timevaldiff(struct timeval *starttime, struct timeval *finishtime)
	{
		size_t msec=0;
		msec+=(finishtime->tv_sec-starttime->tv_sec)*1000;
		msec+=(finishtime->tv_usec-starttime->tv_usec)/1000;

		if( msec == 0)	/// Avoid div/0
			msec = 1;
		
		return msec;
	}	

	
	

};


#endif

