

/**
 
 Test program for the samsonMemory library
 
 */

#include "SamsonSetup.h"        // ss::SamsonSetup
#include "Buffer.h" 	        // ss::Buffer
#include "parseArgs.h"          // parseArgs
#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace levels
#include "parseArgs.h"          // parseArgs
#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace levels
#include "Engine.h"				// ss::Engine
#include "Token.h"				// au::Token

#define DISK_TEST_NUM_FILES		50
#define DISK_TEST_SIZE_FILE		100000

namespace ss {
	
	/*
	class DiskTest : public FileManagerDelegate
	{
		// Token to protect list of stuff
		au::Token token;
		
		
		// Items send to write
		std::set<DiskOperation*> fwi;
		std::set<DiskOperation*> fri;
		std::set<DiskOperation*> fxi;
		
		// Pointer to the File Manager singleton
		ss::FileManager *fm;
		
	public:
		
		DiskTest()
		{
			fm = ss::FileManager::shared();
		}
		
		Buffer *getNewBuffer()
		{
			Buffer *buffer = ss::Engine::shared()->memoryManager.newBuffer("test",DISK_TEST_SIZE_FILE , Buffer::input );
			buffer->setSize( DISK_TEST_SIZE_FILE );
			char *content = buffer->getData();
			for (size_t i = 0 ; i < DISK_TEST_SIZE_FILE ; i++)
				content[i] = (unsigned char) i%100;	// Chars from 0 to 99
			
			return buffer;
		}
		
		bool verifyContent( char * content )
		{
			for (size_t i = 0 ; i < DISK_TEST_SIZE_FILE ; i++)
				if( content[i] != (unsigned char) i%100 )
					return false;
			return true;			
		}
		
		
		void addWrite( std::string fileName )
		{
			// Add an item to write
			
			FileManagerWriteItem *fmi = new FileManagerWriteItem( fileName , getNewBuffer() , this );
			fwi.insert( fmi );
			fm->addItemToWrite( fmi );
		}
		
		void addRead( std::string fileName )
		{
			Buffer *buffer = ss::Engine::shared()->memoryManager.newBuffer("test read" , DISK_TEST_SIZE_FILE , Buffer::input );
			
			FileManagerReadItem *fmi = new FileManagerReadItem(fileName , 0 , DISK_TEST_SIZE_FILE , buffer , this);
			fri.insert( fmi );
			fm->addItemToRead( fmi );
		}
		
		void addRemove( std::string fileName )
		{		
			FileManagerRemoveItem *fmi = new FileManagerRemoveItem(fileName , this);
			fxi.insert( fmi );
			fm->addItemToRemove( fmi );
		}
		
		// File Manager Delegate
		
		void notifyFinishReadItem( FileManagerReadItem *item  )
		{
			LM_M(("Received a read item for file %s", item->fileName.c_str()));
			
			// No errors in the operation
			if( item->error.isActivated() )
				LM_X(1,("Error at notifyFinishReadItem: %s", item->error.getMessage().c_str() ));
			
			// Correct content
			if( !verifyContent( item->buffer->getData() ) )
				LM_X(1,("Error verifying content"));
			
			token.retain();
			if( fri.find( item ) == fri.end() )
				LM_X(1,("Error since item was not in the list"));
			fri.erase( item );
			token.release();
			
			delete item;
		}
		
		void notifyFinishWriteItem( FileManagerWriteItem *item  )
		{
			LM_M(("Received a write item for file %s", item->fileName.c_str()));

			// No errors in the operation
			if( item->error.isActivated() )
				LM_X(1,("Error at notifyFinishWriteItem: %s", item->error.getMessage().c_str() ));
			
			token.retain();
			if( fwi.find( item ) == fwi.end() )
				LM_X(1,("Error since item was not in the list"));
			fwi.erase( item );
			token.release();
			
			delete item;
		}
		
		void notifyFinishRemoveItem( FileManagerRemoveItem *item  )
		{
			// No errors in the operation
			if( item->error.isActivated() )
				LM_X(1,("Error at notifyFinishRemoveItem: %s", item->error.getMessage().c_str() ));

			
			LM_M(("Received a remove item for file %s", item->fileName.c_str()));
			token.retain();
			if( fxi.find( item ) == fxi.end() )
				LM_X(1,("Error since item was not in the list"));
			fxi.erase( item );
			token.release();
			
			delete item;
		}
		
		// Loop until no operation is pending
		
		void loop()
		{
			while (true) 
			{
				LM_M(("Looping waiting some operations. Pending %u writes %u reads %u removes" , fwi.size() , fri.size() , fxi.size() ));
				if ( !fxi.size() && !fri.size() && !fwi.size() )
					exit(0);
				
				sleep(1);
				
 			}
		}
		
	};
	
	 */
}


/* ****************************************************************************
 *
 * parse arguments
 */

char			 workingDir[1024]; 	


PaArgument paArgs[] =
{
	{ "-working",     workingDir,       "WORKING",     PaString,  PaOpt,  _i SAMSON_DEFAULT_WORKING_DIRECTORY,   PaNL,   PaNL,  "Working directory"     },
	PA_END_OF_ARGS
};

int main( int argC , char *argV[] )
{
	
	paConfig("prefix",                        (void*) "SSW_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE:EXEC: TEXT");
	paConfig("log to file",                   (void*) true);
	
	paParse(paArgs, argC, (char**) argV, 1, false);
	
	lmAux((char*) "father");
	
	// Init SamsonSetup 
	ss::SamsonSetup::load( workingDir );		// Load setup and create default directories
	
	// Init Engine
	ss::Engine::init();

	/*
	// Object to keep all the information of the test
	ss::DiskTest test;
	
	LM_T(1,("Prova"));
	LM_M(("Init disk test"));
	
	for (int i = 0 ; i < DISK_TEST_NUM_FILES ; i++ )
	{
		std::stringstream o;
		o << "file_" << i;

		LM_M(("Adding file %s to write / read / remove" , o.str().c_str() ));
		
		test.addWrite( o.str() );
		test.addRead( o.str() );
		test.addRemove( o.str() );
		
	}
*/	

	ss::Engine::shared()->runInBackground();

	//test.loop();
	
	ss::Engine::shared()->quit();
	
	return 0;
}
