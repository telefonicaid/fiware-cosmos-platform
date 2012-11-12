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
#ifndef GLOBALS_H
#define GLOBALS_H

/* ****************************************************************************
*
* FILE                  globals.h -  
*
*
*
*/
#include <string>



/* ****************************************************************************
*
* MAX - 
*/
#define MAX(a, b) (((a) > (b))? (a) : (b))


/* ****************************************************************************
*
* TF - true or false
*/
#define TF(b) ((b == true)? "true" : "false")



/* ****************************************************************************
*
* version - 
*/
extern std::string version;

#endif
