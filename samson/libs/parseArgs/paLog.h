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
#ifndef PA_LOG_H
#define PA_LOG_H

/* ****************************************************************************
 *
 * FILE                  paLog.h
 *
 * AUTHOR                Ken Zangelin
 *
 */
#include <errno.h>              /* errno, strerror                           */
#include <stdio.h>              /* printf                                    */

extern bool paLogOn;

#define PA_DEBUG

#ifdef PA_DEBUG

#define LOG printf   /* used by PA_M, PA_E & PA_P if PA_DEBUG defined */

#define PA_M(s)                                                     \
  do {                                                                \
    if (paLogOn == true)                                          \
    {                                                             \
      LOG("M: %s/%s[%d]: ", __FILE__, __FUNCTION__, __LINE__);    \
      LOG s;                                                      \
      LOG("\n");                                                  \
    }                                                             \
  } while (0)

#define PA_E(s)                                                     \
  do {                                                                \
    if (paLogOn == true)                                          \
    {                                                             \
      LOG("E: %s/%s[%d]: ", __FILE__, __FUNCTION__, __LINE__);    \
      LOG s;                                                      \
      LOG("\n");                                                  \
    }                                                             \
  } while (0)

#define PA_W(s)                                                     \
  do {                                                                \
    if (paLogOn == true)                                          \
    {                                                             \
      LOG("W: %s/%s[%d]: ", __FILE__, __FUNCTION__, __LINE__);    \
      LOG s;                                                      \
      LOG("\n");                                                  \
    }                                                             \
  } while (0)

#define PA_P(s)                                                     \
  do {                                                                \
    if (paLogOn == true)                                          \
    {                                                             \
      LOG("E: %s/%s[%d]: ", __FILE__, __FUNCTION__, __LINE__);    \
      LOG s;                                                      \
      LOG(": %s\n", strerror(errno));                             \
    }                                                             \
  } while (0)

#else

#  define PA_M(s)
#  define PA_E(s)
#  define PA_P(s)
#endif  // ifdef PA_DEBUG

#endif  // ifndef PA_LOG_H
