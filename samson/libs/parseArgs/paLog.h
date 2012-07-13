#ifndef PA_LOG_H
#define PA_LOG_H

/* ****************************************************************************
*
* FILE                  paLog.h
*
* AUTHOR                Ken Zangelin
*
*/
#include <stdio.h>              /* printf                                    */
#include <errno.h>              /* errno, strerror                           */

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
#endif

#endif
