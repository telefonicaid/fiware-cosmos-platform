
/* ****************************************************************************
 *
 * FILE                     Process.cpp
 *
 * AUTHOR                   Ken Zangelin
 *
 * CREATION DATE            Nov 8 2010
 *
 */
#include <iostream>                     // std::cout

#include "ProcessBase.h"                    // Own interface
#include "logMsg.h"                     // LM_*
#include "Alarm.h"                      // ALARM
#include "processTraceLevels.h"         // LMT_*
#include "iomConnect.h"                 // iomConnect
#include "iomMsgAwait.h"                // iomMsgAwait
#include "iomMsgRead.h"                 // iomMsgRead
#include "iomMsgSend.h"                 // iomMsgSend
#include "Message.h"                    // Message::*
#include "ProcessWriter.h"              // ss::ProcessOperationFramework
#include "ProcessOperationFramework.h"  // ss::ProcessOperationFramework
#include "ProcessAssistant.h"			// ProcessAssistant::finish

#include <sys/uio.h>
#include <unistd.h>

namespace ss {
	
	/* ****************************************************************************
	 *
	 * Constructor - 
	 */
	ProcessBase::ProcessBase(int _rFd, int _wFd)
	{
		rFd      = _rFd;
		wFd      = _wFd;
	}
		
	
	/* **********************************************************************
	 *		 
	 * run - 
	 *
	 * Block method until a command is received from the ProcessAssistant (via pipe)
	 * When a command is received, runCommand will be executed.
	 * runCommand calls passCommand to send messages back to the parent
	 */
	void ProcessBase::run(void)
	{
		while (1)
		{
			network::ProcessMessage p;
			
			read(p);
			
			LM_T(60, ("Read command message '%d' - running it!", p.code() ));
			network::ProcessMessage ans = runCommand(p);
			
			LM_T(60, ("after running command '%d'", p.code() ));
			write( ans );
		}
	}
	
	
	/* ****************************************************************************
	 *
	 * passCommand - 
	 */
	network::ProcessMessage ProcessBase::passCommand(network::ProcessMessage p)
	{
		write( p );
		
		network::ProcessMessage ans;
		read( ans);
		
		return ans;
		
	}
}