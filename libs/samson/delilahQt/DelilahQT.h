#ifndef _H_Delilah_QT
#define _H_Delilah_QT

/* ****************************************************************************
*
* FILE                     DelilahQT.h
*
* DESCRIPTION			   QT terminal for delilah
*
*/


#include "logMsg/logMsg.h"				
#include "samson/delilah/Delilah.h"			// samson::Delilah

namespace samson {
	
	/**
	 Main class for the DelilahQT program
	 */
	
	class DelilahQt : public Delilah
	{
		
	public:
		
		DelilahQt( NetworkInterface *network );        
        
        // Main run command ( it should be bloking )
        void run();
        
    };

}


#endif
