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
