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

/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_mob_script_generate_results_QA
#define _H_SAMSON_mob_script_generate_results_QA


#include <samson/module/samson.h>


namespace samson{
namespace mob{


	class script_generate_results_QA : public samson::Script
	{

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: Date system.Void  
input: system.UInt64 mob.MobilityDegree  
input: system.UInt64 mob.Place  
input: system.UInt64 mob.Place  
output: system.UInt64 mob.Results

extendedHelp: 		Script to generate final mobility results.

#endif // de INFO_COMMENT



	};


} // end of namespace mob
} // end of namespace samson

#endif
