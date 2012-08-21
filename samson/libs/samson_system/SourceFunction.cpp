

#include "SourceFunction.h"  // Own interface


namespace samson {
namespace system {
// Manager to instanciate all possible operations
SourceFunctionManager source_function_manager;


SourceFunction *SourceFunction::getSourceForFunction(std::string name
                                                     , au::vector<Source>& input_sources
                                                     , au::ErrorManager *error) {
  return source_function_manager.getInstance(name, input_sources, error);
}
}
}