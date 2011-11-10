#ifndef _H_BLOCK_INFO
#define _H_BLOCK_INFO

#include <string>                       // std::string
#include "samson/common/coding.h"       // KVInfo

namespace samson {
    
    /**
     Structure used to report information about blocks
     **/
    
    struct BlockInfo
    {
        int num_blocks;
        size_t size;
        size_t size_on_memory;
        size_t size_on_disk;
        size_t size_locked;
        
        FullKVInfo info;
        
        KVFormat format;    // Common format for all the information
        
        time_t min_time;
        time_t max_time;
        
        int accumulate_divisions;    // Accumulation of the divisions 1 - 64
        
        BlockInfo()
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
        
        void getInfo( std::ostringstream &output )
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
        
        double onMemoryPercentadge()
        {
            if( size == 0)
                return 0;
            
            return (double) size_on_memory / (double) size;
        }
        
        double onDiskPercentadge()
        {
            if( size == 0)
                return 0;
            
            return (double) size_on_disk / (double) size;
        }
        
        double lockedPercentadge()
        {
            if( size == 0)
                return 0;
            
            return (double) size_locked / (double) size;
        }
        
        std::string str()
        {
            
            return au::str( "%s [ %s | %s on memory / %s on disk / %s locked ] %s " 
                           , au::str( num_blocks , "Blocs").c_str() 
                           , au::str( size , "bytes").c_str()
                           , au::percentage_string( size_on_memory , size).c_str()
                           , au::percentage_string( size_on_disk , size).c_str()
                           , au::percentage_string( size_locked , size).c_str()
                           , info.str().c_str()
                           );
        }

        // Get a reduced version of the string
        std::string strShort();
    
        double getAverageNumDivisions()
        {
            if( num_blocks == 0)
                return 1;
            
            return (double) accumulate_divisions / (double) num_blocks;
        }
        
        void push( KVFormat _format )
        {
            if ( format == KVFormat("*","*") )
                format = _format;
            else if ( format != KVFormat("?" , "?" ) )
                if ( format != _format )
                    format = KVFormat("?" , "?" );
        }
        
        void pushTime( time_t time )
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
        
        time_t min_time_diff()
        {
            if ( min_time == 0)
                return 0;
            
            return time(NULL) - min_time;
        }
        
        time_t max_time_diff()
        {
            if ( max_time == 0)
                return 0;
            
            return time(NULL) - max_time;
        }
        
        double getOverhead()
        {
            if( size == 0 )
                return 0;
            
            return (double) ( size - info.size  ) / (double) size;
        }
        
        
        bool isContentOnMemory()
        {
            return ( size == size_on_memory );
        }
        
        bool isContentOnDisk()
        {
            return ( size == size_on_disk );
        }
        
        
    };
    
    
}

#endif
