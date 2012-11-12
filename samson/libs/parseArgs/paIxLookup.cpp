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
#include "baStd.h"              /* BA standard header file                   */
#include "logMsg/logMsg.h"             /* LM_ENTRY, LM_EXIT, ...                    */

#include "parseArgs/parseArgs.h"          /* PaArgument                                */
#include "paBuiltin.h"          /* paBuiltin, paBuiltinNoOf                  */
#include "paOptions.h"          /* paOptionsNoOf                             */
#include "paIxLookup.h"         /* Own interface                             */



/* ****************************************************************************
*
* paIxLookup - 
*/
PaiArgument* paIxLookup(PaiArgument* paList, int ix)
{
   int builtins = paBuiltinNoOf();

   if (ix < builtins)
      return &paBuiltin[ix];
   else if (ix < paOptionsNoOf(paList))
      return &paList[ix - builtins];

   return NULL;
}
