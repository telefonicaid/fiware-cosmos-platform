#include <string.h>             /* strcmp                                    */

#include "paIsSet.h"            /* Own interface                             */



/* ****************************************************************************
 *
 * paIsSet - is an argument existing in the parse list
 */
bool paIsSet(int argC, char *argV[], const char *option) {
  int i;

  for (i = 1; i < argC; i++) {
    if (strcmp(argV[i], option) == 0)
      return true;
  }

  return false;
}

/* ****************************************************************************
 *
 * paIsSetSoGet - return value of option 'option'
 */
const char *paIsSetSoGet(int argC, char *argV[], const char *option) {
  int i;

  for (i = 1; i < argC; i++) {
    if (strcmp(argV[i], option) == 0)
      return argV[i + 1];
  }

  return NULL;
}

