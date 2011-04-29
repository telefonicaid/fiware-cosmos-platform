
#include "DelilahUploadDataProcess.h"			// Own interface
#include "engine/MemoryManager.h"					// ss::MemoryManager
#include "engine/Buffer.h"							// ss::Buffer
#include "Packet.h"							// ss::Packet
#include "Message.h"						// ss::Message
#include "Delilah.h"						// ss::Delilah
#include "samson.pb.h"						// network::...
#include "DelilahClient.h"					// ss::DelilahClient
#include "SamsonSetup.h"					// ss::SamsonSetup

namespace ss
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
