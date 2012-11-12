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
#ifndef PA_GET_VAL_H
#define PA_GET_VAL_H

/* ****************************************************************************
*
* FILE                  paGetVal.h
*
* AUTHOR                Ken Zangelin
*
*/



/* ****************************************************************************
*
* errors returned by function getVal
*/
typedef enum PaGetVal
{
	PaError = -1,
	PaOk    = 0,
	PaHex,
	PaOct,
	PaDec,
	PaDef,
	PaNotOk
} PaGetVal;



/* ****************************************************************************
*
* paGetVal - calculate the integer value of a string
*/
extern void* paGetVal(char* string, int* error);

#endif
