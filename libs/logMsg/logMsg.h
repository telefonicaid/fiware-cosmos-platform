#ifndef LOG_MSG_H
#define LOG_MSG_H

/* ****************************************************************************
*
* FILE            logMsg.h - log message header file
*
* AUTHOR          Ken Zangelin
*
*/
#include <stdio.h>              /* !sprintf                                  */

#if !defined(__APPLE__)
#include <malloc.h>             /* free                                      */
#endif

#include <errno.h>              /* errno                                     */
#include <string.h>             /* strerror                                  */
#include <stdarg.h>             /* ellipses                                  */
#include <stdlib.h>				/* free()									 */


/******************************************************************************
*
* globals
*/
extern int    inSigHandler;
extern char*  progName;



/* ****************************************************************************
*
* LmStatus - 
*/
typedef enum LmStatus
{
   LmsOk = 0,

   LmsInitNotDone,
   LmsInitAlreadyDone,
   LmsNull,
   LmsFdNotFound,
   LmsFdInvalid,
   LmsFdOccupied,
   
   LmsMalloc,
   LmsOpen,
   LmsFopen,
   LmsLseek,
   LmsFseek,
   LmsWrite,
   LmsFgets,
   
   LmsNoFiles,
   LmsProgNameLevels,
   LmsLineTooLong,
   LmsStringTooLong,
   LmsBadFormat,
   LmsBadSize,
   LmsBadParams,
   LmsPrognameNotSet,
   LmsPrognameError,
   LmsClearNotAllowed
} LmStatus;



/* ****************************************************************************
*
* LmFormat - format for buffer presenting function
*/
typedef enum LmFormat
{
   LmfByte = 1,
   LmfWord = 2,
   LmfLong = 4
} LmFormat;



/* ****************************************************************************
*
* LmExitFp - function type for the exit function
*/
typedef void (*LmExitFp)(int code, void* input, char* logLine, char* stre);
typedef void (*LmWarningFp)(void* input, char* logLine, char* stre);
typedef void (*LmErrorFp)(void* input, char* logLine, char* stre);



/* ****************************************************************************
*
* LmWriteFp - alternative write function type
*/
typedef void (*LmWriteFp)(char*);



/* ****************************************************************************
*
* LmOutHook - type for function pointer for lmOut hook
*/
typedef void (*LmOutHook)(void* vP, char* text, char type, const char* date, const char* file, int lineNo, const char* fName, int tLev, const char* stre);



/* ****************************************************************************
*
* LmxFp - extra log function to call (for SGS use)
*/
typedef int (*LmxFp)(int, char*);



/* ****************************************************************************
*
* Predefined trace levels
*/
typedef enum LmTraceLevel
{
   LmtIn = 0,
   LmtFrom,
   LmtEntry,
   LmtExit,
   LmtLine,

   /* For listLib */
   LmtListCreate,
   LmtListInit,
   LmtListInsert,
   LmtListItemCreate,
   
   LmtUserStart
} LmTraceLevel;



/* ****************************************************************************
*
* LmTracelevelName - 
*/
typedef char* (*LmTracelevelName)(int level);



/* ****************************************************************************
*
* 
*/
#ifdef LM_OFF
 #define LM_NO_V
 #define LM_NO_M
 #define LM_NO_H
 #define LM_NO_W
 #define LM_NO_E
 #define LM_NO_P
 #define LM_NO_X
 #define LM_NO_XP
 #ifdef LM_ON
   #undef LM_ON
 #endif

#endif

#ifdef LM_NO_V
#define LM_V(s)
#define LM_V2(s)
#define LM_V3(s)
#define LM_V4(s)
#define LM_V5(s)
#else
/* ****************************************************************************
*
* LM_V - log verbose message
*/
#define LM_V(s)                                                               \
do {                                                                          \
   char* text;                                                                \
                                                                              \
   if (lmOk('V', 0) == LmsOk)                                                 \
   {                                                                          \
      if ((text = lmTextGet s) != NULL)                                       \
      {                                                                       \
         lmOut(text, 'V', __FILE__, __LINE__, (char*) __FUNCTION__, 0, NULL); \
         ::free(text);                                                        \
      }                                                                       \
   }                                                                          \
} while (0)

