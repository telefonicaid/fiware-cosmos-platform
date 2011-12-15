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
#include "au/namespace.h"

#include "engine/DiskOperation.h"		// engine::DiskOperationType

#define STATISTICS_BUFFER_SIZE 100

NAMESPACE_BEGIN(engine)

class DiskOperation;

class DiskStatisticsItem
{
    
public:

    au::rate::Rate rate;
    
    DiskStatisticsItem();
    
    void add( size_t _size );
    void getInfo( std::ostringstream& output);
    
};	

class DiskStatistics
{
    
public:
    
    DiskStatisticsItem item_read;
    DiskStatisticsItem item_write;
    DiskStatisticsItem item_total;
    
    void add( DiskOperation::DiskOperationType type, size_t size );
    
    void getInfo( std::ostringstream& output);
    
};

NAMESPACE_END

#endif

