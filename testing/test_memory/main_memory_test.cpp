

/**
 
Test program for the samsonMemory library
 
*/

#include "SamsonSetup.h"             // ss::SamsonSetup
#include "MemoryManager.h"           // ss::MemoryManager
#include "Buffer.h" 	             // ss::Buffer
#include <assert.h>                  // assert

int main( int args , char *argv[] )
{
	
	// Init SamsonSetup 
	ss::SamsonSetup::load();
	
	// Init the memory manager
	ss::MemoryManager::init();
	
	// Get the global pointer
	ss::MemoryManager* mm = ss::MemoryManager::shared();
	assert( mm );

	// Default value for memory
	assert( mm->getUsedMemoryInput() == 0 );

	// Get a buffer of a particular size
	ss::Buffer *b = mm->newBuffer( "buffer_1" ,  10000 , ss::Buffer::input );

	// Check memory is reserved correclty
	assert( mm->getUsedMemoryInput() == 10000 );
	
	// Get a buffer of a particular size
	ss::Buffer *b2 = mm->newBuffer( "buffer_2" ,  20000, ss::Buffer::input );

	// Check memory is reserved correclty
	assert( mm->getUsedMemoryInput() == 30000 );

	// Destroy buffers
	mm->destroyBuffer(b);
	mm->destroyBuffer(b2);
	
	// Check memory is reserved correclty
	assert( mm->getUsedMemoryInput() == 0 );
	
	return 0;
}