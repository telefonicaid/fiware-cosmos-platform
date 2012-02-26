
#ifndef _H_SAMSON_PACKET_QUEUE
#define _H_SAMSON_PACKET_QUEUE

#include <string>

#include "au/Token.h"
#include "au/TokenTaker.h"
#include "au/Rate.h"

#include "samson/common/status.h"
#include "samson/common/Visualitzation.h"

#include "samson/network/SocketConnection.h"
#include "samson/network/Packet.h"

namespace samson {
    
    
    class NetworkManager;
    
    class PacketQueue
    {
        au::Token            token;
        au::list<Packet>     queue;
        
        
    public:
        
        PacketQueue() : token("PacketQueue") 
        {
        }
        
        size_t getNumPackets()
        {
            au::TokenTaker tt(&token);
            return queue.size();
        }
        
        size_t getTotalSize()
        {
            au::TokenTaker tt(&token);
            
            size_t total = 0;
            au::list<Packet>::iterator it_queue;
            for ( it_queue = queue.begin() ; it_queue != queue.end() ; it_queue++ )
            {
                Packet * packet = *it_queue;
                total += packet->getSize();
            }
            return total;
        }
        
        void push( Packet* p)
        {
            au::TokenTaker tt(&token);
            queue.push_back(p);
        }
        
        Packet* next()
        {
            au::TokenTaker tt(&token);
            Packet* packet = queue.findFront();
            return packet;
        }
        
        Packet* extract()
        {
            au::TokenTaker tt(&token);
            return queue.extractFront();
        }
        
        void pop()
        {
            au::TokenTaker tt(&token);
            Packet* packet = queue.extractFront();
            
            if( !packet )
                LM_W(("pop without packet called at PacketQueue"));
        }
        
        std::string str()
        {
            au::TokenTaker tt(&token);
            return au::str("PaquetQueue: %lu packets" , queue.size() );
        }
        
        
        void clear()
        {
            // Clear queues, removing packages and alloc buffers
            au::TokenTaker tt(&token);
            au::list<Packet>::iterator it_queue;
            for ( it_queue = queue.begin() ; it_queue != queue.end() ; it_queue++ )
            {
                engine::Buffer * buffer = ( *it_queue )->buffer;
                if ( buffer )
                    engine::MemoryManager::shared()->destroyBuffer( buffer );
            }
            
            // Remove elements calling delete to all of them
            queue.clearList();
        }
        
        
    };    
}

#endif