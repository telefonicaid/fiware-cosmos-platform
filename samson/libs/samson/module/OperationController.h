
#ifndef _H_SS_OPERATION_CONTROLLER
#define _H_SS_OPERATION_CONTROLLER


#include <stdarg.h>             /* va_start, va_arg, va_end                  */
#include <stdio.h>

namespace samson {
// Interface of the operation to report stuff

class OperationController {
public:

  // Function to report progress of the operation
  virtual void reportProgress(double p) = 0;
};
}

#endif
