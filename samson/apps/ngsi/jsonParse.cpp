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
/* ****************************************************************************
*
* FILE                           jsonParse.cpp - 
*
*
*
*/
#include <sys/types.h>                  // system types ...

#include "json.h"
#include "logMsg/logMsg.h"              // LM_*

#include "traceLevels.h"                // Trace levels for log msg library
#include "Entity.h"                     // Entity
#include "jsonParse.h"                  // Own interface



/* ****************************************************************************
*
* jsonTypeName - 
*/
const char* jsonTypeName(json_type type)
{
	switch (type)
	{
	case json_type_null:       return "json_type_null";
	case json_type_boolean:    return "json_type_boolean";
	case json_type_double:     return "json_type_double";
	case json_type_int:        return "json_type_int";
	case json_type_object:     return "json_type_object";
	case json_type_array:      return "json_type_array";
	case json_type_string:     return "json_type_string";
	}

	return "json_type_unknown";
}



/* ****************************************************************************
*
* jsonParse - 
*/
int jsonParse(struct Entity* entityP, char* in)
{
	struct json_object* json;
	struct json_object* jsonObj;
	struct lh_table*    table;
	lh_entry*           entry;
	char*               key;

	json  = json_tokener_parse(in);
	table = json_object_get_object(json);
	entry = table->head;
	
	while (entry != NULL)
	{
		key     = (char*) entry->k;
		jsonObj = json_object_object_get(json, key);
		
		json_type type = json_object_get_type(jsonObj);

		if (type == json_type_boolean)
		{
			bool b = json_object_get_boolean(jsonObj);
			LM_T(LmtHttpData, ("json: '%s' : %s (%s)", key, (b==true)? "true" : "false", jsonTypeName(type)));
		}
		else if (type == json_type_double)
		{
			double d = json_object_get_double(jsonObj);
			LM_T(LmtHttpData, ("json: '%s' : %f (%s)", key, d, jsonTypeName(type)));
		}
		else if (type == json_type_int)
		{
			int i = json_object_get_int(jsonObj);
			LM_T(LmtHttpData, ("json: '%s' : %d (%s)", key, i, jsonTypeName(type)));
		}
		else if (type == json_type_string)
		{
			const char* s = json_object_get_string(jsonObj);
			LM_T(LmtHttpData, ("json: '%s' : %s (%s)", key, s, jsonTypeName(type)));
		}
			
		entry = entry->next;
	}

	return 0;
}
