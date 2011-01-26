#include <stdarg.h>             // va_start, va_arg, va_end
#include <stdio.h>              // vsnprintf
#include <string.h>             // strdup

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace levels

#include "Message.h"            // Message
#include "iomMsgSend.h"         // iomMsgSend
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
	Endpoint*    from,
	Severity     severity,
	Type         type,
	const char*  fileName,
	int          lineNo,
	const char*  funcName,
	char*        message
)
{
	AlarmData alarm;

	alarm.severity = severity;
	alarm.type     = type;
	alarm.pad1     = 0;
	alarm.lineNo   = lineNo;

	strncpy(alarm.from, from->name.c_str(), sizeof(alarm.from));
	strncpy(alarm.fileName, fileName, sizeof(alarm.fileName));
	strncpy(alarm.funcName, funcName, sizeof(alarm.funcName));
	strncpy(alarm.message, message, sizeof(alarm.message));

	iomMsgSend(controller, from, Message::Alarm, Message::Evt, &alarm, sizeof(alarm));
	LM_T(LmtAlarm, ("ALARM: '%s'", message));
}



/* ****************************************************************************
*
* alarmSave - 
*/
void alarmSave
(
	Endpoint*   from,
	AlarmData*  alarmP
)
{
	// Save alarm data to file
}

}
}