#define LM_V2(s)                                                              \
do {                                                                          \
   char* text;                                                                \
                                                                              \
   if (lmOk('2', 0) == LmsOk)                                                 \
   {                                                                          \
      if ((text = lmTextGet s) != NULL)                                       \
      {                                                                       \
         lmOut(text, '2', __FILE__, __LINE__, (char*) __FUNCTION__, 0, NULL); \
         ::free(text);                                                        \
      }                                                                       \
   }                                                                          \
} while (0)


#define LM_V3(s)                                                              \
do {                                                                          \
   char* text;                                                                \
                                                                              \
   if (lmOk('3', 0) == LmsOk)                                                 \
   {                                                                          \
      if ((text = lmTextGet s) != NULL)                                       \
      {                                                                       \
         lmOut(text, '3', __FILE__, __LINE__, (char*) __FUNCTION__, 0, NULL); \
         ::free(text);                                                        \
      }                                                                       \
   }                                                                          \
} while (0)


#define LM_V4(s)                                                              \
do {                                                                          \
   char* text;                                                                \
                                                                              \
   if (lmOk('4', 0) == LmsOk)                                                 \
   {                                                                          \
      if ((text = lmTextGet s) != NULL)                                       \
      {                                                                       \
         lmOut(text, '4', __FILE__, __LINE__, (char*) __FUNCTION__, 0, NULL); \
         ::free(text);                                                        \
      }                                                                       \
   }                                                                          \
} while (0)


#define LM_V5(s)                                                              \
do {                                                                          \
   char* text;                                                                \
                                                                              \
   if (lmOk('5', 0) == LmsOk)                                                 \
   {                                                                          \
      if ((text = lmTextGet s) != NULL)                                       \
      {                                                                       \
         lmOut(text, '5', __FILE__, __LINE__, (char*) __FUNCTION__, 0, NULL); \
         ::free(text);                                                        \
      }                                                                       \
   }                                                                          \
} while (0)

#define LM_VV     LM_V2
#define LM_VVV    LM_V3
#define LM_VVVV   LM_V4
#define LM_VVVVV  LM_V5
#endif


#ifdef LM_NO_M
#define LM_M(s)
#else
/* ****************************************************************************
*
* LM_M - log message
*/
#define LM_M(s)                                                            \
do {                                                                       \
   char* text;                                                             \
                                                                           \
   if ((text = lmTextGet s) != NULL)                                       \
   {                                                                       \
      lmOut(text, 'M', __FILE__, __LINE__, (char*) __FUNCTION__, 0, NULL); \
      ::free(text);                                                          \
   }                                                                       \
} while (0)
#endif


#ifdef LM_NO_H
#define LM_H(s)
#else
/* ****************************************************************************
*
* LM_H - "hidden" log message
*/
#define LM_H(s)                                                            \
do {                                                                       \
   char* text;                                                             \
                                                                           \
   if ((text = lmTextGet s) != NULL)                                       \
   {                                                                       \
      lmOut(text, 'H', __FILE__, __LINE__, (char*) __FUNCTION__, 0, NULL); \
      ::free(text);                                                          \
   }                                                                       \
} while (0)
#endif


#ifdef LM_NO_C
#define LM_S(s)
#else
/* ****************************************************************************
*
* LM_C - log message with timestamp
*/
#define LM_S(s)                                                            \
do {                                                                       \
   char* text;                                                             \
                                                                           \
   if ((text = lmTextGet s) != NULL)                                       \
   {                                                                       \
      lmOut(text, 'S', __FILE__, __LINE__, (char*) __FUNCTION__, 0, NULL); \
      ::free(text);                                                          \
   }                                                                       \
} while (0)
#endif


#ifdef LM_NO_F
#define LM_F(s)
#else
/* ****************************************************************************
*
* LM_F - log message
*/
#define LM_F(s)                                                          \
do {                                                                     \
   char* text;                                                           \
                                                                         \
   if ((text = lmTextGet s) != NULL)                                     \
   {                                                                     \
      lmOut(text, 'F', __FILE__, __LINE__, "***", 0, NULL);              \
      ::free(text);                                                        \
   }                                                                     \
} while (0)
#endif


