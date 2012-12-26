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
