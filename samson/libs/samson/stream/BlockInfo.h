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
#ifndef _H_BLOCK_INFO
#define _H_BLOCK_INFO

#include <string>                       // std::string

#include "samson/common/FullKVInfo.h"
#include "samson/common/KVInfo.h"
#include "samson/module/KVFormat.h"

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
        
        BlockInfo();
        
        // Push information
        // ----------------------------------------------------------------
        void push( KVFormat _format );
        void pushTime( time_t time );
        
        // Get information
        // ----------------------------------------------------------------
       
        time_t min_time_diff();
        time_t max_time_diff();
        double getOverhead();
        bool isContentOnMemory();
        bool isContentOnDisk();
        double onMemoryPercentadge();
        double onDiskPercentadge();
        double lockedPercentadge();
        double getAverageNumDivisions();

        // Debug strings
        // ----------------------------------------------------------------
        std::string str();
        std::string strShort();
        void getInfo( std::ostringstream &output );

        
        
    };
    
    
}

#endif
