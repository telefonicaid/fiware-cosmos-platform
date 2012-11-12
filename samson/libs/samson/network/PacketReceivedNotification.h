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
#ifndef _H_PACKET_RECEIVER_NOTIFICATION
#define _H_PACKET_RECEIVER_NOTIFICATION

#include "au/Object.h"

#include "engine/EngineElement.h"// samson::EngineElement
#include "engine/MemoryManager.h"
#include "samson/network/NetworkInterface.h"
#include "samson/network/Packet.h"			// samson::Packet

namespace samson
{

/**
 
 PacketReceived
 Process a packet received from the network library
 
 */
    
    class PacketReceivedNotification : public engine::EngineElement
    {
        
        NetworkInterfaceReceiver* receiver;
        au::ObjectContainer<Packet> packet_container; // Container keeping packet retained
        
    public:
        
        PacketReceivedNotification( NetworkInterfaceReceiver* _receiver , Packet *_packet )
        : engine::EngineElement( "packet_received" )
        {
            if( !_packet )
                LM_X(1,("Internal error"));
            
            receiver = _receiver;
            packet_container.setObject( _packet );
            
            std::ostringstream txt;
            txt << "PacketReceivedNotification: ";
            txt << " Packet: " << packet_container.getObject()->str() << "";
            description = txt.str();
            shortDescription = "Packet";
            
        }
        
        void run()
        {
            if( ! receiver )
                LM_W(("Packet %s lost since network interface is still not activated." , packet_container.getObject()->str().c_str() ));
            else
                receiver->receive( packet_container.getObject() );
        }
        
    };
    
}

#endif
