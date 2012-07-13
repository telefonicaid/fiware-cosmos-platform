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

#include "au/string.h"
#include "au/mutex/Token.h"     
#include "au/mutex/TokenTaker.h"
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
        
        
    };
	
    
    /* ****************************************************************************
     *
     * NetworkInterfaceBase - Interaction with NetworkInterfaceReceiver
     */
    
    class NetworkInterfaceBase
    {
        
    public:
        
        NetworkInterfaceReceiver* network_interface_receiver;       // Receiver for incomming packages
        
        void  setReceiver( NetworkInterfaceReceiver* receiver )
        {
            network_interface_receiver = receiver;            
        }
        
        // Convenient way to run the receive methods using Engine system
        void schedule_receive( Packet* packet );

    };
    
    /* ****************************************************************************
     *
     * NetworkInterface - interface of the interconnection element (Network and NetworkSimulator)
     */
    
    class  NetworkInterface : public NetworkInterfaceBase
    {

    public:

        // Constructor and destructor
        NetworkInterface(){};
        virtual ~NetworkInterface(){};

        // Send packet to an element in the samson cluster ( delilah or worker )
        virtual Status send( Packet* packet )
        {
            LM_E(("NetworkInterface send() method not implemented"));
            return Error;
        }
    
        // Close all threads and return the "run" call
        virtual void quit(void)
        {
            LM_W(("NetworkInterface quit() method not implemented"));
        }
      
        virtual void stop()
        {
            LM_W(("NetworkInterface stop() method not implemented"));
        }
        
        // Basic information about the cluster ( list of workers and delilahs )
        virtual std::vector<size_t> getWorkerIds()
        {
            LM_E(("NetworkInterface getWorkerIds() method not implemented"));
            return std::vector<size_t>();
        }

        virtual std::vector<size_t> getConnectedWorkerIds()
        {
            // Default implementation( for samsonLocal )
            return getWorkerIds();
        }
        
        virtual std::vector<size_t> getDelilahIds()
        {
            LM_E(("NetworkInterface getDelilahIds() method not implemented"));
            return std::vector<size_t>();
        }
        
        virtual std::string cluster_command( std::string command )
        {
            LM_W(("NetworkInterface cluster_command() method not implemented (%s)", command.c_str()));
            return "";
        }
        
        virtual std::string getLoginInfo()
        {
            return "?";
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
        
        // Get information about network state
        virtual void getInfo( ::std::ostringstream& output , std::string command , std::string format = "xml")
        {
            LM_W(("NetworkInterface getInfo() method not implemented (%s)", command.c_str()));
        }
        
        virtual NodeIdentifier getMynodeIdentifier()=0;
        
        virtual std::string getHostForWorker(size_t worker_id)
        {
			if (worker_id == 0) {}
            LM_E(("NetworkInterface getHostForWorker() method not implemented"));
            return "";
        }

        virtual unsigned short getPortForWorker(size_t worker_id)
        {
			if (worker_id == 0) {}
            LM_E(("NetworkInterface getPortForWorker() method not implemented"));
            return 0xFFFF;
        }
        
        // Get a descriptio of this connection ( host & port for example )
        virtual std::string str()
        {
            return au::str("NetworkInterrface::str() TO BE IMPLEMENTED" );
        }
        
        // Check if we are fully connected
        virtual bool ready()
        {
            return true;
        }
        
    };
    
    // Old stuff to be reviewd
    void getInfoEngineSystem( ::std::ostringstream &output , NetworkInterface* network );
    
    
}

#endif
