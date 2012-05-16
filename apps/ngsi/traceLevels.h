#ifndef TRACE_LEVELS_H
#define TRACE_LEVELS_H



/* ****************************************************************************
*
* TraceLevels - 
*/
typedef enum TraceLevels
{
	LmtRead         = 21,
	LmtInput,
	LmtInputLines,
	LmtComponents,
	LmtSuffix,
	LmtPeer,
    LmtOperation,
    LmtParameters,
    LmtParameters2,
	LmtHttpHeader,
	LmtHttpData,
	LmtHttpDataSeparate,
	LmtEntity,
	LmtRegisterContext,
	LmtDuration,
	LmtRegistration
} TraceLevels;

#endif
