
#include <sstream>       

#include "Queue.h"          // Own interface
#include "QueuesManager.h"  // samson::stream::QueuesManager
#include "Block.h"          // samson::stream::Block
#include "BlockManager.h"   // samson::stream::BlockManager

#include "engine/ProcessManager.h"      // engine::ProcessManager

#include "samson/common/EnvironmentOperations.h"    // getStatus()
#include "samson/module/ModulesManager.h"           
#include "QueueTask.h"
#include "samson/network/NetworkInterface.h"
#include "samson/worker/SamsonWorker.h"
#include "samson/stream/ParserQueueTask.h"          // samson::stream::ParserQueueTask


#include "BlockMatrix.h"        // Own interface

namespace samson {
    namespace stream
    {
        
#pragma mark BlockList
        
        void BlockList::add( Block *block )
        {
            // Insert in the back of the list
            blocks.push_back( block );
            
            if( block->header )
                accumulated_info.append( block->header->info );
            else
                accumulated_info.append( block->getSize() , 1 );
        }
        
        Block* BlockList::extract( )
        {
            return blocks.extractFront();
        }
                
        size_t BlockList::getSize()
        {
            size_t total = 0;
            for (  au::list< Block >::iterator i = blocks.begin() ; i!= blocks.end() ; i++)
                total += (*i)->getSize();
            return total;
        }
        
