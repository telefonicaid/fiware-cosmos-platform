#ifndef _H_NETWORK_NODE
#define _H_NETWORK_NODE

#include "logMsg/logMsg.h"
#include "samson/network/NetworkInterface.h"        // samson::NetworkInterface
#include "samson/network/Packet.h"                  // samson::Packet
namespace samson {
    
    class NetworkNode
    {
        const char* node_name;              // Name of this node for debugging
        
    public:
        
        NetworkInterface* network;          // Network interface to send packets
        
    public:
        
        NetworkNode( const char* _node_name, NetworkInterface* _network)
        {
            node_name = _node_name;
            network = _network;
        }
        
        // Generic method to send packets
        void send(int endpoint,  Packet *p )
        {
            LM_T(LmtNodeMessages, ("%s sends %s" , node_name , p->str().c_str()));
            network->send(NULL, endpoint, p);
        }
        
    };
    
}

#endif