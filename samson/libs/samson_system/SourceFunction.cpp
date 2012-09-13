

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
