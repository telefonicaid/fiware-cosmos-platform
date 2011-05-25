

#include "Queue.h"      // Own interface
#include "Block.h"      // samson::stream::Block
#include <sstream>      // 

namespace samson {
    namespace stream
    {
        

        // Print the status of a particular queue
        
        std::string Queue::getStatus()
        {
            
            std::ostringstream output;
            
            // Get information about size on memory / on disk
            
            size_t size=0;
            size_t size_on_memory=0;
            size_t size_on_disk=0;
            
            std::list<Block*>::iterator b;
            
            for ( b = blocks.begin() ; b != blocks.end() ; b++ )
            {
                size += (*b)->getSize();
                size_on_memory += (*b)->getSizeOnMemory();
                size_on_disk += (*b)->getSizeOnDisk();
            }
            
            output << "[ Queue " << name << " S: " << au::Format::string( size ) << " M: " << au::Format::percentage_string(size_on_memory, size) << " D: " << au::Format::percentage_string(size_on_disk, size) << " ]";
            
            return output.str();
        }
        
        
    }
}