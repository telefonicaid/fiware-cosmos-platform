

/**
 
 Main dialog to stablish a connection with SAMSON Platform
 
 */

#ifndef _H_DELILAHCONNECTION_QT
#define _H_DELILAHCONNECTION_QT


#include <string>

namespace samson
{
    class Delilah;
    class Network2;
}

class DelilahConnection
{

    samson::Network2* networkP;
    bool sent_process_vector_packet;

public:
    
    samson::Delilah *delilah;
    
    
    DelilahConnection();
    void connect( std::string controller );
    bool isReady();
    
    
};


#endif