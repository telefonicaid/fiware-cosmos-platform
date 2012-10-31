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
#include <stdio.h>               /* sprintf                                   */

#include "paFullName.h"          /* Own interface                             */
#include "parseArgs/parseArgs.h"  /* PaArgument                                */



/* ****************************************************************************
 *
 * paFullName - fill in name and description in string string
 */
char *paFullName(char *string, PaiArgument *aP) {
  if ((aP->option == NULL) || (aP->option[0] == 0)) {
    sprintf(string, "variable %s", aP->description);
  } else if (aP->type == PaBoolean) {
    sprintf(string, "%s (%s)", aP->name, aP->description);
  } else {
    sprintf(string, "%s <%s>", aP->name, aP->description);
  } return string;
}

