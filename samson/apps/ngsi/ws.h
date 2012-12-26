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
#ifndef WS_H
#define WS_H


/* ****************************************************************************
*
* isWs - 
*/
extern bool isWs(char c);



/* ****************************************************************************
*
* wsClean - remove starting and trailing whitespace
*/
extern char* wsClean(char* in);



/* ****************************************************************************
*
* onlyWS - 
*/
extern bool onlyWS(char* s);

#endif
