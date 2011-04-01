#include "SamsonSetup.h"             // ss::SamsonSetup
#include "MemoryManager.h"           // ss::MemoryManager
#include "Engine.h"                  // ss::Engine
#include "Buffer.h" 	             // ss::Buffer
#include "logMsg.h"		     // LM_M()
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>



class MemoryTest : public CPPUNIT_NS::TestFixture
{

  CPPUNIT_TEST_SUITE( MemoryTest );
  CPPUNIT_TEST( test );
  CPPUNIT_TEST_SUITE_END();

 public:

  void setUp()
  {
	// Init SamsonSetup 
	ss::SamsonSetup::load();
	
	// Init the memory manager
	ss::Engine::init();

	ss::Engine::shared()->runInBackground();
  }

  void tearDown()
  {
	ss::Engine::shared()->quit();
  }

 protected:

  void test()
  {
	
	// Get the global pointer
	ss::MemoryManager* mm = &ss::Engine::shared()->memoryManager;

	// Default value for memory
	CPPUNIT_ASSERT( mm->getUsedMemoryInput() == 0 );

	// Get a buffer of a particular size
	ss::Buffer *b = mm->newBuffer( "buffer_1" ,  10000 , ss::Buffer::input );

	// Check memory is reserved correclty
	CPPUNIT_ASSERT( mm->getUsedMemoryInput() == 10000 );
	
	// Get a buffer of a particular size
	ss::Buffer *b2 = mm->newBuffer( "buffer_2" ,  20000, ss::Buffer::input );

	// Check memory is reserved correclty
	CPPUNIT_ASSERT( mm->getUsedMemoryInput() == 30000 );

	// Destroy buffers
	mm->destroyBuffer(b);
	mm->destroyBuffer(b2);
	
	// Check memory is reserved correclty
	CPPUNIT_ASSERT( mm->getUsedMemoryInput() == 0 );

  }

};



