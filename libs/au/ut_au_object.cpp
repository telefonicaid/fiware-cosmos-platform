
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#include "parseArgs/parseArgs.h"
#include "parseArgs/paConfig.h"

#include "logMsg/logMsg.h"

#include "au/Object.h"
#include "au/ObjectVector.h"
#include "au/ObjectDictionary.h"


/* ****************************************************************************
 *
 * UT_C - Unit test check
 */
#define UT_C(b, s) if( b ) LM_X(1,s)

PaArgument paArgs[] =
{
    // No additional commands at the end
	PA_END_OF_ARGS
};

int logFd;

class TestObject : public au::Object
{
    int value;
        
    public:
    
    TestObject( int _value )
    {
        value = _value;
    }
    
    int getValue()
    {
        return value;
    }
    
};

void test_object_vector()
{
    
    // Vector of elements
    au::ObjectVector* vector = new au::ObjectVector();
    
    // Create an object
    LM_V(("Create a simple object"));
    TestObject *object = new TestObject(10);

    // Put into the vector ( now it comes with two owners )
    LM_V(("Put object into a vector"));
    vector->push_back( object );
    
    UT_C( object->getRetainCount() != 2 , ("Error") );
    
    LM_V(("Release object"));
    object->release();

    LM_V(("Get the object from the vector"));
    TestObject *object2 = dynamic_cast<TestObject*>( vector->extractFromBack() );
    
    LM_V(("Retain the object"));
    object2->retain();

    LM_V(("Clear the vector"));
    vector->clear();
    
    LM_V(("Check the value of the object"));
    UT_C( object2->getValue() != 10 , ("Error") );

    LM_V(("Release object"));
    object2->release();
    
    LM_V(("Release the vector itself"));
    vector->release();
    
}

void test_object_dictionary()
{
    
    // Vector of elements
    au::ObjectDictionary* dictionary = new au::ObjectDictionary();
    
    // Create an object
    LM_V(("Create a simple object"));
    TestObject *object = new TestObject(10);
    
    // Put into the vector ( now it comes with two owners )
    LM_V(("Put object into a dictionary"));
    dictionary->insertInMap("a", object );
    
    UT_C( object->getRetainCount() != 2 , ("Error") );
    
    LM_V(("Release object"));
    object->release();
    
    LM_V(("Get the object from the dictionary"));
    TestObject *object2 = dynamic_cast<TestObject*>( dictionary->extractFromMap("a") );
    
    LM_V(("Retain the object"));
    object2->retain();
    
    LM_V(("Clear the dictionary"));
    dictionary->clear();
    
    LM_V(("Check the value of the object"));
    UT_C( object2->getValue() != 10 , ("Error") );
    
    LM_V(("Release object"));
    object2->release();
    
    LM_V(("Release the dictionary itself"));
    dictionary->release();
    
}



int main( int argC , const char*argV[] )
{
    
    paConfig("usage and exit on any warning", (void*) true);
    
    paConfig("log to screen",                 (void*) true);
    paConfig("log to file",                   (void*) true);
    paConfig("screen line format",            (void*) "TYPE:EXEC: TEXT");
    paConfig("log to stderr",                 (void*) true);
    
    // Parse input arguments    
    paParse(paArgs, argC, (char**) argV, 1, false);
    logFd = lmFirstDiskFileDescriptor();
    
 
    // Run some test
    LM_M(("Running object vector test"));
    test_object_vector();
    
    LM_M(("Running object dictionary test"));
    test_object_dictionary();

    
    LM_M(("OK"));
    
}