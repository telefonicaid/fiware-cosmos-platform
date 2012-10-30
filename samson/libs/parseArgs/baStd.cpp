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
#include <stdio.h>              /* sscanf                                    */
#include <stdlib.h>             /* atoi                                      */
#include <string.h>             /* strtoul, strncmp                          */

#include "baStd.h"              /* Own interface                             */



/* ****************************************************************************
 *
 * IntType -
 */
typedef enum IntType {
  Bin,
  Oct,
  Dec,
  Hex
} IntType;



/* ****************************************************************************
 *
 * baStoi - string to integer
 */
long baStoi(char *string, int *baseP, char *errorText) {
  char last;
  long multiplicator = 1;
  int sign          = 1;
  long value;
  // IntType type;
  int base;
  char *validchars = (char *)"Q";

  if ((string == NULL) || (string[0] == 0)) {
    return 0;
  }

  if (*string == '-') {
    ++string;
    sign = -1;
  }

  last = string[strlen(string) - 1];
  if (last == 'k') {
    multiplicator = 1024;
  } else if (last == 'M') {
    multiplicator = 1024 * 1024;
  } else if (last == 'G') {
    multiplicator = 1024 * 1024 * 1024;
  }

#ifdef __LP64__
  else if (last == 'T') {
    multiplicator = (long)1024 * 1024 * 1024 * 1024;
  } else if (last == 'P') {
    multiplicator = (long)1024 * 1024 * 1024 * 1024 * 1024;
  }

#endif

  if (multiplicator != 1) {
    string[strlen(string) - 1] = 0;
  }
  if (strncmp(string, "0x", 2) == 0) {
    // type        = Hex;
    base        = 16;
    string      = &string[2];
    validchars  = (char *)"0123456789abcdfeABCDEF";
  } else if (strncmp(string, "H'", 2) == 0) {
    // type        = Hex;
    base        = 16;
    string      = &string[2];
    validchars  = (char *)"0123456789abcdfeABCDEF";
  } else if (strncmp(string, "H", 1) == 0) {
    // type        = Hex;
    base        = 16;
    string      = &string[1];
    validchars  = (char *)"0123456789abcdfeABCDEF";
  } else if (strncmp(string, "0", 1) == 0) {
    // type        = Oct;
    base        = 8;
    string      = &string[1];
    validchars  = (char *)"01234567";
  } else if (strncmp(string, "B", 1) == 0) {
    // type        = Bin;
    base        = 2;
    string      = &string[1];
    validchars  = (char *)"01";
  } else {
    // type        = Dec;
    base        = 10;
    validchars  = (char *)"0123456789";
  }

  if (baseP) {
    *baseP = base;
  }
  if (strspn(string, validchars) != strlen(string)) {
    if (errorText) {
      sprintf(errorText, "bad string in integer conversion: '%s'", string);
    }
    return -1;
  }

  value = strtoull(string, NULL, base);

  return sign * multiplicator * value;
}

/* ****************************************************************************
 *
 * baStof - string float to binary float
 */
float baStof(char *string) {
  float f;

  sscanf(string, "%f", &f);

  return f;
}

/* ****************************************************************************
 *
 * baStod - string double to binary double
 */
double baStod(char *string) {
  double f;

  sscanf(string, "%lf", &f);

  return f;
}

/* ****************************************************************************
 *
 * baWs - is the character 'c' a whitespace (space, tab or '\n')
 */
bool baWs(char c) {
  switch (c) {
    case ' ':
    case '\t':
    case '\n':
      return true;

      break;
    default:
      return false;

      break;
  }
}

/* ****************************************************************************
 *
 * baWsNoOf - number of whitespaces in the string 'string'
 */
int baWsNoOf(char *string) {
  int no = 0;

  while (*string != 0) {
    if (baWs(*string) == true) {
      ++no;
    }
    ++string;
  }
  return no;
}

/* ****************************************************************************
 *
 * baWsStrip -
 */
char *baWsStrip(char *s) {
  char *str;
  char *tmP;
  char *toFree;

  if ((s == NULL) || (s[0] == 0)) {
    return s;
  }

  str = strdup(s);
  if (str == NULL) {
    s[0] = 0;
    return s;
  }

  toFree = str;
  while ((*str == ' ') || (*str == '\t')) {
    ++str;
  }

  tmP = &str[strlen(str) - 1];

  while ((tmP > str) && ((*tmP == ' ') || (*tmP == '\t'))) {
    --tmP;
  }
  ++tmP;
  *tmP = 0;

  if (str[0] != 0) {
    strcpy(s, str);
  } else {
    s[0] = 0;
  } free(toFree);

  return s;
}

/* ****************************************************************************
 *
 * baWsOnly -
 */
bool baWsOnly(char *str) {
  while (*str != 0) {
    if (baWs(*str) == false) {
      return false;
    }
    ++str;
  }

  return true;
}

/* ****************************************************************************
 *
 * baCharCount -
 */
int baCharCount(char *line, char c) {
  int noOf = 0;

  while (*line != 0) {
    if (*line == c) {
      ++noOf;
    }
    ++line;
  }

  return noOf;
}

