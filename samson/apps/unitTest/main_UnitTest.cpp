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
 * FILE            main_UnitTest.cpp
 *
 * AUTHOR          Javier Lois
 *
 * DATE            December 2011
 *
 * DESCRIPTION
 *
 * Main file for the automatic unit testing application
 *
 */

#include "au/log/LogCentral.h"
#include "au/log/LogCommon.h"
#include "gtest/gtest.h"
#include "logMsg/logMsg.h"
#include "parseArgs/paBuiltin.h"        // paLsHost, paLsPort
#include "parseArgs/paConfig.h"         // paConfig()
#include "parseArgs/paIsSet.h"
#include "parseArgs/parseArgs.h"
#include "samson/common/SamsonSetup.h"
#include <stdio.h>


extern bool lmAssertAtExit;

/* ****************************************************************************
 *
 * parse arguments
 */
PaArgument paArgs[] =
{
  PA_END_OF_ARGS
};


/* ****************************************************************************
 *
 * global variables
 */
int logFd             = -1;

/* ****************************************************************************
 *
 * main -
 */

int main(int argC, char **argV) {
  paConfig("usage and exit on any warning", (void *)true);
  paConfig("log to screen",                 (void *)"only errors");
  paConfig("log to screen",                 (void *)false);
  paConfig("log to file",                   (void *)true);
  paConfig("log file line format",          (void *)"TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
  paConfig("screen line format",            (void *)"TYPE@TIME  EXEC: TEXT");
  paConfig("default value", "-logDir",      (void *)"/var/log/samson");
  paConfig("man author",                    "Samson team");

  // Parse incomming arguments without being alteres by google test library arguments
  if ((argC > 2) &&  (strcmp(argV[1], "-t") == 0)) {
    paParse(paArgs, 3, (char **)argV, 3, false);
  } else {
    // Avoid parsing any argument
    paParse(paArgs, 1, (char **)argV, 1, false);
  }

  // Set assert flag to true ro force asserts instead of exits
  lmAssertAtExit = true;

  // Init log sytem
  au::LogCentral::InitLogSystem(argV[0]);
  au::log_central->AddScreenPlugin("screen", "[type][channel] text");

  // Set Error level for this channel to avoid unnecessary warning messages when testing stuff
  au::log_central->evalCommand("log_set system E");

  // Run all tests
  ::testing::InitGoogleTest(&argC, argV);
  int r = RUN_ALL_TESTS();


  au::LogCentral::StopLogSystem();

  return r;
}

