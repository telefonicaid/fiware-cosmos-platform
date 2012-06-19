
#ifndef _H_SAMSON_PACKET_QUEUE
#define _H_SAMSON_PACKET_QUEUE

#include <string>

#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "au/Rate.h"

#include "samson/common/status.h"
#include "samson/common/Visualitzation.h"

#include "samson/network/Packet.h"

namespace samson {
    
    
    class NetworkManager;
    
    // List of packets ( retained )
    
    class PacketQueue
    {
        au::Token            token;
        au::list<Packet>     queue;
        
        au::Cronometer cronometer;
        
    public:
        
        PacketQueue() : token("PacketQueue") 
        {
        }
        
        ~PacketQueue()
        {
            // Remove pending packets
            clear();
        }

        size_t getSeconds()
        {
            return cronometer.diffTimeInSeconds();
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
        
        void push( Packet* p )
        {
            au::TokenTaker tt(&token);
            p->retain();
            queue.push_back(p);
        }
        
        Packet* next()
        {
            au::TokenTaker tt(&token);
            Packet* packet = queue.findFront();
            return packet;
        }
        
        void pop()
        {
            au::TokenTaker tt(&token);
            Packet* packet = queue.extractFront();
            
            if( !packet )
            {
                LM_W(("pop without packet called at PacketQueue"));
                return;
            }

            packet->release();
            
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
            
            // Release all packets
            au::list<Packet>::iterator it_queue;
            for ( it_queue = queue.begin() ; it_queue != queue.end() ; it_queue++ )
                (*it_queue)->release();
            
            // Note that this is not clearList since elements should not be deleted
            // We have a retain/release model over instances of Packet
            queue.clear();
        }
        
        void pushTo( PacketQueue* target_packet_queue )
        {
            // Etxratc all packets pushing them to another packet_queue
            while( true )
            {
                Packet* packet = next();
                
                if( packet )
                {
                    target_packet_queue->push( packet );
                    pop();
                }
                else
                    return;
            }
        }
        
    };    
    
    
    class MultiPacketQueue
    {
        // Pending packets ( only used while disconnected )
        au::map<std::string , PacketQueue> packet_queues;
        au::Token token_packet_queues;
        
    public:
        
        MultiPacketQueue() : token_packet_queues("token_packet_queues")
        {
            
        }
        
        ~MultiPacketQueue()
        {
            packet_queues.clearMap();
        }

        void clear()
        {
            au::TokenTaker tt(&token_packet_queues);
            packet_queues.clearMap();
        }
        
        // Pending packets
        void push_pending_packet( std::string name , Packet * packet )
        {
            std::string prefix = "worker_";
            if( name.substr( 0 , prefix.length() ) == prefix )
            {
                au::TokenTaker tt(&token_packet_queues);
                packet_queues.findOrCreate(name)->push(packet);                
            }
            else
            {
                LM_W(("Destroying packet %s for unconnected node (%s) since it is not a worker" 
                      , packet->str().c_str(), name.c_str() ));
            }
        }
        
        void push_pending_packet( std::string name , PacketQueue * packet_queue )
        {
            std::string prefix = "worker_";
            if( name.substr( 0 , prefix.length() ) == prefix )
            {
                au::TokenTaker tt(&token_packet_queues);
                PacketQueue * target_paquet_queue = packet_queues.findOrCreate(name);                

                // Move all packets to the target queue
                packet_queue->pushTo(target_paquet_queue);                
            }
            else
            {
                packet_queue->clear();
            }
        }
        
        void pop_pending_packet( std::string name , PacketQueue * packet_queue )
        {
            //LM_W(("Popping pending packets for connection %s" , name.c_str()));
            
            au::TokenTaker tt(&token_packet_queues);
            PacketQueue * source_paquet_queue = packet_queues.extractFromMap(name);                
            
            if( !source_paquet_queue )
                return; // No pending packets
            
            // Push packets to the provided queue
            source_paquet_queue->pushTo(packet_queue);

            // remove the original paquet queue
            delete source_paquet_queue;
        }
        
        au::tables::Table * getPendingPacketsTable()
        {
            au::tables::Table* table = new au::tables::Table( au::StringVector( "Connection" , "#Packets" , "Size" ) );
            
            au::map<std::string , PacketQueue>::iterator it;
            
            for( it = packet_queues.begin() ; it != packet_queues.end() ; it++ )
            {
                au::StringVector values;
                
                values.push_back( it->first ); // Name of the connection
                
                PacketQueue* packet_queue = it->second;
                
                values.push_back( au::str( packet_queue->getNumPackets() ) );
                values.push_back( au::str( packet_queue->getTotalSize() ) );
                
                table->addRow( values );
                
            }
            
            table->setTitle("Pending packets");
            
            return table;
            
        }        
        
        void check()
        {
            au::TokenTaker tt(&token_packet_queues);
            
            au::map<std::string , PacketQueue>::iterator it_packet_queues;
            for( it_packet_queues = packet_queues.begin() ; it_packet_queues != packet_queues.end() ;  )
            {
                if( it_packet_queues->second->getSeconds() > 60 )
                {
                    std::string name = it_packet_queues->first;
                    LM_W(("Removing  pending packets for %s since it has been disconnected mote thatn 60 secs",name.c_str()));
                    it_packet_queues->second->clear();   
                    packet_queues.erase( it_packet_queues++ );
                }
                else
                    ++it_packet_queues;
            }
        }

        
    };

}

#endif
