#include <stdio.h>
#include <stdlib.h>            /* atexit                                     */
#include <fcntl.h>
#include <memory.h>            /* memset                                     */
#include <unistd.h>
#include <signal.h>            /* kill(), SIGINT                             */
#include <sys/termios.h>       /* struct termios                             */
#include <termio.h>            /* termio ...                                 */
#include <errno.h>             /* errno                                      */

#include "baTerm.h"            /* Own interface                              */



/* ****************************************************************************
*
* global variables
*/
static struct termios term;
static struct termios orgTerm;
static int            thisTty;
static int            termIsChanged = 0;



/* ****************************************************************************
*
* baTermSetup - 
*/
int baTermSetup(void)
{
	/* Set up terminal to send every character */
    
	if ((thisTty = open("/dev/tty", O_WRONLY)) < 0)
		return 1;
  
	/* Save the old terminal setting */
	if (tcgetattr(thisTty, &orgTerm) == -1)
		return 2;

	if (tcgetattr(thisTty, &term) == -1)
		return 3;


	setbuf(stdout, (char*) 0);
	setbuf(stdin, (char*) 0);

	term.c_iflag    |= ICRNL;
	term.c_lflag    &= ~ICANON;
	term.c_lflag    &= ~ISIG;
	term.c_lflag    &= ~ECHO;
	term.c_lflag    |= ECHOE;

	term.c_cc[VMIN]  = 1;
	term.c_cc[VTIME] = 0;

	/* Set terminal for this session */
	if (tcsetattr(thisTty, TCSAFLUSH, &term) < 0) 
		return 4;

	termIsChanged = 1;

	atexit(baTermReset);

	return 0;
}



/* ****************************************************************************
*
* baTermReset - 
*/
void baTermReset(void)
{
	int x;

	if (termIsChanged == 0)
		return;

	do
	{
		x = tcsetattr(thisTty, TCSAFLUSH, &orgTerm);
	} while (x == -1 && errno == EINTR);
}
