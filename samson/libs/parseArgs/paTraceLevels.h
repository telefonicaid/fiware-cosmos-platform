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
#ifndef PA_TRACE_LEVELS_H
#define PA_TRACE_LEVELS_H

/* ****************************************************************************
 *
 * FILE                     paTraceLevels.h - trace levels for ParseArgs lib
 *
 * AUTHOR                   Ken Zangelin
 *
 * CREATION DATE            Oct 13 2010
 *
 */



/* ****************************************************************************
 *
 * PaTraceLevels -
 */
enum PaTraceLevels {
  LmtPaSetup          = 0,
  LmtPaConfigAction,
  LmtPaDefVal,
  LmtPaEnvVal,
  LmtPaRcFileVal,
  LmtPaComLineVal,
  LmtPaApVals,
  LmtPaLimits,
  LmtPaUsage,
  LmtPaRcFile,
  LmtPaSList,
  LmtPaIList,
  LmtHelp
};

#endif  // ifndef PA_TRACE_LEVELS_H