#ifdef LM_NO_W
#define LM_W(s)
#else
/* ****************************************************************************
*
* LM_W - log warning message
*/
#define LM_W(s)                                                            \
do {                                                                       \
   char* text;                                                             \
                                                                           \
   if ((text = lmTextGet s) != NULL)                                       \
   {                                                                       \
      lmOut(text, 'W', __FILE__, __LINE__, (char*) __FUNCTION__, 0, NULL); \
      ::free(text);                                                          \
   }                                                                       \
} while (0)
#endif



#ifdef LM_NO_E
#define LM_E(s)
#else
/* ****************************************************************************
*
* LM_E - log error message
*/
#define LM_E(s)                                                            \
do {                                                                       \
   char* text;                                                             \
                                                                           \
   if ((text = lmTextGet s) != NULL)                                       \
   {                                                                       \
      lmOut(text, 'E', __FILE__, __LINE__, (char*) __FUNCTION__, 0, NULL); \
      ::free(text);                                                          \
   }                                                                       \
} while (0)
#endif

#ifdef LM_NO_P
#define LM_P(s)
#else
/* ****************************************************************************
*
* LM_P - log perror message
*/
#define LM_P(s)                                                            \
do {                                                                       \
   char* text;                                                             \
   char  stre[128];                                                        \
                                                                           \
   strcpy(stre, strerror(errno));                                          \
   if ((text = lmTextGet s) != NULL)                                       \
   {                                                                       \
      lmOut(text, 'P', __FILE__, __LINE__, (char*) __FUNCTION__, 0, stre); \
      ::free(text);                                                          \
   }                                                                       \
} while (0)
#endif


/* LMS - signal handler-safe log message functions */

#ifdef LM_NO_V
#define LMS_V(s)
#else
/* ****************************************************************************
*
* LMS_V - log verbose message
*/
#define LMS_V(s)                                                                    \
do {                                                                                \
   char* text;                                                                      \
                                                                                    \
   if (lmOk('V', 0) == LmsOk)                                                       \
   {                                                                                \
      if ((text = lmTextGet s) != NULL)                                             \
      {                                                                             \
         lmAddMsgBuf(text, 'V', __FILE__, __LINE__, (char*) __FUNCTION__, 0, NULL); \
         ::free(text);                                                                \
      }                                                                             \
   }                                                                                \
} while (0)
#endif


#ifdef LM_NO_M
#define LMS_M(s)
#else
/* ****************************************************************************
*
* LMS_M - log message
*/
#define LMS_M(s)                                                                 \
do {                                                                             \
   char* text;                                                                   \
                                                                                 \
   if ((text = lmTextGet s) != NULL)                                             \
   {                                                                             \
      lmAddMsgBuf(text, 'M', __FILE__, __LINE__, (char*) __FUNCTION__, 0, NULL); \
      ::free(text);                                                                \
   }                                                                             \
} while (0)
#endif


#ifdef LM_NO_F
#define LMS_F(s)
#else
/* ****************************************************************************
*
* LMS_F - log message
*/
#define LMS_F(s)                                                        \
do {                                                                    \
   char* text;                                                          \
                                                                        \
   if ((text = lmTextGet s) != NULL)                                    \
   {                                                                    \
      lmAddMsgBuf(text, 'F', __FILE__, __LINE__, "FFF", 0, NULL);       \
      ::free(text);                                                       \
   }                                                                    \
} while (0)
#endif


#ifdef LM_NO_W
#define LMS_W(s)
#else
/* ****************************************************************************
*
* LMS_W - log warning message
*/
#define LMS_W(s)                                                                 \
do {                                                                             \
   char* text;                                                                   \
                                                                                 \
   if ((text = lmTextGet s) != NULL)                                             \
   {                                                                             \
      lmAddMsgBuf(text, 'W', __FILE__, __LINE__, (char*) __FUNCTION__, 0, NULL); \
      ::free(text);                                                                \
   }                                                                             \
} while (0)
#endif



#ifdef LM_NO_E
#define LMS_E(s)
#else
/* ****************************************************************************
*
* LMS_E - log error message
*/
#define LMS_E(s)                                                                 \
do {                                                                             \
   char* text;                                                                   \
                                                                                 \
   if ((text = lmTextGet s) != NULL)                                             \
   {                                                                             \
      lmAddMsgBuf(text, 'E', __FILE__, __LINE__, (char*) __FUNCTION__, 0, NULL); \
      ::free(text);                                                                \
   }                                                                             \
} while (0)
#endif

