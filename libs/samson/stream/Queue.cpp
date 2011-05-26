
#include <sstream>       

#include "Queue.h"          // Own interface
#include "Block.h"          // samson::stream::Block
#include "BlockManager.h"   // samson::stream::BlockManager
    
#include "samson/common/EnvironmentOperations.h"    // getStatus()


namespace samson {
    namespace stream
    {
        
        // Add a block to a particular queue
        
        void Queue::add( Block *block )
        {
            
            // Retain the block to be an owner of the block
            stream::BlockManager::shared()->retain( block );
            
            // Insert in the back of the list
            blocks.push_back( block );
        }
        
        

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
            
            output << "[ Queue " << name << " : " << blocks.size() << " blocks with " << au::Format::string( size, "Bytes" );
            output << " " << au::Format::percentage_string(size_on_memory, size) << " on memory";
            output << " & " << au::Format::percentage_string(size_on_disk, size) << " on disk ]";
            
            if( streamQueue )
                output << " " << samson::getStatus( streamQueue );

            
            return output.str();
        }
        
        
    }
}