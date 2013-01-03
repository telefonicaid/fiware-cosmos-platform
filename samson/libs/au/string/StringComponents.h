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

/* ****************************************************************************
*
* FILE            StringComponent
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            2012
*
* DESCRIPTION
*
*  Class used to efficienly parse a string into components in a non-intrusive way
*  Original string is never modified
*
* ****************************************************************************/


#ifndef _H_AU_STRING_COMPONENT
#define _H_AU_STRING_COMPONENT

#include <math.h>
#include <time.h>

#include <cstring>
#include <list>
#include <string>    // std::String

#include "au/statistics/Cronometer.h"
#include "au/string/StringUtilities.h"
#include "au/string/xml.h"

namespace au {
class StringComponents {
public:

  StringComponents();
  ~StringComponents();

  // Main command to process line
  size_t ProcessLine(const char *line, size_t max_length, char separator);

  // Components of the last line
  std::vector<char *> components;

private:

  char *internal_line;
  size_t max_size;
};
}

#endif  // ifndef _H_AU_STRING_COMPONENT
