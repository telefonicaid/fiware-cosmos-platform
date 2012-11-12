/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
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
	LmtEmpty,
	LmtRest,
	LmtRestPath,
	LmtLogRequest,


	LmtRegistration = 41,
	LmtRegistrationId,
	LmtRegistrationLookup,
	LmtRegistrationDbLookup,
	LmtRegister,
	
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
	LmtAttributeInit = 80,

	LmtMetadata  = 81,
	LmtMetadataLookup,
	LmtMetadataDbLookup,
	LmtMetadataPresent,
	LmtMetadatasPresent,
	LmtMetadataToDb,

	LmtRegMetadata  = 91,
	LmtRegMetadataLookup,
	LmtRegMetadataDbLookup,
	LmtRegMetadataPresent,
	LmtRegMetadatasPresent,
	LmtRegMetadataToDb,

	LmtDbTable   = 101,
	LmtSqlQuery,
	LmtDbRegReq,
	LmtDbEntity,
	LmtDbId,
	LmtUpdate,

	LmtToTree = 111,
	LmtToRAM,
	LmtToDB,

	LmtHttpData = 121,
	LmtHttpDataSeparate,
	LmtRegisterContext,
	LmtDuration
} TraceLevels;

#endif