#ifdef LM_NO_P
#define LMS_P(s)
#else
/* ****************************************************************************
*
* LMS_P - log perror message
*/
#define LMS_P(s)                                                                 \
do {                                                                             \
   char* text;                                                                   \
   char  stre[128];                                                              \
                                                                                 \
   strcpy(stre, strerror(errno));                                                \
   if ((text = lmTextGet s) != NULL)                                             \
   {                                                                             \
      lmAddMsgBuf(text, 'P', __FILE__, __LINE__, (char*) __FUNCTION__, 0, stre); \
      ::free(text);                                                                \
   }                                                                             \
} while (0)
#endif


/* ****************************************************************************
*
* LMS_T - log trace message
*/
#define LMS_T(tLev, s)                                                                 \
do {                                                                                   \
   char* text;                                                                         \
                                                                                       \
   if (lmOk('T', tLev) == LmsOk)                                                       \
   {                                                                                   \
      if ((text = lmTextGet s) != NULL)                                                \
      {                                                                                \
         lmAddMsgBuf(text, 'T', __FILE__, __LINE__, (char*) __FUNCTION__, tLev, NULL); \
         ::free(text);                                                                   \
      }                                                                                \
   }                                                                                   \
} while (0)


/* ****************************************************************************
*
* LM_RE - log error message and return with code
*/
#define LM_RE(code, s)           \
do {                             \
   LM_E(s);                      \
   if (1 == 1) return code;      \
} while (0)



/* ****************************************************************************
*
* LM_RP - log perror message and return with code
*/
#define LM_RP(code, s)           \
do {                             \
   LM_P(s);                      \
   if (1 == 1) return code;      \
} while (0)



/* ****************************************************************************
*
* LM_RVE - log error message and return with void
*/
#define LM_RVE(s)           \
do {                        \
   LM_E(s);                 \
   if (1 == 1) return;      \
} while (0)



/* ****************************************************************************
*
* LM_RVP - log perror message and return with void
*/
#define LM_RVP(s)           \
do {                        \
   LM_P(s);                 \
   if (1 == 1) return;      \
} while (0)



#ifdef LM_NO_X
#define LM_X(c, s)  exit(c)
#else
/* ****************************************************************************
*
* LM_X - log error message and exit with code
*/
#define LM_X(c, s)                                                         \
do {                                                                       \
   char* text;                                                             \
                                                                           \
   if ((text = lmTextGet s) != NULL)                                       \
   {                                                                       \
      lmOut(text, 'X', __FILE__, __LINE__, (char*) __FUNCTION__, c, NULL); \
      ::free(text);                                                          \
   }                                                                       \
} while (0)
#endif


#ifdef LM_NO_XP
#define LM_XP(c, s)  exit(c)
#else
/* ****************************************************************************
*
* LM_XP - log perror message and exit with code
*/
#define LM_XP(c, s)                                                        \
do {                                                                       \
   char* text;                                                             \
   char  stre[128];                                                        \
                                                                           \
   strcpy(stre, strerror(errno));                                          \
   if ((text = lmTextGet s) != NULL)                                       \
   {                                                                       \
      lmOut(text, 'x', __FILE__, __LINE__, __FUNCTION__, c, stre);	       \
      ::free(text);                                                          \
   }                                                                       \
} while (0)
#endif


#ifndef LM_ON
#  define LM_TODO(s)
#  define LM_DOUBT(s)
#  define LM_FIX(s)
#  define LM_T(tLev, s)
#  define LM_D(s)
#  define LM_RAW(s)
#  define LM_IN(s)
#  define LM_FROM(s)
#  define LM_LINE()
#  define LM_MLINE()
#  define LM_ENTRY()
#  define LM_EXIT()
#  define LM_BUG(s)
#  define LM_READS(_to, _desc, _buf, _sz, _form)
#  define LM_WRITES(_to, _desc, _buf, _sz, _form)
#  define LM_BUF(_desc, _buf, _sz, _form)
#else



/* ****************************************************************************
*
* LM_TODO - log todo message
*/
#define LM_TODO(s)                                                            \
do {                                                                          \
   char* text;                                                                \
                                                                              \
   if (lmOk('t', 0) == LmsOk)                                                 \
   {                                                                          \
      if ((text = lmTextGet s) != NULL)                                       \
      {                                                                       \
         lmOut(text, 't', __FILE__, __LINE__, (char*) __FUNCTION__, 0, NULL); \
         ::free(text);                                                          \
      }                                                                       \
   }                                                                          \
} while (0)



