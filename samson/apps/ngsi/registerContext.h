#ifndef REGISTER_CONTEXT_H
#define REGISTER_CONTEXT_H

#include <string>
#include <vector>

#include "Verb.h"                           // Verb
#include "Format.h"                         // Format

using namespace std;



/* ****************************************************************************
*
* registerContext - 
*/
extern bool registerContext(int fd, Verb verb, Format format, char* data);

#endif
