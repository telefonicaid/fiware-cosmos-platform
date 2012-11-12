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
#ifndef _H_SAMSON_LOG
#define _H_SAMSON_LOG

namespace samson
{

	/* ****************************************************************************
	 *
	 * LogLineData
	 */

	typedef struct
	{
	  char  processName[64];
	  char  date[32];
	  char  text[256];
	  char  file[64];
	  char  fName[64];
	  char  stre[64];
	  char  type;
	  int   lineNo;
	  int  tLev;
	} LogLineData;

	typedef enum {
	LMT_User01 = 250,
	LMT_User02 = 251,
	LMT_User03 = 252,
	LMT_User04 = 253,
	LMT_User05 = 254,
	LMT_User06 = 255,
	} UserTraceLevels;
	
	
}

/* ****************************************************************************
 *
 * MACROS for logging
 */


#define OLM_TIME_FORMAT  "%A %d %h %H:%M:%S %Y"


#define OLM_G( _type,  _tLev, s)                                   \
do {                                                               \
LogLineData l;                                                     \
char* text = Tracer::print s;                                      \
strncpy( l.text , text , sizeof(l.text));                          \
free( text );                                                      \
                                                                   \
time_t t = time(NULL);                                             \
struct tm timeinfo;                                                \
localtime_r ( &t , &timeinfo );                                    \
strftime (l.date,sizeof(l.date),OLM_TIME_FORMAT,&timeinfo);        \
l.type = _type;                                                    \
l.tLev = _tLev;                                                    \
strncpy( l.file   , __FILE__ , sizeof(l.file));                    \
strncpy( l.fName ,  __FUNCTION__,sizeof(l.fName));                 \
l.lineNo = __LINE__;                                               \
l.stre[0] = 0;                                                     \
tracer->trace( &l );                                               \
} while (0)

#define OLM_T(_tLev, s) OLM_G('T' , _tLev , s )
#define OLM_D( s )      OLM_G('D' , 0     , s )
#define OLM_M( s )      OLM_G('M' , 0     , s )
#define OLM_W( s )      OLM_G('W' , 0     , s )
#define OLM_E( s )      OLM_G('E' , 0     , s )


#endif
