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
#include <stdio.h>                   // NULL
#include <string.h>                  // strcmp

#include "ws.h"                      // Own interface



/* ****************************************************************************
*
* isWs - 
*/
bool isWs(char c)
{
    if (c == ' ')  return true;
    if (c == '\n') return true;
    if (c == '\r') return true;
    if (c == '	') return true;

    return false;
}



/* ****************************************************************************
*
* wsClean - remove starting and trailing whitespace
*/
char* wsClean(char* in)
{
    if (in == NULL)
        return NULL;

    while ((*in != 0) && isWs(*in))
        ++in;
    if (*in == 0)
        return NULL;

    while ((*in != 0) && isWs(in[strlen(in) -1]))
        in[strlen(in) -1] = 0;
    if (*in == 0)
        return NULL;

    return in;
}



/* ****************************************************************************
*
* onlyWS - 
*/
bool onlyWS(char* s)
{
	int ix;

	while (s[ix] != 0)
	{
		if (s[ix] == ' ')    { ++ix; continue; }
		if (s[ix] == '\n')   { ++ix; continue; }
		if (s[ix] == '	')   { ++ix; continue; }

		return false;
	}

	return true;
}
