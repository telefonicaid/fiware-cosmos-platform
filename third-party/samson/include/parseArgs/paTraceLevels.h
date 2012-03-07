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
enum PaTraceLevels
{
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

#endif
