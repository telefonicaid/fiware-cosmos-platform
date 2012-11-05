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
#ifndef PA_CONFIG_H
#define PA_CONFIG_H

/* ****************************************************************************
 *
 * FILE                  paConfig.h
 *
 * AUTHOR                Ken Zangelin
 *
 */
#include "baStd.h"               /* bool, ...                            */

#include "parseArgs/parseArgs.h"  /* Where we have the ext decl of paConfig    */



/* ****************************************************************************
 *
 * Configurable variables
 */
extern bool paUseBuiltins;
extern bool paExitOnError;
extern bool paExitOnUsage;
extern bool paPrintErrorsOnStderr;
extern char *paBuiltinPrefix;
extern char *paPrefix;
extern char *paRcFileName;
extern char *paRcFileDir;
extern char *paGenericRcDir;
extern char *paProgName;
extern char paTraceV[1024];

extern char *paHelpFile;
extern char *paHelpText;

extern char *paManSynopsis;
extern char *paManShortDescription;
extern char *paManDescription;
extern char *paManExitStatus;
extern char *paManReportingBugs;

extern char *paManCopyright;
extern char *paManVersion;
extern char *paManAuthor;

extern bool paUsageOnAnyWarning;

extern bool paLogToFile;
extern bool paLogToScreen;
extern bool paLogScreenToStderr;
extern bool paLogScreenOnlyErrors;

extern char *paLogFilePath;
extern char *paLogFileLineFormat;
extern char *paLogFileTimeFormat;
extern char *paLogScreenLineFormat;
extern char *paLogScreenTimeFormat;

extern char *paTracelevels;

extern bool paVerbose;
extern bool paVerbose2;
extern bool paVerbose3;
extern bool paVerbose4;
extern bool paVerbose5;
extern bool paDebug;
extern bool paToDo;
extern bool paReads;
extern bool paWrites;
extern bool paFix;
extern bool paBug;
extern bool paBuf;
extern bool paDoubt;

extern bool paMsgsToStdout;
extern bool paMsgsToStderr;

extern char paPid[16];




/* ****************************************************************************
 *
 * paConfig -
 */
extern int paConfig(const char *item, const void *value, const void *value2 = (const void *)0);



/* ****************************************************************************
 *
 * paConfigActions -
 */
extern int paConfigActions(bool preTreat);



/* ****************************************************************************
 *
 * paConfigCleanup -
 */
extern void paConfigCleanup(void);

#endif  // ifndef PA_CONFIG_H