/* ****************************************************************************
*
* LM_DOUBT - log doubt message
*/
#define LM_DOUBT(s)                                                        \
do {                                                                       \
   char* text;                                                             \
                                                                           \
   if ((text = lmTextGet s) != NULL)                                       \
   {                                                                       \
      lmOut(text, 'd', __FILE__, __LINE__, (char*) __FUNCTION__, 0, NULL); \
      ::free(text);                                                          \
   }                                                                       \
} while (0)



/* ****************************************************************************
*
* LM_FIX - log fix message
*/
#define LM_FIX(s)                                                          \
do {                                                                       \
   char* text;                                                             \
                                                                           \
   if ((text = lmTextGet s) != NULL)                                       \
   {                                                                       \
      lmOut(text, 'F', __FILE__, __LINE__, (char*) __FUNCTION__, 0, NULL); \
      ::free(text);                                                          \
   }                                                                       \
} while (0)



/* ****************************************************************************
*
* LM_BUG - log bug message
*/
#define LM_BUG(s)                                                          \
do {                                                                       \
   char* text;                                                             \
                                                                           \
   if ((text = lmTextGet s) != NULL)                                       \
   {                                                                       \
      lmOut(text, 'B', __FILE__, __LINE__, (char*) __FUNCTION__, 0, NULL); \
      ::free(text);                                                          \
   }                                                                       \
} while (0)



/* ****************************************************************************
*
* LM_T - log trace message
*/
#define LM_T(tLev, s)                                                            \
do {                                                                             \
   char* text;                                                                   \
                                                                                 \
   if (lmOk('T', tLev) == LmsOk)                                                 \
   {                                                                             \
      if ((text = lmTextGet s) != NULL)                                          \
      {                                                                          \
         lmOut(text, 'T', __FILE__, __LINE__, (char*) __FUNCTION__, tLev, NULL); \
         ::free(text);                                                             \
      }                                                                          \
   }                                                                             \
} while (0)



/* ****************************************************************************
*
* LM_D - log debug message
*/
#define LM_D(s)                                                               \
do {                                                                          \
   char* text;                                                                \
                                                                              \
   if (lmOk('D', 0) == LmsOk)                                                 \
   {                                                                          \
      if ((text = lmTextGet s) != NULL)                                       \
      {                                                                       \
         lmOut(text, 'D', __FILE__, __LINE__, (char*) __FUNCTION__, 0, NULL); \
         ::free(text);                                                          \
      }                                                                       \
   }                                                                          \
} while (0)



/* ****************************************************************************
*
* LM_RAW - log raw message
*/
#define LM_RAW(s)                                                       \
do {                                                                    \
   char* text;                                                          \
                                                                        \
   if ((text = lmTextGet s) != NULL)                                    \
   {                                                                    \
      lmOut(text, 'R', __FILE__, __LINE__, NULL, 0, NULL);              \
      ::free(text);                                                       \
   }                                                                    \
} while (0)



/* ****************************************************************************
*
* LM_LINE - line number trace macro
*/
#define LM_LINE() \
   LM_T(LmtLine, ("line %d", __LINE__))


/* ****************************************************************************
*
* LM_MLINE - line number trace macro
*/
#define LM_MLINE() \
   LM_M(("line %d", __LINE__))


/* ****************************************************************************
*
* LM_IN - in function trace macro
*/
#define LM_IN(s) \
   LM_T(LmtIn, s)



/* ****************************************************************************
*
* LM_FROM - from function trace macro
*/
#define LM_FROM(s) \
   LM_T(LmtFrom, s)



/* ****************************************************************************
*
* LM_ENTRY - in function trace macro
*/
#define LM_ENTRY() \
   LM_T(LmtEntry, ("Entering function %s", (char*) __FUNCTION__))



/* ****************************************************************************
*
* LM_EXIT - exit function trace macro
*/
#define LM_EXIT() \
   LM_T(LmtExit, ("Leaving function %s", (char*) __FUNCTION__))



