#ifndef SAMSON_ALARM_H
#define SAMSON_ALARM_H

/* ****************************************************************************
*
* FILE                     Alarm.h - alarm definitions for all Samson IPC
*
*/
#include "samson/network/Endpoint.h"			// Endpoint



namespace samson
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
	BadRequest,
	CoreWorkerNotFound,
	CoreWorkerBusy,
	CoreWorkerDied,
	CoreWorkerNotRestarted
} Type;



/* ****************************************************************************
*
* AlarmData - 
*/
typedef struct AlarmData
{
	Severity    severity;
	Type        type;
	int         pad1;
	int         lineNo;	
	char        from[32];
	char        fileName[32];
	char        funcName[32];
	char        message[128];
} AlarmData;



/* ****************************************************************************
*
* ALARM - 
*/
#define ALARM(severity, type, message)                                                              \
do                                                                                                  \
{                                                                                                   \
    char* text;                                                                                     \
                                                                                                    \
	text = ::samson::Alarm::alarmTextGet message;					                                    \
	::samson::Alarm::alarmSend(controller, me, severity, type, __FILE__, __LINE__, __FUNCTION__, text); \
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
	samson::Endpoint*    controller,
	samson::Endpoint*    me,
	Severity         severity,
	Type             type,
	const char*      fileName,
	int              lineNo,
	const char*      funcName,
	char*            message
);



/* ****************************************************************************
*
* alarmSave - 
*/
extern void alarmSave
(
	Endpoint*    from,
	AlarmData*   alarmP
);

}
}

#endif
