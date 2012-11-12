/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */


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