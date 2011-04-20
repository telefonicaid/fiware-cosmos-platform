
#ifndef _H_ENGINE_ELEMENT
#define _H_ENGINE_ELEMENT

#include <sstream>			// std::ostringstream 
#include <string>
#include "logMsg.h"			// Lmt
#include "traceLevels.h"	// LmtEngine
#include "au/Format.h"         // au::Format

namespace engine
{
	class EngineElement 
	{
		
		time_t thiggerTime;							// Delay to process this item
		int delay;									// Delay of execution or period in repeated operations
		bool repeated;								// Flag to determine if it is necessary to repeated the process
        int counter;                                // Number of times this element has been executed ( only in repeated )
		
	protected:
        
		std::string description;                    // String for easy debugging
        std::string shortDescription;               // Short description
        
	public:
		
		virtual void run()=0;						// Run method to execute
		
		// Constructor for inmediate action or repeated actions
		EngineElement();
		EngineElement( int seconds );

		// Reschedule action once executed
		void Reschedule();
		
		time_t getThriggerTime();
		
		bool isRepeated();

		// Get a description string for debuggin
		std::string getDescription();
        std::string getShortDescription();
		
	};

	
	
}

#endif
