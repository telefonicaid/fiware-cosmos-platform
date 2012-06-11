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


	LmtRegistration = 41,
	LmtRegister,
	LmtEmpty,
	LmtRest,
	LmtRestPath,
	LmtLogRequest,
	
	LmtDiscover = 51,
	LmtDiscoverTreat,

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
	LmtToDB,

	LmtHttpData = 111,
	LmtHttpDataSeparate,
	LmtRegisterContext,
	LmtDuration
} TraceLevels;

#endif
