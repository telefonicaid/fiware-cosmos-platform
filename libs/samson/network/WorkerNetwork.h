


#ifndef _H_SAMSON_WORKER_NETWORK
#define _H_SAMSON_WORKER_NETWORK

#include "samson/common/status.h"
#include "samson/common/ports.h"


#include "samson/network/NetworkInterface.h"
#include "samson/network/CommonNetwork.h"
#include "samson/network/NetworkListener.h"

namespace samson {
    
    
    class WorkerNetwork : public CommonNetwork
    {
        
        // Counter to asign new names
        size_t tmp_counter;

        // Counter to assign delilah connections
        size_t delilah_counter;
        
        // Worker listtener for new worker / delilah connections
        NetworkListener * worker_listener;

        // Web RESET interface
        NetworkListener * web_listener;
        
    public:
        
        // Constructor
        // ------------------------------------------------------------
        WorkerNetwork( int port , int web_port );        
        
        // NetworkManager interface
        // ------------------------------------------------------------
        void receive( NetworkConnection* connection, Packet* packet );
        void processHello( NetworkConnection* connection, Packet* packet );
        
        // NetworkInterface
        // ------------------------------------------------------------
        
        
        // Suspend the network elements implemented behind NetworkInterface
        // Close everything and return the "run" call
        virtual void quit(void)
        {
        }
        
        
        // NetworkManager interface
        void newSocketConnection( NetworkListener* listener , SocketConnection * socket_connetion );
    
        
    private:
        
        void update_cluster_information( ClusterInformation * new_cluster_information );
        void update_cluster_information( ClusterInformation * new_cluster_information , size_t assigned_id );
        
    };
    
}

#endif
