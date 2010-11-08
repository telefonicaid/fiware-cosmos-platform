#ifndef _H_PROCESS
#define _H_PROCESS



namespace ss {



/* ****************************************************************************
*
* Process - 
*
* This class is intanciated in the independent process created by any ProcessAssitant.
* There will be a pipe connecting ProcessAssitant and Process
*
* Basically this class blocks the main thread of this independent process in the "run" method until a command is received from the pipe.
* once a command is received, runCommand method is executed.
* During runCommand executiong, it can eventually be called the passCommand method to sent "messages" to the ProcessAssistant.
* This method should be blocking until ProcessAssitant confirms it has received and process the message
* Finally, when runCommand has finished a notification message is sent back to the ProcessAssistant to notify that everything has finished
* and run methods blocks again waiting for the next command
*/
	
class Process
{
public:
	Process(int fd);
	void     run(void);

private:
	void     runCommand(const char* command);
	char*    passCommand(const char* command);

	int fd;
};

}

#endif