        std::string BlockList::str()
        {
            size_t size=0;
            size_t size_on_memory=0;
            size_t size_on_disk=0;
            
            std::ostringstream output;
            std::list<Block*>::iterator b;
            
            for ( b = blocks.begin() ; b != blocks.end() ; b++ )
            {
                size += (*b)->getSize();
                size_on_memory += (*b)->getSizeOnMemory();
                size_on_disk += (*b)->getSizeOnDisk();
                
                output << (*b)->str();
            }
            
            if( blocks.size() == 0 )
                output << "[ No content ]";
            else
            {
                output << "[ " << blocks.size() << " blocks with " << au::Format::string( size, "Bytes" );
                output << " " << au::Format::percentage_string(size_on_memory, size) << " on memory";
                output << " & " << au::Format::percentage_string(size_on_disk, size) << " on disk ]";
            }
            
            output << "[ Accumulatd " << accumulated_info.str() << " ]";
            
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
    
        void BlockList::retain()
        {
            std::list<Block*>::iterator b;
            for ( b = blocks.begin() ; b != blocks.end() ; b++ )
                BlockManager::shared()->retain(*b);
            return;
                
        }
        void BlockList::release()
        {
            std::list<Block*>::iterator b;
            for ( b = blocks.begin() ; b != blocks.end() ; b++ )
                BlockManager::shared()->release(*b);
            return;
        }
        
        void BlockList::retain(size_t id)
        {
            std::list<Block*>::iterator b;
            for ( b = blocks.begin() ; b != blocks.end() ; b++ )
                BlockManager::shared()->retain( *b , id );
            return;
        }
        
        
        void BlockList::release(size_t id)
        {
            std::list<Block*>::iterator b;
            for ( b = blocks.begin() ; b != blocks.end() ; b++ )
                BlockManager::shared()->release( *b , id );
            return;
        }
        
        void BlockList::lock()
        {
            std::list<Block*>::iterator b;
            for ( b = blocks.begin() ; b != blocks.end() ; b++ )
                BlockManager::shared()->lock( *b );
        }
        
        void BlockList::unlock()
        {
            std::list<Block*>::iterator b;
            for ( b = blocks.begin() ; b != blocks.end() ; b++ )
                BlockManager::shared()->unlock( *b );
        }
        
        // Get information
        FullKVInfo BlockList::getInfo()
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

        
#pragma mark BlockMatrix

        // Add a block to a particular queue
        
        void BlockMatrix::add( int _channel , Block *block )
        {
            BlockList * channel = channels.findOrCreate( _channel );
            channel->add( block );
        }
                
        
        
        Block *BlockMatrix::extractFromChannel( int _channel )
        {
            BlockList * channel = channels.findOrCreate( _channel );
            return channel->extract();
        }
        
        
        bool BlockMatrix::isEmpty()
        {
            au::map<int,BlockList>::iterator i;
            for ( i =  channels.begin() ; i!= channels.end() ; i++)
                if( !i->second->isEmpty() )
                    return false;
            return true;
        }
        
        bool BlockMatrix::isEmpty( int channel_begin , int channel_end )
        {
            
            au::map<int,BlockList>::iterator i;
            for ( i =  channels.begin() ; i!= channels.end() ; i++)
            {
                if( ( i->first >= channel_begin ) && ( i->first < channel_end ) )
                    if( !i->second->isEmpty() )
                        return false;
            }
            
            return true;
        }
        

        bool BlockMatrix::isContentOnMemory()
        {
            au::map<int,BlockList>::iterator i;
            for ( i =  channels.begin() ; i!= channels.end() ; i++)
                if( !i->second->isContentOnMemory() )
                    return false;
            return true;
            
        }
        
        void BlockMatrix::retain()
        {
            au::map<int,BlockList>::iterator i;
            for ( i =  channels.begin() ; i!= channels.end() ; i++)
                i->second->retain();
        }
        
        void BlockMatrix::release()
        {
            au::map<int,BlockList>::iterator i;
            for ( i =  channels.begin() ; i!= channels.end() ; i++)
                i->second->release();
        }

        void BlockMatrix::retain(size_t id)
        {
            au::map<int,BlockList>::iterator i;
            for ( i =  channels.begin() ; i!= channels.end() ; i++)
                i->second->retain(id);
        }
        
        void BlockMatrix::release(size_t id)
        {
            au::map<int,BlockList>::iterator i;
            for ( i =  channels.begin() ; i!= channels.end() ; i++)
                i->second->release(id);
        }
        
        
        void BlockMatrix::lock()
        {
            au::map<int,BlockList>::iterator i;
            for ( i =  channels.begin() ; i!= channels.end() ; i++)
                i->second->lock();
        }
        
        void BlockMatrix::unlock()
        {
            au::map<int,BlockList>::iterator i;
            for ( i =  channels.begin() ; i!= channels.end() ; i++)
                i->second->unlock();
        }
        
        std::string BlockMatrix::str()
        {
            std::ostringstream output;
            
            if( channels.size() == 0)
                output << "[ No data ]";
            else
            {
                
                au::map<int , BlockList>::iterator i;
                for (i = channels.begin() ; i != channels.end() ; i++ )
                    output << "Channel " << i->first << " : " << i->second->str() << "\n"; 
            }
            
            return output.str();
        }
        
        void BlockMatrix::extract( BlockMatrix* _matrix , int channel_begin , int channel_end , size_t max_size )
        {
            if( channel_end  < channel_begin )
                LM_X(1, ("Internal error"));
            
            size_t total_size = 0;
            int current_channel = channel_begin;
            int num_channels_without_block=0;
            
            while( ( total_size < max_size ) || ( max_size == 0 ) ) // No limit if max_size == 0
            {
                Block *block = extractFromChannel(current_channel);

                if( block )
                {
                    total_size += block->getSize();
                    _matrix->add(current_channel, block);
                }
                
                current_channel++;
                
                if( current_channel >= channel_end )
                    current_channel = channel_begin;
                
                if( block )
                    num_channels_without_block++;
                else
                    num_channels_without_block=0;
                
                if( num_channels_without_block == (channel_end - channel_begin) )
                    return; // No more packets
                
            }
            
            
        }
        
        void BlockMatrix::copyFrom( BlockMatrix* _matrix , int hg_begin , int hg_end )
        {
            au::map<int , BlockList>::iterator c;
            for ( c = _matrix->channels.begin() ; c!= _matrix->channels.end() ; c++ )
            {
                int _channel = c->first;
                BlockList *list = c->second;
                au::list< Block >::iterator b;
                for ( b = list->blocks.begin() ; b != list->blocks.end() ; b++ )
                {
                    Block *block = *b;
                    if( block->isNecessaryForHashGroups( hg_begin , hg_end ) )
                        add( _channel , block );
                }
            }
        }
        
        
        FullKVInfo BlockMatrix::getInfo()
        {
            FullKVInfo info;
            info.clear();
            
            au::map<int , BlockList>::iterator c;
            for ( c = channels.begin() ; c!= channels.end() ; c++ )
            {
                BlockList *list = c->second;
                info.append( list->getInfo() );
            }
            
            return info;
        }

        size_t BlockMatrix::getNumBlocks()
        {
            int num = 0;
            
            au::map<int , BlockList>::iterator c;
            for ( c = channels.begin() ; c!= channels.end() ; c++ )
            {
                BlockList *list = c->second;
                num +=  list->getNumBlocks();
            }
            
            return num;
        }


        

    }       
}