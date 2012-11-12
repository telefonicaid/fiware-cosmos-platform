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
* FILE                     Parameter.h - 
*
*
*
*/
#include <string>

#include "logMsg/logMsg.h"                  // LM_*

#include "traceLevels.h"                    // Trace levels for log msg library
#include "globals.h"                        // MAX
#include "Parameter.h"                      // Own interface

using namespace std;



/* ****************************************************************************
*
* Global variables
*/
static Parameter     parameterV[100];   // 100 is more than enough ...
static int           parameters       = 0;



/* ****************************************************************************
*
* parametersPresent - 
*/
void parametersPresent(void)
{
    int           ix;
    unsigned int  nameLen = 0;

    for (ix = 0; ix < parameters; ix++)
        nameLen = MAX(nameLen, strlen(parameterV[ix].name));

    char format[128];
    snprintf(format, sizeof(format), "param %%02d: %%-%ds %%s", nameLen + 1);

    LM_T(LmtParameters, (""));
    LM_T(LmtParameters, ("----- Parameters -----"));
    for (ix = 0; ix < parameters; ix++)
    {
        std::string name = std::string(parameterV[ix].name) + ":";

        LM_T(LmtParameters, (format, ix, name.c_str(), parameterV[ix].value));
    }
    LM_T(LmtParameters, ("----------------------"));
    LM_T(LmtParameters, (""));
}



/* ****************************************************************************
*
* parametersParse - 
*/
void parametersParse(char* params)
{
    int pIx = 0;

    memset(parameterV, 0, sizeof(parameterV));
    do
    {
        char* next = strchr(params, '&');
        char* eq;

        if (next != NULL)
        {
            *next = 0;
            ++next;
        }

        eq   = strchr(params, '=');
        if (eq)
        {
            *eq = 0;
            ++eq;

            parameterV[pIx].name  = params;
            parameterV[pIx].value = eq;

            LM_T(LmtParameters2, ("Found parameter %d: '%s' - '%s'", pIx, parameterV[pIx].name, parameterV[pIx].value));
            ++pIx;
        }
        else
            LM_W(("No '=' found: '%s'", params));

        params = next;
    } while (params != NULL);

    parameters = pIx;
    parametersPresent();
}



