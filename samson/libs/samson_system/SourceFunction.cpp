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


#include "samson_system/SourceFunction.h"  // Own interface


namespace samson {
namespace system {
// Manager to instantiate all possible operations
SourceFunctionManager source_function_manager;


SourceFunction *SourceFunction::GetSourceForFunction(const std::string& function_name
                                                     , au::vector<Source>& input_sources
                                                     , au::ErrorManager *error) {
  return source_function_manager.getInstance(function_name, input_sources, error);
}
}
}
