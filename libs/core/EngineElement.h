
#ifndef _H_ENGINE_ELEMENT
#define _H_ENGINE_ELEMENT

#include <sstream>			// std::ostringstream 
#include <string>
#include "logMsg.h"			// Lmt
#include "traceLevels.h"	// LmtEngine

namespace ss
{
	class EngineElement 
	{
		
		time_t thiggerTime;							// Delay to process this item
		int delay;									// Delay of execution or period in repeated operations
		bool repeated;								// Flag to determine if it is necessary to repeated the process
		
	protected:
		std::string description;					// String for easy debugging
	public:
		
		virtual void run()=0;						// Run method to execute
		
		// Simple inmediate action
		
		EngineElement()
		{
			delay = 0;
			repeated = false;
			thiggerTime = time(NULL) + delay;
			
			description = "Unkown non-repeated engine element";
		}
		
		// Repeated action
		
		EngineElement( int seconds )
		{
			delay = seconds;
			repeated = true;
			thiggerTime = time(NULL) + delay;
			
			std::ostringstream txt;
			txt << "Unkown repeated ( every " << seconds  << " seconds ) engine element";
			description = txt.str();
		}
		
		// Reschedule action once executed
		
		void Reschedule()
		{
			thiggerTime += delay;
		}
		
		time_t getThriggerTime()
		{
			return thiggerTime;
		}
		
		bool isRepeated()
		{
			return repeated;
		}
		
		std::string getDescription()
		{
			return description;
		}
		
	};
	
	/**
	 Nothing EngineElement. It is used to loop infinetelly the engine loop...
	 */
	
	class NothingEngineElement : public EngineElement
	{
		std::string text;
	public:
		
		NothingEngineElement( ) : EngineElement( 10 )	// Loop every 10 seconds to keep everything alive
		{
			description = "NothingEngineElement";
		}
		
		void run()
		{
			// Nothing to do
			LM_T( LmtEngine , ("Loop of the NothingEngineElement to keep everything live"));
		}
	};
	
    

	
	
}

#endif
