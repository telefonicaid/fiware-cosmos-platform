
#include <sstream>       

#include "Queue.h"          // samson::stream::Queue
#include "QueuesManager.h"  // samson::stream::QueuesManager
#include "Block.h"          // samson::stream::Block
#include "BlockManager.h"   // samson::stream::BlockManager

#include "engine/ProcessManager.h"                  // engine::ProcessManager

#include "samson/common/EnvironmentOperations.h"    // getStatus()
#include "samson/common/Info.h"                     // samson::Info
#include "samson/common/coding.h"                   // KVInfo

#include "samson/module/ModulesManager.h"           
#include "QueueTask.h"
#include "samson/network/NetworkInterface.h"
#include "samson/worker/SamsonWorker.h"
#include "samson/stream/ParserQueueTask.h"          // samson::stream::ParserQueueTask


#include "BlockList.h"                              // Own interface

namespace samson {
    namespace stream
    {
        
#pragma mark BlockList
        
        
        
        BlockList::~BlockList()
        {
            clearBlockList();
        }
        
        
        void BlockList::clearBlockList()
        {
            // Remove all the blocks contained in this list
            while( blocks.size() > 0 )
                remove( top() );
            
        }
        
        void BlockList::createBlock( engine::Buffer *buffer , bool txt )
        {
            // create a new block
            Block *block  = new Block( buffer , txt );

            // Insert this block in my list
            blocks.push_back( block );
            
            // Insert myself in the the list inside the block
            block->lists.insert( this );
            
            // Add automatically to the Block Manager
            BlockManager::shared()->insert( block );   
        }
        
        
        void BlockList::add( Block *block )
        {
            
            // Accumulated information
            accumulated_info.append( block->getInfo() );
            
            // Insert this block in my list
            blocks.push_back( block );
            
            // Insert myself in the the list inside the block
            block->lists.insert( this );
            
            // Check the block
            BlockManager::shared()->check( block );
        }
        
        void BlockList::remove( Block* block )
        {
            // Remove this block from my list of blocks
            blocks.remove( block );

            // remove this container from the list inside the block
            block->lists.erase( this );
         
            // Check the block
            BlockManager::shared()->check(block);
            
        }
                               
        void BlockList::remove( size_t id )
        {
            au::list< Block >::iterator b;
            for (b=blocks.begin(); b != blocks.end();b++)
            {
                if( (*b)->id == id)
                {
                    remove( *b );
                    return;
                }
            }
        }

        
        Block* BlockList::top( )
        {
            return blocks.front();
        }
        
        size_t BlockList::getSize()
        {
            size_t total = 0;
            for (  au::list< Block >::iterator i = blocks.begin() ; i!= blocks.end() ; i++)
                total += (*i)->getSize();
            return total;
        }
        
        /*
        void BlockList::copyFrom( BlockMatrix* matrix , int channel )
        {
            BlockList *bl = matrix->channels.findInMap( channel );
            if( !bl )
                return; // Nothing to copy because there is not such a channel
            
            // Copy all the blocks from another
            copyFrom( bl );
        }
         */
        
        
        void BlockList::copyFrom( BlockList* list )
        {
            au::list< Block >::iterator b;
            for (b = list->blocks.begin() ;  b != list->blocks.end() ; b++)
                add( *b );
            
        }

        void BlockList::extractFrom( BlockList* list , size_t max_size )
        {
            size_t total_size = 0 ;
            
            while( list->top() != NULL )
            {
                Block *b = list->top();
                total_size += max_size;

                if (( max_size > 0) && ( total_size > max_size ))
                    return;
                
                add( b );
                list->remove( b );
                
            }
            
        }
        
        
        std::string BlockList::str()
        {
            size_t size             = 0;
            size_t size_on_memory   = 0;
            size_t size_on_disk     = 0;
            
            FullKVInfo info;
            
            std::ostringstream output;
            std::list<Block*>::iterator b;
            
            for ( b = blocks.begin() ; b != blocks.end() ; b++ )
            {
                size            += (*b)->getSize();
                size_on_memory  += (*b)->getSizeOnMemory();
                size_on_disk    += (*b)->getSizeOnDisk();
                info.append( (*b)->getInfo() );
                
            }
            
            /*
             output << "\tBlocks: ";
             for ( b = blocks.begin() ; b != blocks.end() ; b++ )
             output << (*b)->str();
             output << "\n";
             */
            
            if( blocks.size() == 0 )
            {
                output << "\n\tTotal info          " << "[ No content ]";
                output << "\n\tAccumulated info    " << accumulated_info.str() << " ]";
            }
            else
            {
                output << "\n\tTotal info          " << info.str();
                output << "\n\tAccumulated info    " << accumulated_info.str();
                output << "\n\tBlock information   " << "( " << blocks.size() << " blocks with " << au::Format::string( size, "Bytes" );
                output << " " << au::Format::percentage_string(size_on_memory, size) << " on memory";
                output << " & " << au::Format::percentage_string(size_on_disk, size) << " on disk )";
            }
            
            
            return output.str();
        }
        
        bool BlockList::isEmpty()
        {
            return (blocks.size()==0);
        }
        
        
        
        bool BlockList::isContentOnMemory()
        {
            std::list<Block*>::iterator b;
            for ( b = blocks.begin() ; b != blocks.end() ; b++ )
                if( ! (*b)->isContentOnMemory() )
                    return false;
            
            return true;
        }
        

        
        // Get information
        FullKVInfo BlockList::getFullKVInfo()
        {
            FullKVInfo info;
            std::list<Block*>::iterator b;
            for ( b = blocks.begin() ; b != blocks.end() ; b++ )
                info.append( ( *b )->getInfo() );
            return info;
        }
        
        size_t BlockList::getNumBlocks()
        {
            return blocks.size();
        }
        
        std::string BlockList::getSummary()
        {
            FullKVInfo info =  getFullKVInfo();
            size_t num_blocks = getNumBlocks();
            
            return au::Format::string("BlockList with %s containing %s" , au::Format::string(num_blocks ,"Blocks").c_str() , info.str().c_str() );
        }
        
        
        void BlockList::getInfo( std::ostringstream& output)
        {
            
            size_t size_total       = 0;
            size_t size_on_memory   = 0;
            size_t size_on_disk     = 0;
            
            FullKVInfo info;
            
            std::list<Block*>::iterator b;            
            for ( b = blocks.begin() ; b != blocks.end() ; b++ )
            {
                size_total      += (*b)->getSize();
                size_on_memory  += (*b)->getSizeOnMemory();
                size_on_disk    += (*b)->getSizeOnDisk();
                info.append( (*b)->getInfo() );
                
            }
            
            output << "<block_list>\n";
            
            output << "<size_total>"        << size_total        << "</size_total>\n";
            output << "<size_on_memory>"    << size_on_memory    << "</size_on_memory>\n";
            output << "<size_on_disk>"      << size_on_disk      << "</size_on_disk>\n";
            
            output << "<size>" << info.size << "</size>\n";
            output << "<kvs>" << info.kvs << "</kvs>\n";
            
            output << "</block_list>\n";
        }
        
        
    }       
}