#ifndef _H_NETWORK_INTERFACE
#define _H_NETWORK_INTERFACE

/* ****************************************************************************
 *
 * FILE                     NetworkInterface.h - network interface
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            2010
 *
 */
#include "logMsg/logMsg.h"             // LM_*
#include "logMsg/traceLevels.h"        // LmtNetworkInterface, ...

#include "au/Lock.h"                   // au::Lock
#include "au/string.h"
#include "au/Token.h"     
#include "au/TokenTaker.h"
#include "au/ErrorManager.h"

#include "samson/common/SamsonSetup.h"
#include "samson/common/status.h"
#include "samson/common/Visualitzation.h"

#include "samson/network/Message.h"               // samson::Message::MessageCode
#include "samson/network/ClusterInformation.h"    // samson::Endpoint::Type


namespace samson {
    
    /* ****************************************************************************
     *
     * Forward declarations
     */
    
    class Packet;
    
    /* ****************************************************************************
     *
     * NetworkInterfaceReceiver - 
     */
    
    class NetworkInterfaceReceiver
    {
        
    public:
        
        // This method  is only called by PacketReceivedNotification
        friend class PacketReceivedNotification;
        
        // Method implemented to really receive the packet
        // Note: Packet also included notification about connections and disconnections of workers and delilahs
        virtual void receive( Packet* packet ) = 0;
        
        // Virtual destructor for correct memory deallocation
        virtual ~NetworkInterfaceReceiver() { };
                
        // Synchronous interface to reset a worker with a new worker_id... everything should be reset at this worker
        virtual void reset_worker( size_t worker_id ){};
        
        // Synchronous interface to get informtion for the REST interface
        virtual ::std::string getRESTInformation( ::std::string in )
        {
            return "Not implemented\n";
        }
        
        // Convenient way to run the receive methods using Engine system
        void schedule_receive( Packet* packet );

        
    };
	
    
    
    class NetworkInterfaceBase
    {
        
    public:
        
        NetworkInterfaceReceiver* network_interface_receiver;       // Received to get packages

        void  setReceiver(NetworkInterfaceReceiver* receiver)
        {
            network_interface_receiver = receiver;            
        }
        
    };
    
    /* ****************************************************************************
     *
     * NetworkInterface - interface of the interconnection element (Network and NetworkSimulator)
     */
    
    class  NetworkInterface : public NetworkInterfaceBase
    {
        
    public:
        
 
        NetworkInterface();
        virtual ~NetworkInterface();

        // Send packet to an element in the samson cluster ( delilah or worker )
        virtual Status send( Packet* packet )
        {
            LM_X(1, ("NetworkInterface mothod not implemented"));
            return Error;
        }
        
        // Get information about network state
        virtual void getInfo( ::std::ostringstream& output , std::string command )
        {
            LM_W(("NetworkInterface mothod not implemented"));
        }
    
        // Suspend the network elements implemented behind NetworkInterface
        // Close everything and return the "run" call
        virtual void quit(void)
        {
            LM_W(("NetworkInterface mothod not implemented"));
        }
      
        // Basic information about the cluster ( list of workers and delilahs )
        virtual std::vector<size_t> getWorkerIds()
        {
            LM_X(1, ("NetworkInterface mothod not implemented"));
            return std::vector<size_t>();
        }

        virtual std::vector<size_t> getDelilahIds()
        {
            LM_X(1, ("NetworkInterface mothod not implemented"));
            return std::vector<size_t>();
        }
        
        virtual std::string cluster_command( std::string command )
        {
            LM_W(("NetworkInterface mothod not implemented"));
            return "";
        }
        
        virtual network::Collection* getConnectionsCollection( Visualization* visualization )
        {
            return NULL;
        }
        
        virtual size_t get_rate_in()
        {
            return 0;
        }
        virtual size_t get_rate_out()
        {
            return 0;
        }
        
        virtual NodeIdentifier getMynodeIdentifier()=0;

        
    };
    
    // Old stuff to be reviewd
    void getInfoEngineSystem( ::std::ostringstream &output , NetworkInterface* network );
    
    
}

#endif
