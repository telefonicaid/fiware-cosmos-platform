

#include "DiskStatistics.h"		// Own interface
#include <sstream>				// std:ostringstream
#include "au/Format.h"				// au::Format
#include "DiskOperation.h"		// ss::DiskOperation

namespace engine
{
	
	DiskStatisticsItem::DiskStatisticsItem()
	{
		pos = 0;
		length =0 ;
	}	

	
	
	void DiskStatisticsItem::add( size_t _size , size_t _mili_seconds )
	{
		size[pos] = _size;
		time[pos] = _mili_seconds;
		
		// Move to the next position
		pos = (pos+1)%STATISTICS_BUFFER_SIZE;
		if( length < STATISTICS_BUFFER_SIZE)
			length++;		
	}
	
	std::string DiskStatisticsItem::getStatus()
	{
		size_t total_size=0;
		size_t total_time=0;
		
		for (int i = 0 ; i < length ; i++)
		{
			total_time += time[i];
			total_size += size[i];
		}
		
		size_t rate;
		
		if( total_time > 0)
			rate = (size_t)(((double)total_size / (double)total_time )*1000.0);
		else
			rate = 0;
		
		std::ostringstream output;
		output << "[" << au::Format::string( total_size ,"B" );
		output << " at ";
		output << au::Format::string( rate ,"Bps" );;
		output << "]";
		
		return output.str();
	}
	

#pragma mark DiskStatistics
	
	void DiskStatistics::add( DiskOperation *operation )
	{
		add( operation->getType() , operation->getSize() , operation->getOperationTime() );
	}
	
	
	void DiskStatistics::add( DiskOperation::DiskOperationType type, size_t size , size_t micro_seconds )
	{
		switch (type) {
			case DiskOperation::read:
			{
				item_read.add( size, micro_seconds);
			}
				break;
			case DiskOperation::append:
			case DiskOperation::write:
			{
				item_write.add( size, micro_seconds);
			}
				break;
			case DiskOperation::remove:
			{
			}
				break;
		}
		
		item_total.add( size, micro_seconds);
	}
	
	std::string DiskStatistics::getStatus()
	{
		std::ostringstream output;
		output << item_total.getStatus();
		output << "( R:" << item_read.getStatus();
		output << " W:" << item_write.getStatus() << " )";
		return	 output.str();
	}
	
	size_t DiskStatistics::timevaldiff(struct timeval *starttime, struct timeval *finishtime)
	{
		size_t msec=0;
		msec+=(finishtime->tv_sec-starttime->tv_sec)*1000;
		msec+=(finishtime->tv_usec-starttime->tv_usec)/1000;
		
		if( msec == 0)	/// Avoid div/0
			msec = 1;
		
		return msec;
	}	
	

}