/* ****************************************************************************
*
* LM_READS - read buffer presentation
*/
#define LM_READS(_from, _desc, _buf, _sz, _form)  \
   lmBufferPresent((char*) _from, (char*) _desc, _buf, _sz, _form, 'r')




/* ****************************************************************************
*
* LM_BUF - buffer presentation
*/
#define LM_BUF(_desc, _buf, _sz, _form)  \
   lmBufferPresent(NULL, _desc, _buf, _sz, _form, 'b')




/* ****************************************************************************
*
* LM_WRITES - written buffer presentation
*/
#define LM_WRITES(_to, _desc, _buf, _sz, _form)  \
   lmBufferPresent((char*) _to, (char*) _desc, (char*) _buf, _sz, _form, 'w')

#endif



/* ****************************************************************************
*
* LMX_E - 
*/
#define LMX_E(xCode, s)                                                    \
do {                                                                       \
   char* text;                                                             \
                                                                           \
   if ((text = lmTextGet s) != NULL)                                       \
   {                                                                       \
      lmOut(text, 'E', __FILE__, __LINE__, (char*) __FUNCTION__, 0, NULL); \
      if (lmxFp != NULL) lmxFp(xCode, text);                               \
      ::free(text);                                                          \
   }                                                                       \
} while (0)



/* ****************************************************************************
*
* LMX_RE - 
*/
#define LMX_RE(xCode, rCode, s)                                            \
do {                                                                       \
   char* text;                                                             \
                                                                           \
   if ((text = lmTextGet s) != NULL)                                       \
   {                                                                       \
      lmOut(text, 'E', __FILE__, __LINE__, (char*) __FUNCTION__, 0, NULL); \
      if (lmxFp != NULL) lmxFp(xCode, text);                               \
      ::free(text);                                                          \
   }                                                                       \
   if (1 == 1) return rCode;                                               \
} while (0)



/* ****************************************************************************
*
* LMX_X - log error message and exit with code
*/
#define LMX_X(xCode, eCode, s)                                                 \
do {                                                                           \
   char* text;                                                                 \
                                                                               \
   if ((text = lmTextGet s) != NULL)                                           \
   {                                                                           \
      lmOut(text, 'X', __FILE__, __LINE__, (char*) __FUNCTION__, eCode, NULL); \
      if (lmxFp != NULL) lmxFp(xCode, text);                                   \
      ::free(text);                                                              \
   }                                                                           \
} while (0)



/* ****************************************************************************
*
* LMX_W - log warning message
*/
#define LMX_W(xCode, s)                                                    \
do {                                                                       \
   char* text;                                                             \
                                                                           \
   if ((text = lmTextGet s) != NULL)                                       \
   {                                                                       \
      lmOut(text, 'W', __FILE__, __LINE__, (char*) __FUNCTION__, 0, NULL); \
      if (lmxFp != NULL) lmxFp(xCode, text);                               \
      ::free(text);                                                          \
   }                                                                       \
} while (0)



/* ****************************************************************************
*
* LMX_M - log message
*/
#define LMX_M(xCode, s)                                                    \
do {                                                                       \
   char* text;                                                             \
                                                                           \
   if ((text = lmTextGet s) != NULL)                                       \
   {                                                                       \
      lmOut(text, 'M', __FILE__, __LINE__, (char*) __FUNCTION__, 0, NULL); \
      if (lmxFp != NULL) lmxFp(xCode, text);                               \
      ::free(text);                                                          \
   }                                                                       \
} while (0)




/* ****************************************************************************
*
* LMX_V - log verbose message
*/
#define LMX_V(xCode, s)                                                       \
do {                                                                          \
   char* text;                                                                \
                                                                              \
   if (lmOk('V', 0) == LmsOk)                                                 \
   {                                                                          \
      if ((text = lmTextGet s) != NULL)                                       \
      {                                                                       \
         lmOut(text, 'V', __FILE__, __LINE__, (char*) __FUNCTION__, 0, NULL); \
         if (lmxFp != NULL) lmxFp(xCode, text);                               \
         ::free(text);                                                          \
      }                                                                       \
   }                                                                          \
} while (0)



