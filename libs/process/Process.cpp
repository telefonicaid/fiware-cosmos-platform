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



namespace ss {



/* ****************************************************************************
*
* Constructor - 
*/
Process::Process(int _fd)
{
	fd = _fd;
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
		fds = iomMsgAwait(fd, -1, -1);
		if (fds != 1)
		{
			LM_W(("iomMsgAwait returned %d", fds));
			sleep(1);
			continue;
		}

		int s;
		s = iomMsgRead(fd, (char*) "father", &msgCode, &msgType, &dataP, &dataLen, NULL, NULL, 0);
		if (s == -2)
			LM_X(1, ("father died - I die as well"));
		else if (s != 0)
			LM_X(1, ("iomMsgRead returned error %d", s));

		if (msgCode != Message::Command)
			LM_X(1, ("father sent me a '%s' message - I expect only 'Command' messages ...", messageCode(msgCode)));

		LM_M(("Read command message '%s' - running it!", (char*) dataP));
		runCommand(command);

		if (dataP != command)
			free(dataP);
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
void Process::runCommand(const char* command)
{
	int x = 0;

	LM_M(("running command '%s'", command));

	while (1)
	{
		sleep(3);
		++x;

		if ((x % 3) == 0)
			passCommand("X is a multiple of three");

		if (x >= 20)
		{
			passCommand("finish");
			break;
		}
	}

	LM_M(("command '%s' finished", command));
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

	LM_M(("passing command '%s' to father", command));

	iomMsgSend(fd, "father", progName, Message::Command, Message::Msg, (void*) command, strlen(command));
	fds = iomMsgAwait(fd, 5, 0);
	if (fds != 1)
		LM_W(("iomMsgAwait returned %d", fds));
	else
		iomMsgRead(fd, "father", &msgCode, &msgType, &dataP, &dataLen, NULL, NULL, 0);

	if (dataP != out)
		free(dataP);

	return (char*) "ok";
}

}
