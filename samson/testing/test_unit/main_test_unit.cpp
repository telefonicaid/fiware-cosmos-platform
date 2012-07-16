
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "MemoryTest.h"     // MemoryTest

class SimplestCase : public CPPUNIT_NS::TestFixture
{

  CPPUNIT_TEST_SUITE( SimplestCase );
  CPPUNIT_TEST( MyTest );
  CPPUNIT_TEST_SUITE_END();

  protected:

  void MyTest()
  {
    float fnum = 2.00001f;
    CPPUNIT_ASSERT_DOUBLES_EQUAL( fnum, 2.0f, 0.0005 );
  }

};

CPPUNIT_TEST_SUITE_REGISTRATION( SimplestCase );  


int  main( int argC, char*argV[] )
{

  CppUnit::TextUi::TestRunner runner;
  runner.addTest( SimplestCase::suite() );

  // Memory test suit
  runner.addTest( MemoryTest::suite() );

  // Change the default outputter to a compiler error format outputter
  runner.setOutputter( new CppUnit::CompilerOutputter( &runner.result(), std::cerr ) );


  bool success = runner.run();
  return success ? 0 : 1;
}
