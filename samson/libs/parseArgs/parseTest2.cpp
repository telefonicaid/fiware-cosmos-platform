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
#include "logMsg.h"
#include "parseArgs.h"
#include <stdio.h>

int main(int argC, char *argV[]) {
  char *fName = "main";

  paConfig("prefix", "P2_");
  paConfig("log to file", "/tmp/");
  paConfig("msgs to stdout", (void *)FALSE);
  paParse(NULL, argC, argV, 1, FALSE);

  LM_V(("verbose message"));
  if (paResultString[0] != 0) {
    printf("Got a paResultString:\n%s", paResultString);
  } else if (paWarnings != 0) {
    int ix;

    printf("Got warnings:\n");
    for (ix = 0; ix < paWarnings; ix++) {
      printf("Severity %02d: %s\n", paWarning[ix].severity, paWarning[ix].string);
    }
    printf("\n");
  }

  return 0;
}

