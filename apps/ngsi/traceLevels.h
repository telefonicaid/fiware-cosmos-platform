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

	LmtPeer = 31,
    LmtOperation,
    LmtParameters,
    LmtParameters2,
	LmtHttpHeader,

	LmtHttpData = 41,
	LmtHttpDataSeparate,
	LmtEntity,
	LmtRegisterContext,
	LmtDuration,

	LmtRegistration = 51,
	LmtRest,
	LmtRestPath,

	LmtAttribute = 61,
	LmtMetadata,

	LmtDbTable   = 71,
	LmtDbRegReq,
	LmtDbEntity,
	LmtDbId
} TraceLevels;

#endif
