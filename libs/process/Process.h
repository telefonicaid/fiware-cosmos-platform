#ifndef _H_PROCESS
#define _H_PROCESS

#include "ProcessBase.h"			// ss::Process
#include "ModulesManager.h"			// ss::ModulesManager	
#include "samson/Environment.h"		// ss::Environment

namespace ss {

	class ProcessOperationFramework;	

	class Process : public ProcessBase
	{
	public:
		
		ModulesManager modulesManager;	// Ower own modules manager
		Environment environment;
		
		Process(int rFd, int wFd ) : ProcessBase( rFd ,  wFd )
		{
		}
		
		virtual network::ProcessMessage runCommand( network::ProcessMessage );
		
	};

}

#endif
