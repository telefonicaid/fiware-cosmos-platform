/* ****************************************************************************
*
* FILE                     Process.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Nov 8 2010
*
*/
#include "logMsg.h"                // LM_*
#include "Alarm.h"                 // ALARM
#include "processTraceLevels.h"    // LMT_*

#include "iomConnect.h"            // iomConnect
#include "iomMsgAwait.h"           // iomMsgAwait
#include "iomMsgRead.h"            // iomMsgRead
#include "iomMsgSend.h"            // iomMsgSend
#include "Message.h"               // Message::*
#include "Process.h"               // Own interface

#include <iostream>					// std::cout

namespace ss {



/* ****************************************************************************
*
* Constructor - 
*/
Process::Process(int _rFd, int _wFd, int _core, int _workers)
{
	rFd      = _rFd;
	wFd      = _wFd;
	core     = _core;
	workers  = _workers;

	LM_M(("Started process on core %d in set of %d workers"));
}



/* **********************************************************************
*		 
* run - 
*
* Block method until a command is received from the ProcessAssistant (via pipe)
* When a command is received, runCommand will be executed.
* runCommand calls passCommand to send messages back to the parent
*/
void Process::run(void)
{
	int                   fds;
	char                  command[64];
	void*                 dataP = command;
	int                   dataLen;
	Message::MessageCode  msgCode;
	Message::MessageType  msgType;

	while (1)
	{
		LM_T(60, ("Awaiting message on fd %d", rFd));
		fds = iomMsgAwait(rFd, -1, -1);
		if (fds != 1)
		{
			LM_W(("iomMsgAwait returned %d", fds));
			sleep(1);  // sleep a little before calling iomMsgAwait again
			continue;
		}

		int s;
		s = iomMsgRead(rFd, (char*) "father", &msgCode, &msgType, &dataP, &dataLen, NULL, NULL, 0);
		LM_T(60, ("reading on fd %d got s=%d", rFd, s));
		if (s == -2)
			LM_X(1, ("father died - I die as well"));
		else if (s != 0)
			LM_X(1, ("iomMsgRead returned error %d", s));

		if (msgCode != Message::Command)
			LM_X(1, ("father sent me a '%s' message - I expect only 'Command' messages ...", messageCode(msgCode)));

		LM_T(60, ("Read command message '%s' - running it!", (char*) dataP));
		runCommand((char*) dataP);
		LM_T(60, ("after running command '%s'", dataP));

		if (dataP != command)
		{
			LM_T(60, ("NOT freeing data pointer for command - talk to Andreu about this - may fail whenever dataP was big enough ..."));
			// free(dataP);
		}
	}
}



/* **********************************************************************
*
* runCommand - 
*
* This command implementes how to run the process sent from the ProcessAssistant
* This operation can be a map, a reduce, a generator, etc..
* During this method execution, passCommand method can be called multiple time to pass commands to the user
* Any of this commands can be neither finish nor error
* 
* When runCommand finishes, a message is passed to the ProcessAssistant ( finish or error )
*/
void Process::runCommand(const char* com)
{
	LM_T(60, ("running command '%s'", com));

	sleep(1);
	passCommand("finish");

	LM_T(60, ("command '%s' finished", com));
}



/* ****************************************************************************
*
* passCommand - 
*/
char* Process::passCommand(const char* command)
{
	int                   fds;
	char                  out[64];
	void*                 dataP    = out;
	int                   dataLen  = sizeof(out);
	Message::MessageCode  msgCode;
	Message::MessageType  msgType;

	LM_T(60, ("passing command '%s' to father", command));

	iomMsgSend(wFd, "father", progName, Message::Command, Message::Msg, (void*) command, strlen(command));

	fds = iomMsgAwait(rFd, 5, 0);
	if (fds != 1)
		LM_W(("iomMsgAwait returned %d", fds));
	else
		iomMsgRead(rFd, "father", &msgCode, &msgType, &dataP, &dataLen, NULL, NULL, 0);

	if (dataP != out)
		free(dataP);

	return (char*) "ok";
}

}
