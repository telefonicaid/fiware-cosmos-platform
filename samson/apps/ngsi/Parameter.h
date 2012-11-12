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
#ifndef PARAMETER_H
#define PARAMETER_H

/* ****************************************************************************
*
* FILE                     Parameter.h - 
*
*
*
*/



/* ****************************************************************************
*
* Parameter - 
*/
typedef struct Parameter
{
    char* name;
    char* value;
} Parameter;



/* ****************************************************************************
*
* parametersParse - 
*/
extern void parametersParse(char* params);



/* ****************************************************************************
*
* parametersPresent - 
*/
extern void parametersPresent(void);

#endif
