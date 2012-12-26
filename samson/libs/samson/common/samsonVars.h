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
#ifndef SAMSON_VARS_H
#define SAMSON_VARS_H

/* ****************************************************************************
 *
 * FILE                samsonVars.h - samson common variables
 *
 *
 *
 */
#include "samson/common/samsonDirectories.h"  // Default directories



/* ****************************************************************************
 *
 * SAMSON_ARG_VARS - global samson variables for all samson executables.
 *
 * NOTE
 * Also used as arguments for argument parsing
 */
#define SAMSON_ARG_VARS            \
  char samsonHome[1024];           \
  char samsonWorking[1024]



/* ****************************************************************************
 *
 * SAMSON_EXTERNAL_VARS - extern declaration of the global samson variables
 */
#define SAMSON_EXTERNAL_VARS       \
  extern char samsonHome[1024];    \
  extern char samsonWorking[1024]



/* ****************************************************************************
 *
 * SAMSON_ARGS - to be used in creating the argument vector for paParse
 */
#define SAMSON_WORKING_DEFAULT             "/var/samson"
#define SAMSON_HOME_DEFAULT                "/opt/samson"


#define SWD _i SAMSON_WORKING_DEFAULT
#define SHD _i SAMSON_HOME_DEFAULT

#define SAMSON_ARGS \
  { "-working",  samsonWorking,  "SAMSON_WORKING", PaString,  PaOpt, SWD,     PaNL,   PaNL,  "working directory"       },  \
  { "-home",     samsonHome,     "SAMSON_HOME",    PaString,  PaOpt, SHD,     PaNL,   PaNL,  "home directory"          }

#endif  // ifndef SAMSON_VARS_H