/* ****************************************************************************
*
* LMX_D - log message
*/
#define LMX_D(xCode, s)                                                       \
do {                                                                          \
   char* text;                                                                \
                                                                              \
   if (lmOk('D', 0) == LmsOk)                                                 \
   {                                                                          \
      if ((text = lmTextGet s) != NULL)                                       \
      {                                                                       \
         lmOut(text, 'D', __FILE__, __LINE__, (char*) __FUNCTION__, 0, NULL); \
         if (lmxFp != NULL) lmxFp(xCode, text);                               \
         ::free(text);                                                          \
      }                                                                       \
   }                                                                          \
} while (0)



/* ****************************************************************************
*
* LMX_T - log trace message
*/
#define LMX_T(xCode, tLev, s)                                                    \
do {                                                                             \
   char* text;                                                                   \
                                                                                 \
   if (lmOk('T', tLev) == LmsOk)                                                 \
   {                                                                             \
      if ((text = lmTextGet s) != NULL)                                          \
      {                                                                          \
         lmOut(text, 'T', __FILE__, __LINE__, (char*) __FUNCTION__, tLev, NULL); \
         if (lmxFp != NULL) lmxFp(xCode, text);                                  \
         ::free(text);                                                             \
      }                                                                          \
   }                                                                             \
} while (0)



/* ****************************************************************************
*
* Output decision variables
*/
extern int  lmBug2;
extern bool lmBug;
extern bool lmVerbose;
extern bool lmVerbose2;
extern bool lmVerbose3;
extern bool lmVerbose4;
extern bool lmVerbose5;
extern bool lmDebug;
extern bool lmHidden;
extern bool lmToDo;
extern bool lmFix;
extern bool lmDoubt;
extern bool lmBuf;
extern bool lmReads;
extern bool lmWrites;
extern bool lmAssertAtExit;



/* ****************************************************************************
*
* lmxFp - 
*/
extern LmxFp lmxFp;



/* ****************************************************************************
*
* lmInit - 
*/
extern LmStatus lmInit(void);



/* ****************************************************************************
*
* lmInitX - 
*/
extern LmStatus lmInitX(char* progName, char* tLevel, int* i1P, int* i2P);



/* ****************************************************************************
*
* lmStrerror - 
*/
extern const char* lmStrerror(LmStatus s);



/* ****************************************************************************
*
* lmProgName - 
*/
extern char* lmProgName(char* pn, int levels, bool pid, const char* extra = NULL);



/* ****************************************************************************
*
* lmTraceSet - 
*/
extern LmStatus lmTraceSet(const char* levelFormat);



/* ****************************************************************************
*
* lmTraceAdd - 
*/
extern LmStatus lmTraceAdd(const char* levelFormat);



/* ****************************************************************************
*
* lmTraceSub - 
*/
extern LmStatus lmTraceSub(const char* levelFormat);



/* ****************************************************************************
*
* lmTraceGet - 
*/
extern char* lmTraceGet(char* levelString);
extern char* lmTraceGet(char* levelString, int levelStringSize, char* traceV);



/* ****************************************************************************
*
* lmFormat - 
*/
extern LmStatus lmFormat(int index, char* f);



/* ****************************************************************************
*
* lmTimeFormat - 
*/
extern LmStatus lmTimeFormat(int index, char* f);



/* ****************************************************************************
*
* lmGetInfo - 
*/
LmStatus lmGetInfo(int index, char* info);



/* ****************************************************************************
*
* lmFdGet - 
*/
extern LmStatus lmFdGet(int index, int* iP);



/* ****************************************************************************
*
* lmTraceAtEnd - 
*/
extern LmStatus lmTraceAtEnd(int index, char* start, char* end);



/* ****************************************************************************
*
* lmAux - 
*/
extern LmStatus lmAux(char* a);



/* ****************************************************************************
*
* lmTextGet - 
*/
extern char* lmTextGet(const char* format, ...);



/* ****************************************************************************
*
* lmOk - check whether or not to present the line 
*/
extern LmStatus lmOk(char type, int tLev);



/* ****************************************************************************
*
* lmFdRegister - 
*/
extern LmStatus lmFdRegister
(
	int          fd,
	const char*  format,
	const char*  timeFormat,
	const char*  info,
	int*         indexP
);



/* ****************************************************************************
*
* lmFdUnregister
*/
extern void lmFdUnregister(int fd);



/* ****************************************************************************
*
* lmPathRegister - 
*/
extern LmStatus lmPathRegister
(
   const char* path,
   const char* format,
   const char* timeFormat,
   int*        indexP
);



