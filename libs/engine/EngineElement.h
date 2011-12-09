
/* ****************************************************************************
 *
 * FILE            EngineElement
 *
 * AUTHOR          Andreu Urruela
 *
 * DATE            July 2011
 *
 * DESCRIPTION
 *
 * Element defining a simple foreground task to be executed in the main thread
 * This is the basic elements of the Engine library and can be scheduled using engine::Engine::shared()->add( . )
 *
 * ****************************************************************************/

#ifndef _H_ENGINE_ELEMENT
#define _H_ENGINE_ELEMENT

#include <sstream>                      // std::ostringstream 
#include <string>

#include "logMsg/logMsg.h"              // Lmt
#include "logMsg/traceLevels.h"         // LmtEngine

#include "au/string.h"                  // au::Format
#include "au/namespace.h"

NAMESPACE_BEGIN(engine)

class EngineElement 
{
    
    //time_t thiggerTime;							// Delay to process this item
    int delay;									// Delay of execution or period in repeated operations
    bool repeated;								// Flag to determine if it is necessary to repeated the process
    int counter;                                // Number of times this element has been executed ( only in repeated )
    
protected:
    
    std::string description;                    // String for easy debugging
    std::string shortDescription;               // Short description
    
public:
    time_t thiggerTime;							// Delay to process this item
    
    virtual void run()=0;						// Run method to execute
    
    // Constructor for inmediate action or repeated actions
    EngineElement();
    EngineElement( int seconds );
    
    // Virtual destructor necessary to destory children-classes correctly
    virtual ~EngineElement(){};
    
    // Reschedule action once executed
    void Reschedule();
    void Reschedule(time_t now);
    
    time_t getThriggerTime();
    
    bool isRepeated();
    
    // Get a description string for debuggin
    std::string getDescription();
    std::string getShortDescription();
    
    // get xml information
    void getInfo( std::ostringstream& output);
    
};

// Class used only to test the automatic cancelation works

class EngineElementSleepTest : public EngineElement
{
    
public:
    
    EngineElementSleepTest() : EngineElement(10)
    {
        description = au::str("Sleep element just to sleep %d seconds", 10 );    
        shortDescription = description;
    }
    
    virtual void run()
    {
        LM_M(("Running EngineElementSleepTest for 60 seconds..."));
        sleep( 60 );    // Test the program exit
        LM_M(("End of Running EngineElementSleepTest for 60 seconds..."));
    }
    
};

NAMESPACE_END

#endif
