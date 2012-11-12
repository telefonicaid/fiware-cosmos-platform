/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */


#include "au/xml.h"
#include "au/string.h"

#include "samson/common/coding.h"
#include "BlockInfo.h"      // Own interface


namespace samson
{
    
    BlockInfo::BlockInfo()
    {
        num_blocks = 0;
        size = 0;
        size_on_memory = 0;
        size_on_disk = 0;
        size_locked = 0;
        
        format = KVFormat("*","*");
        
        // Initial values for time
        min_time = 0;
        max_time = 0;
        
        accumulate_divisions = 0;
        
    }
    
    void BlockInfo::getInfo( std::ostringstream &output )
    {
        au::xml_open( output , "block_info" );
        au::xml_simple( output , "num_blocks" , num_blocks );
        
        au::xml_simple( output , "size" , size );
        au::xml_simple( output , "size_on_memory" , size_on_memory );
        au::xml_simple( output , "size_on_disk" , size_on_disk );
        au::xml_simple( output , "size_locked" , size_locked );
        
        
        au::xml_simple( output , "min_time_diff" , (size_t)min_time_diff() );
        au::xml_simple( output , "max_time_diff" , (size_t)max_time_diff() );
        
        au::xml_simple( output , "num_divisions" , getAverageNumDivisions() );
        
        info.getInfo( output );
        
        format.getInfo( output );
        
        au::xml_close( output , "block_info" );
    }
    
    double BlockInfo::onMemoryPercentadge()
    {
        if( size == 0)
            return 0;
        
        return (double) size_on_memory / (double) size;
    }
    
    double BlockInfo::onDiskPercentadge()
    {
        if( size == 0)
            return 0;
        
        return (double) size_on_disk / (double) size;
    }
    
    double BlockInfo::lockedPercentadge()
    {
        if( size == 0)
            return 0;
        
        return (double) size_locked / (double) size;
    }
    
    std::string BlockInfo::str()
    {
        
        return au::str( "%s [ %s | %s on memory / %s on disk / %s locked ] %s " 
                       , au::str( num_blocks , "Blocs").c_str() 
                       , au::str( size , "bytes").c_str()
                       , au::str_percentage( size_on_memory , size).c_str()
                       , au::str_percentage( size_on_disk , size).c_str()
                       , au::str_percentage( size_locked , size).c_str()
                       , info.str().c_str()
                       );
    }
    
    
    double BlockInfo::getAverageNumDivisions()
    {
        if( num_blocks == 0)
            return 1;
        
        return (double) accumulate_divisions / (double) num_blocks;
    }
    
    void BlockInfo::push( KVFormat _format )
    {
        if ( format == KVFormat("*","*") )
            format = _format;
        else if ( format != KVFormat("?" , "?" ) )
            if ( format != _format )
                format = KVFormat("?" , "?" );
    }
    
    void BlockInfo::pushTime( time_t time )
    {
        if( min_time == 0)
        {
            min_time = time;
            max_time = time;
            return;
        }
        
        if( time < min_time )
            min_time = time;
        
        if( time > max_time )
            max_time = time;
        
    }
    
    time_t BlockInfo::min_time_diff()
    {
        if ( min_time == 0)
            return 0;
        
        return time(NULL) - min_time;
    }
    
    time_t BlockInfo::max_time_diff()
    {
        if ( max_time == 0)
            return 0;
        
        return time(NULL) - max_time;
    }
    
    double BlockInfo::getOverhead()
    {
        if( size == 0 )
            return 0;
        
        return (double) ( size - info.size  ) / (double) size;
    }
    
    
    bool BlockInfo::isContentOnMemory()
    {
        return ( size == size_on_memory );
    }
    
    bool BlockInfo::isContentOnDisk()
    {
        return ( size == size_on_disk );
    }
    

    
    std::string BlockInfo::strShort()
    {
        if (num_blocks == 0 )
            return "empty";
        
        return au::str( "%3d %s %s %c%c" 
                       , num_blocks 
                       , au::str( info.kvs , "kvs" ).c_str()
                       , au::str( info.size ,"B" ).c_str()
                       , ( size_locked == size )?'L':( ( size_on_memory == size )?'M':' ')
                       , ( size_on_disk == size )?'D':' '
                       );
    }        

    
}