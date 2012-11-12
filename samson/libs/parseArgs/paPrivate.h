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
#ifndef PA_PRIVATE_H
#define PA_PRIVATE_H

/* ****************************************************************************
*
* FILE                  paPrivate.h
*
* AUTHOR                Ken Zangelin
*
*/
#include "baStd.h"                   /* BaBoolean                            */
#include "parseArgs/parseArgs.h"     /* PaArgument                           */



/* ****************************************************************************
*
* PA_IS_XXX
*/
#define PA_IS_OPTION(argP)      ((argP->what & PawOption)    == PawOption)
#define PA_IS_PARAMETER(argP)   ((argP->what & PawParameter) == PawParameter)
#define PA_IS_VARIABLE(argP)    ((argP->what & PawVariable)  == PawVariable)



/* ****************************************************************************
*
* PaWhat - 
*/
typedef enum PaWhat
{
   PawOption    = (1 << 0),
   PawParameter = (1 << 1),
   PawVariable  = (1 << 2),
   PawBuiltin   = (1 << 3)
} PaWhat;



/* ****************************************************************************
*
* PaTypeUnion - 
*/
typedef union PaTypeUnion
{
   char                c;
   unsigned char       uc;
   short               s;
   unsigned short      us;
   int                 i;
   unsigned int        ui;
   long long           l;
   unsigned long long  ul;
   char*               string;
   unsigned char*      ustring;
   bool                boolean;
} PaTypeUnion;



/* ****************************************************************************
*
* paBuiltin - 
*/
extern PaiArgument*  paiList;



/* ****************************************************************************
*
* paBuiltin - 
*/
extern PaiArgument paBuiltin[];



/* ****************************************************************************
*
* stoi - convert string to integer
*
* PREFIXES
*   B:    Boolean
*   0:    Octal
*   H:    Hexadecimal
*   H':   Hexadecimal
*   0x:   Hexadecimal
*/
extern int stoi(char* string);

#endif
