
#include "DelilahUploadDataProcess.h"			// Own interface
#include "engine/MemoryManager.h"					// samson::MemoryManager
#include "engine/Buffer.h"							// samson::Buffer
#include "samson/network/Packet.h"							// samson::Packet
#include "samson/network/Message.h"						// samson::Message
#include "samson/delilah/Delilah.h"						// samson::Delilah
#include "samson/common/samson.pb.h"						// network::...
#include "DelilahClient.h"					// samson::DelilahClient
#include "samson/common/SamsonSetup.h"					// samson::SamsonSetup

namespace samson
{
	
	void TXTFileSet::fill( engine::Buffer *b )
	{
		
		// First the header
		
		if( finish )
		{
			return;	// Just in case
		}
		
		// Write the previous characters here
		bool ans = b->write( previousBuffer, previousBufferSize );
		if( !ans )
			LM_X(1,("Error writing in a TXTFileSet"));
		
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
					return;
				}
			}
		}
		
		// Full buffer
		// Remove the last chars until a complete line and keep for the next read
		previousBufferSize =  b->removeLastUnfinishedLine( previousBuffer );
		
		
		
	}

}
