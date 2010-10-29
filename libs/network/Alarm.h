#ifndef SAMSON_ALARM_H
#define SAMSON_ALARM_H

/* ****************************************************************************
*
* FILE                     Alarm.h - alarm definitions for all Samson IPC
*
*/
#include "Endpoint.h"			// Endpoint



namespace ss
{
namespace Alarm
{



/* ****************************************************************************
*
* Severity - 
*/
typedef enum Severity
{
	Debug,
	Verbose,
	Warning,
	Error,
	Severe
} Severity;



/* ****************************************************************************
*
* Type - 
*/
typedef enum Type
{
	CoreWorkerNotFound,
	CoreWorkerBusy,
	CoreWorkerDied,
	CoreWorkerNotRestarted
} Type;



/* ****************************************************************************
*
* ALARM - 
*/
#define ALARM(severity, type, message)                                                              \
do                                                                                                  \
{                                                                                                   \
    char* text;                                                                                     \
                                                                                                    \
	text = ::ss::Alarm::alarmTextGet message;					                                    \
	::ss::Alarm::alarmSend(controller, me, severity, type, __FILE__, __LINE__, __FUNCTION__, text); \
	if (text)                                                                                       \
		free(text);                                                                                 \
} while (0)



/* ****************************************************************************
*
* alarmTextGet - 
*/
char* alarmTextGet(const char* format, ...);



/* ****************************************************************************
*
* alarmSend - 
*/
extern void alarmSend
(
	ss::Endpoint*    controller,
	ss::Endpoint*    me,
	Severity         severity,
	Type             type,
	const char*      fileName,
	int              lineNo,
	const char*      funcName,
	char*            message
);

}
}

#endif
