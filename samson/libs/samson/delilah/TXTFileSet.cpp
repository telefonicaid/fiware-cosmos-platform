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

#include "engine/MemoryManager.h"					// samson::MemoryManager
#include "engine/Buffer.h"							// samson::Buffer
#include "samson/network/Packet.h"							// samson::Packet
#include "samson/network/Message.h"						// samson::Message
#include "samson/delilah/Delilah.h"						// samson::Delilah
#include "samson/common/samson.pb.h"						// network::...
#include "samson/common/SamsonSetup.h"					// samson::SamsonSetup

#include "TXTFileSet.h"  // Own interface

namespace samson
{
	
	int TXTFileSet::fill( engine::Buffer *b )
	{
		
		// First the header
		
		if( finish )
		{
			return 0;	// Just in case
		}
        
        if( previousBuffer )
        {
            if( previousBufferSize> 0)
            {
                bool ans = b->write( previousBuffer, previousBufferSize );
                if( !ans )
                    LM_X(1,("Error writing in a TXTFileSet"));
            }
        }
		
		while( b->getAvailableWrite() > 0 )	// While there is space to fill
		{
			
			b->write( inputStream );
            
			// Open the next file if necessary
			if( inputStream.eof() )
			{
				inputStream.close();
				openNextFile();
				if( finish )
				{
					// Information in the header
					return 0;
				}
			}
		}

        // Remove previous buffer ( if any )
        if( previousBuffer )
            free( previousBuffer );
        
		// Remove the last chars until a complete line and keep for the next read
		if( b->removeLastUnfinishedLine( previousBuffer , previousBufferSize ) != 0)
            return 1;   // Error filling the buffer

        // No error here
		return 0;
		
	}

}
