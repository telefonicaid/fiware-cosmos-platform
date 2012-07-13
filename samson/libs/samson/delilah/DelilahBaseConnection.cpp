
#include "au/mutex/TokenTaker.h"

#include "samson/network/DelilahNetwork.h"

#include "DelilahBaseConnection.h" // Own interface

size_t delilah_random_code = 0;

namespace samson 
{

    // Manual network interface ( mainly samsonLocal )
    void DelilahBaseConnection::setNetwork( NetworkInterface* _network )
    {
        au::TokenTaker tt(&token);
        
        // Do not replace a previous network
        if( network )
            return;
        
        network = _network;		     // Keep a pointer to our network interface element
        if( network )
            network->setReceiver(this);
    }
    
    void DelilahBaseConnection::delilah_disconnect( au::ErrorManager * error )
    {
        au::TokenTaker tt(&token);
        au::ErrorContext c( error , "DelilahConnection" ); // Context of the error

        if( !network )
        {
            error->add_error("Not connected to any SAMSON Cluster");
            return;
        }

        // We move the reset here, because if not, the Engine does not live enough to do that
        ((DelilahNetwork*)network)->reset();

        // Schedule destruction of this element in the main engine
        engine::Engine::shared()->add( new DelilahNetworkRemove( (DelilahNetwork*) network ) );
        
        network = NULL;
    }
    
    void DelilahBaseConnection::stop()
    {
        au::TokenTaker tt(&token);
        if( network )
            network->stop();
    }

    
    void DelilahBaseConnection::delilah_connect( std::string connection_type 
                                  , std::string host 
                                  , int port
                                  , std::string user
                                  , std::string password
                                  , au::ErrorManager* error)
    {
        au::TokenTaker tt(&token);
        
        LM_T( LmtNetworkConnection , ("delilah_connect called of type %s for %s:%d, delilah_random_code:%lu", connection_type.c_str(), host.c_str(), port, delilah_random_code));

        if( network )
        {
            LM_E(("Currently connected to %s" , network->getLoginInfo().c_str()));
            error->set( au::str("Currently connected to %s" , network->getLoginInfo().c_str() ) );
            return;
        }
        
        samson::DelilahNetwork * _network = new samson::DelilahNetwork( connection_type , delilah_random_code );
        
        // Set the network element
        network = _network;
        network->setReceiver(this);
        
        LM_T( LmtNetworkConnection , ("calling addMainDelilahConnection for %s:%d, delilah_random_code:%lu", host.c_str(), port, delilah_random_code));
        // Add main delilah connection with specified worker
        samson::Status s = _network->addMainDelilahConnection( host , port , user , password );        
        
        if( s != samson::OK )
        {
            LM_W(("Not possible to open connection with %s:%d (%s)" , host.c_str() , port , samson::status(s)));
            error->set(au::str("Not possible to open connection with %s:%d (%s)" , host.c_str() , port , samson::status(s) ));
            // TODO: Wait for unfinished threads in _network?
            
            network = NULL;
            delete _network;
        }
        
    }
    
    size_t DelilahBaseConnection::getNextWorkerId()
    {
        au::TokenTaker tt(&token);

        if( !network )
            return 0;
        
        std::vector<size_t> workers = network->getWorkerIds();
        
        if( next_worker == -1 )
        {
            int max = workers.size();
            int r = rand();
            if ( max != 0 )
              next_worker = r%max;
        }
        
        next_worker++;
        if( next_worker == (int)workers.size() )
            next_worker = 0;
        
        return workers[ next_worker ];
    }
    
    void DelilahBaseConnection::send( Packet* packet , au::ErrorManager *error )
    {
        if( network )
            network->send( packet );
        else
            error->set( au::str("Not connected to any SAMSON System") );
    }
    
    std::vector<size_t> DelilahBaseConnection::getConnectedWorkerIds( au::ErrorManager * error )
    {
        au::TokenTaker tt(&token);
        
        if( network )
            return network->getConnectedWorkerIds();
        else
        {
            error->set( au::str("Not connected to any SAMSON System") );
            return std::vector<size_t>();
        }
    }
    
    std::vector<size_t> DelilahBaseConnection::getWorkerIds( au::ErrorManager * error )
    {
        au::TokenTaker tt(&token);
        
        if( network )
            return network->getWorkerIds();
        else
        {
            error->set( au::str("Not connected to any SAMSON System") );
            return std::vector<size_t>();
        }
    }
    
    bool DelilahBaseConnection::isConnected()
    {
        au::TokenTaker tt(&token);
        
        return network != NULL;
    }

    bool DelilahBaseConnection::isConnectionReady()
    {
        au::TokenTaker tt(&token);
        
        if( !network)
        {
            LM_W(("No network instance"));
            return false;
        }
        return network->ready();
    }

    
    
    std::string DelilahBaseConnection::getConnectionInformation()
    {
        au::TokenTaker tt(&token);
        
        if( network )
            return network->getLoginInfo();
        else
            return "Unconnected";
    }
    
    std::string DelilahBaseConnection::runClusterCommand( std::string command , au::ErrorManager* error )
    {
        au::TokenTaker tt(&token);
        
        if( network )
            return network->cluster_command( command );                                                    
        else
        {
            error->set( au::str("Not connected to any SAMSON System") );
            return "";
        }
        
    }
    
}
