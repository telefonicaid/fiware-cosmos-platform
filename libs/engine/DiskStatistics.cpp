

#include "engine/DiskStatistics.h"		// Own interface
#include <sstream>                      // std:ostringstream
#include "au/string.h"                  // au::Format
#include "engine/DiskOperation.h"		// engine::DiskOperation

NAMESPACE_BEGIN(engine)

#pragma mark DiskStatisticsItem

DiskStatisticsItem::DiskStatisticsItem()
{
    rate.setTimeLength( 60 ); // Last minute statistics
}	

void DiskStatisticsItem::add( size_t _size )
{
    // Compute the rate of the transactions
    rate.push( _size );
    
}

void DiskStatisticsItem::getInfo( std::ostringstream& output)
{
    output << "<description>" << rate.str() << "</description>\n";
    au::xml_simple( output , "rate" , (size_t) rate.getRate() );
}

#pragma mark DiskStatistics

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

void DiskStatistics::getInfo( std::ostringstream& output)
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



NAMESPACE_END
