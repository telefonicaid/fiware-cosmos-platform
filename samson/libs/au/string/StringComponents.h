
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

#include <cstring>
#include <list>
#include <math.h>
#include <string>    // std::String
#include <time.h>

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
