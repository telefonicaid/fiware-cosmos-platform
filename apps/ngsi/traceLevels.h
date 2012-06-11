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
	LmtRegisterContext,
	LmtDuration,

	LmtRegistration = 51,
	LmtRegister,
	LmtEmpty,
	LmtRest,
	LmtRestPath,
	LmtLogRequest,
	
	LmtEntity = 61,
	LmtEntityLookup,
	LmtEntityDbLookup,
	LmtEntityPresent,
	LmtEntitiesPresent,
	LmtEntityToDb,
	LmtEntityAttribute,
	
	LmtAttribute = 71,
	LmtAttributeLookup,
	LmtAttributeDbLookup,
	LmtAttributePresent,
	LmtAttributesPresent,
	LmtAttributeToDb,
	LmtAttributeCreate,
	LmtAttributeMetaId,
	LmtAttributeInDB,

	LmtMetadata  = 81,
	LmtMetadataLookup,
	LmtMetadataDbLookup,
	LmtMetadataPresent,
	LmtMetadatasPresent,
	LmtMetadataToDb,

	LmtDbTable   = 91,
	LmtSqlQuery,
	LmtDbRegReq,
	LmtDbEntity,
	LmtDbId,
	LmtUpdate,

	LmtToTree = 101,
	LmtToRAM,
	LmtToDB
} TraceLevels;

#endif
