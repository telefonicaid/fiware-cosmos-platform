

/**
 
Test program for the samsonMemory library
 
*/

#include "SamsonSetup.h"             // ss::SamsonSetup
#include "MemoryManager.h"           // ss::MemoryManager
#include "Buffer.h" 	             // ss::Buffer
#include "logMsg.h"					 // LM_M()
#include "Engine.h"                  // ss::Engine

int main( int args , char *argv[] )
{
	
	// Init SamsonSetup 
	ss::SamsonSetup::load();
	
	// Init the memory manager
	ss::Engine::init();
	
	// Get the global pointer
	ss::MemoryManager* mm = &ss::Engine::shared()->memoryManager;

	// Default value for memory
	if( mm->getUsedMemoryInput() != 0 )
		LM_X(1, ("Error in memory manager test since init memory is not 0"));


	// Get a buffer of a particular size
	ss::Buffer *b = mm->newBuffer( "buffer_1" ,  10000 , ss::Buffer::input );

	// Check memory is reserved correclty
	if( mm->getUsedMemoryInput() != 10000 )
		LM_X(1, ("Error in memory manager test since memory used is not set correctly"));

	
	// Get a buffer of a particular size
	ss::Buffer *b2 = mm->newBuffer( "buffer_2" ,  20000, ss::Buffer::input );

	// Check memory is reserved correclty
	if( mm->getUsedMemoryInput() != 30000 )
		LM_X(1, ("Error in memory manager test since memory used is not set correctly"));


	// Destroy buffers
	mm->destroyBuffer(b);
	mm->destroyBuffer(b2);
	
	// Check memory is reserved correclty
	if( mm->getUsedMemoryInput() != 0 )
		LM_X(1, ("Error in memory manager test since memory is not realeased correctly"));
	
	return 0;
}
