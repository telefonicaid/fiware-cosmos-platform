#include <string.h>                     // strerror
#include <errno.h>                      // errno
#include <sys/wait.h>                   // wait

#include "logMsg.h"                     // LM_RE, ...
#include "ProcessCommunication.h"		// Own interface



namespace ss
{


/* ****************************************************************************
*
*  _read - blocking read
*/
int ProcessCommunication::_read(network::ProcessMessage &p)
{
	int   length;
	char* buffer;
		
	// Read the length
	int ans = ::read(rFd, &length, sizeof(int));
	if (ans == -1)
		LM_RE(-1, ("read(fd %d): %s", rFd, ::strerror(errno)));
	else if (ans == 0)
		LM_RE(-2, ("read ZERO bytes - probably connection closed ..."));
	else if (ans != sizeof(int))
		LM_RE(-1, ("read only %d bytes (%d expected)", ans, sizeof(int)));

	buffer = (char*) malloc(length);
		
	// Read the content of the buffer
	ans = ::read(rFd , buffer , length);
	if (ans == -1)
		LM_RE(-1, ("read(fd %d): %s", rFd, strerror(errno)));
	else if (ans == 0)
		LM_RE(-1, ("read ZERO bytes - should NOT be a connection closed ..."));
	else if (ans != length)
		LM_RE(-1, ("read only %d bytes (%d expected)", ans, length));
	
	// Parse the buffer
	p.ParseFromArray(buffer, length);

	free(buffer);
		
	return 0;
}



/* ****************************************************************************
*
* _write
*/
int ProcessCommunication::_write(network::ProcessMessage &p)
{
	int length = p.ByteSize();

	char *buffer;
	buffer = (char*)malloc(length);
		
	p.SerializeToArray(buffer , length);
		
	int ans = ::write(wFd , &length , sizeof(int));
		
	if (ans != sizeof(int))
		return -1;
			
	ans = ::write(wFd , buffer , length);
		
	if (ans != length)
		return -1;
		
	free(buffer);
		
	return 0;
}
	


/* ****************************************************************************
*
* _wait
*/
void ProcessCommunication::_wait(void)
{
	int pid;
	int status;

	LM_M(("Waiting for a dead process"));
	pid = ::wait(&status);

	LM_W(("Process %d died with status 0x%x", pid, status));
}

}