/* ****************************************************************************
*
* lmOut - 
*/
extern LmStatus lmOut
(
   char*        text,
   char         type,
   const char*  file,
   int          lineNo,
   const char*  fName,
   int          tLev,
   const char*  stre
);



/* ****************************************************************************
*
* lmOutHookSet - 
*/
extern void lmOutHookSet(LmOutHook hook, void* vP);
extern bool lmOutHookInhibit();
extern void lmOutHookRestore(bool onoff);



/* ****************************************************************************
*
* lmExitFunction - 
*/
LmStatus lmExitFunction(LmExitFp fp, void* input);



/* ****************************************************************************
*
* lmErrorFunction - 
*/
extern LmStatus lmErrorFunction(LmErrorFp fp, void* input);



/* ****************************************************************************
*
* lmWarningFunction - 
*/
extern LmStatus lmWarningFunction(LmWarningFp fp, void* input);



/******************************************************************************
*
* lmBufferPresent - 
*
* lmBufferPresent presents a buffer in a nice hexa decimal format.
*/
int lmBufferPresent
(
   char*       to,
   char*       description,
   void*       bufP,
   int         size, 
   LmFormat    format,
   int         type
);



/* ****************************************************************************
*
* lmWriteFunction - use alternative write function
*/
LmStatus lmWriteFunction(int i, LmWriteFp fp);



/* ****************************************************************************
*
* lmClear - 
*/
extern LmStatus lmClear(int index, int keepLines, int lastLines);



/* ****************************************************************************
*
* lmDoClear - 
*/
extern LmStatus lmDoClear(void);



/* ****************************************************************************
*
* lmDontClear - 
*/
extern LmStatus lmDontClear(void);



/* ****************************************************************************
*
* lmClearAt - 
*/
LmStatus lmClearAt(int atLines, int keepLines, int lastLines);



/* ****************************************************************************
*
* lmClearGet - 
*/
extern void lmClearGet
(
   bool* clearOn,
   int*       atP,
   int*       keepP,
   int*       lastP,
   int*       logFileBytesP
);



/* ****************************************************************************
*
* lmExitForced - 
*/
extern void lmExitForced(int code);



/* ****************************************************************************
*
* lmxFunction - extra function to call for LMX_ macros
*/
extern LmStatus lmxFunction(LmxFp fp);



/* ****************************************************************************
*
* lmReopen - 
*/
extern LmStatus lmReopen(int index);



/* ****************************************************************************
*
* lmOnlyErrors
*/
extern LmStatus lmOnlyErrors(int index);



/* ****************************************************************************
*
* lmTraceLevel - 
*/
extern const char* lmTraceLevel(int level);



/* ****************************************************************************
*
* lmTraceNameCbSet - 
*/
extern void lmTraceNameCbSet(LmTracelevelName cb);



/* ****************************************************************************
*
* lmTraceIsSet - 
*/
extern bool lmTraceIsSet(int level);



/* ****************************************************************************
*
* lmTraceLevelSet - set a particular level to true/false
*/
extern void lmTraceLevelSet(unsigned int level, bool onOff);



/* ****************************************************************************
*
* lmSdGet - 
*/
int lmSdGet(void);



/* ****************************************************************************
*
* lmAddMsgBuf - 
*/
void lmAddMsgBuf(char* text, char type, const char* file, int line, const char* func, int tLev, const char *stre);



/* ****************************************************************************
*
* lmPrintMsgBuf - 
*/
void lmPrintMsgBuf();


/* ****************************************************************************
*
* lmWarningFunctionDebug
*/
extern void lmWarningFunctionDebug(char* info, char* file, int line);
#define LMWARNINGFUNCTIONDEBUG(info) lmWarningFunctionDebug(info, __FILE__, __LINE__)



/* ****************************************************************************
*
* lmFirstDiskFileDescriptor - 
*/
extern int lmFirstDiskFileDescriptor(void);



/* ****************************************************************************
*
* lmLogLineGet - 
*/
extern long lmLogLineGet(char* typeP, char* dateP, int* msP, char* progNameP, char* fileNameP, int* lineNoP, int* pidP, int* tidP, char* funcNameP, char* messageP, long offset, char** lineP);

#endif
