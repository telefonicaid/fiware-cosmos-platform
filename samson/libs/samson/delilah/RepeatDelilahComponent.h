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

#ifndef _H_DELILAH_REPEAT_COMPONENT
#define _H_DELILAH_REPEAT_COMPONENT

#include "au/ErrorManager.h"        // au::ErrorManager
#include "au/Cronometer.h"          // au::CronometerSystem

#include <cstring>
#include "samson/network/Message.h"		      // Message::MessageCode 
#include "samson/network/Packet.h"			  // samson::Packet
#include "samson/delilah/DelilahComponent.h"

namespace samson 
{
   /*
    class RepeatDelilahComponent : public DelilahComponent , engine::Object
    {
        std::string command;
        int seconds;
        
        au::Cronometer cronometer;
        
    public:
        
        RepeatDelilahComponent( std::string _command , int _seconds );
		std::string getStatus();
        void notify( engine::Notification* notification );
        
		void receive( Packet* packet )
        {
            LM_W(("Received a packet in a RepeatDelilahComponent... nothing was expected"));
            
            if (packet == NULL)
                return;
        }
        
        void run();
        
    };
    */
	
}

#endif
