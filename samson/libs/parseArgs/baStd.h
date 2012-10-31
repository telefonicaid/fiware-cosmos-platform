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
#ifndef BA_STD_H
#define BA_STD_H

/* ****************************************************************************
 *
 * FILE			baStd.h - standard include file for Basic Archive
 *
 * AUTHOR        Ken Zangelin
 */



/* ****************************************************************************
 *
 * useful macros
 */
#ifndef MIN
#  define MIN(a, b)    ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#  define MAX(a, b)    ((a) > (b) ? (a) : (b))
#endif



/* ****************************************************************************
 *
 * BA_VEC_SIZE - size of vector
 */
#define BA_VEC_SIZE(v) (sizeof(v) / sizeof(v[0]))



/* ****************************************************************************
 *
 * BA_FT - get boolean string
 */
#define BA_FT(b) \
  (((b) == true) ? "TRUE" : (((b) == false) ? "FALSE" : "undef"))



/* ****************************************************************************
 *
 * BA_ONOFF - return ON or OFF
 */
#define BA_ONOFF(b) \
  (((b) == TRUE) ? "ON" : (((b) == FALSE) ? "OFF" : "undef"))



/* ****************************************************************************
 *
 * baStoi - string to integer
 */
extern long baStoi(char *string, int *baseP = (int *)0, char *errorText = (char *)0);



/* ****************************************************************************
 *
 * baStof - string float to binary float
 */
extern float baStof(char *string);



/* ****************************************************************************
 *
 * baStod - string double to binary double
 */
extern double baStod(char *string);



/* ****************************************************************************
 *
 * baWs - is the character 'c' a whitespace (space, tab or '\n')
 */
extern bool baWs(char c);



/* ****************************************************************************
 *
 * baWsNoOf - number of whitespaces in the string 'string'
 */
extern int baWsNoOf(char *string);


/* ****************************************************************************
 *
 * baWsStrip -
 */
char *baWsStrip(char *string);



/* ****************************************************************************
 *
 * baWsOnly -
 */
extern bool baWsOnly(char *str);



/* ****************************************************************************
 *
 * baCharCount -
 */
extern int baCharCount(char *line, char c);

#endif  // ifndef BA_STD_H
