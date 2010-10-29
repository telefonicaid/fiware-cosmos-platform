#include <stdarg.h>             // va_start, va_arg, va_end
#include <stdio.h>              // vsnprintf
#include <string.h>             // strdup

#include "logMsg.h"             // LM_*
#include "Alarm.h"              // Own interface



namespace ss
{
namespace Alarm
{



/* ****************************************************************************
*
* alarmTextGet - format message into string
*/
char* alarmTextGet(const char* format, ...)
{
	va_list        args;
	char           vmsg[2048];
	char*          dupString;

	/* "Parse" the varible arguments */
	va_start(args, format);

	/* Print message to variable */
	vsnprintf(vmsg, sizeof(vmsg), format, args);
	vmsg[2047] = 0;
	va_end(args);

	dupString = (char*) strdup(vmsg);

	return dupString;
}



/* ****************************************************************************
*
* alarmSend - 
*/
void alarmSend
(
	Endpoint*    controller,
	Endpoint*    me,
	Severity     severity,
	Type         type,
	const char*  fileName,
	int          lineNo,
	const char*  funcName,
	char*        message
)
{
	LM_F(("ALARM: '%s'", message));
}

}
}
