
#include "samson/network/Network2.h"
#include "samson/network/EndpointManager.h"
#include "samson/network/Packet.h"

#include "samson/delilah/Delilah.h"     // samson::Delilah


#include "DelilahConnection.h"      // Ownt interface


DelilahConnection::DelilahConnection()
{
    networkP = NULL;
    delilah = NULL;
    sent_process_vector_packet = false;
}

void DelilahConnection::connect( std::string controller )
{
    if( networkP )
        return; // Still connecting with a previous controller....
    
    // Initialize the network element for delilah
    networkP  = new samson::Network2( samson::Endpoint2::Delilah, controller.c_str() );
    networkP->runInBackground();

}

bool DelilahConnection::isReady()
{
    if( !networkP )
        return false;
    
    if( delilah )
        return true;        // Delilah element previously created 
    
    if( !networkP->ready() )
        return false;
    
    
    if( !sent_process_vector_packet )
    {
        // Send a packet to get the list of workers ( only once )
        samson::Packet*  packetP  = new samson::Packet(samson::Message::Msg, samson::Message::ProcessVector);
        networkP->epMgr->controller->send( packetP );
        sent_process_vector_packet = true;
    }
    
    if( !networkP->ready(true) )
        return false;
    
    // Creating delilah object
    delilah = new samson::Delilah( networkP );
    
    // Return everything is ready
    return true;        
}

