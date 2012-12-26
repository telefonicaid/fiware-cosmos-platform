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
#ifndef PA_IS_SET_H
#define PA_IS_SET_H

/* ****************************************************************************
 *
 * FILE                  paIsSet.h
 *
 * AUTHOR                Ken Zangelin
 *
 */



/* ****************************************************************************
 *
 * paIsSet - is an argument existing in the parse list
 */
extern bool paIsSet(int argC, char *argV[], const char *option);



/* ****************************************************************************
 *
 * paIsSetSoGet - return value of option 'option'
 */
extern const char *paIsSetSoGet(int argC, char *argV[], const char *option);

#endif
