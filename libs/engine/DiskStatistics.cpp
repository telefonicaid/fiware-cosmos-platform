

#include "engine/DiskStatistics.h"		// Own interface
#include <sstream>                      // std:ostringstream
#include "au/Format.h"                  // au::Format
#include "engine/DiskOperation.h"		// engine::DiskOperation

namespace engine
{
	
#pragma mark DiskStatisticsItem
    
	DiskStatisticsItem::DiskStatisticsItem()
	{
	}	
	
	void DiskStatisticsItem::add( size_t _size )
	{
        // Compute the rate of the transactions
        rate.push( _size );
        
	}
	
	std::string DiskStatisticsItem::getStatus()
	{
        
		std::ostringstream output;
        output << rate.str();
		return output.str();
	}
	

#pragma mark DiskStatistics
/*	
	void DiskStatistics::add( DiskOperation *operation )
	{
		add( operation->getType() , operation->getSize() );
	}
*/	
	
	void DiskStatistics::add( DiskOperation::DiskOperationType type, size_t size )
	{
		switch (type) {
			case DiskOperation::read:
			{
				item_read.add( size );
			}
				break;
			case DiskOperation::append:
			case DiskOperation::write:
			{
				item_write.add( size );
			}
				break;
			case DiskOperation::remove:
			{
			}
				break;
                
		}
        // Add the toal
        item_total.add( size );
		
	}
	
	std::string DiskStatistics::getStatus()
	{
		std::ostringstream output;
		output << "\n\tTotal: " << item_total.getStatus();
        output << "\n\tRead:  " << item_read.getStatus();
        output << "\n\tWrite: " << item_write.getStatus();
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