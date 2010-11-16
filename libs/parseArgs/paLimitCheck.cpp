/* ****************************************************************************
*
* FILE			paLimitCheck.c -  check option values are within limits
*
* AUTHOR		Ken Zangelin
*
* HISTORY
* $Log: paArgs.c,v $
*
*/
#include "logMsg.h"             /* lmVerbose, lmDebug, ...                   */

#include "parseArgs.h"          /* Own interface                             */
#include "paPrivate.h"          /* PaTypeUnion, config variables, ...        */
#include "paTraceLevels.h"      /* LmtPaEnvVal, ...                          */
#include "paWarning.h"          /* paWaringInit, paWarningAdd                */
#include "paFrom.h"             /* paFrom                                    */
#include "paIterate.h"          /* paIterateInit, paIterateNext              */



/* ****************************************************************************
*
* getSpaces - 
*/
static char* getSpaces(int spaces, char* s)
{
	int ix = 0;

	while (--spaces >= 0)
		s[ix++] = ' ';
	s[ix] = 0;

	return s;
}



/* ****************************************************************************
*
* limits - check limits for an option
*/
static int limits(PaArgument* paList, PaArgument* aP, char* input)
{
	char            valS[80];
	bool            upper = true;
	bool            lower = true;
	char            w[512];
	int             iVal;
	short           sVal;
	char            cVal;
	float           fVal;
	double          dVal;
	unsigned int    uiVal;
	unsigned short  usVal;
	unsigned char   ucVal;

	if ((aP->type == PaSList) || (aP->type == PaIList) || (aP->type == PaBoolean))
		return 0;

	if (aP->min == aP->max)
	{
		LM_T(LmtPaLimits, ("'%s': limits equal - no limit check", aP->name));
		lower = false;
		upper = false;
	}
	else if (aP->min == PaNoLim)
	{
		LM_T(LmtPaLimits, ("'%s': no lower limit check", aP->name));
		lower = false;
	}
	else if (aP->max == PaNoLim)
	{
		LM_T(LmtPaLimits, ("'%s': no upper limit check", aP->name));
		upper = false;
	}

	if ((upper == false) && (lower == false))
		return 0;

	LM_T(LmtPaLimits, ("limit check for %s", aP->name));

	w[0] = 0;
	iVal = *((int*)             aP->varP);
	sVal = *((short*)           aP->varP);
	cVal = *((char*)            aP->varP);
	fVal = *((float*)           aP->varP);
	dVal = *((double*)          aP->varP);
	uiVal = *((unsigned int*)   aP->varP);
	usVal = *((unsigned short*) aP->varP);
	ucVal = *((unsigned char*)  aP->varP);
	
	switch (aP->type)
	{
	case PaString:
		if (lower && (strcmp((char*) aP->varP, (char*) aP->min) < 0))
		{
			LM_E(("low limit error for %s (strcmp(\"%s\", \"%s\")')", aP->name,
				  aP->varP, aP->min));
			sprintf(valS, "%s", (char*) aP->varP);
			sprintf(w, "value(\"%s\") < minimum(\"%s\") (%s)",
					(char*) aP->varP, 
					(char*) aP->min,
					aP->option? paFrom(paList, (char*) aP->option): "parameter");
		}
		else if (upper && (strcmp((char*) aP->varP, (char*) aP->max) > 0))
		{
			LM_E(("high limit error for %s", aP->name));
			sprintf(valS, "%s", (char*) aP->varP);
			sprintf(w, "value(\"%s\") > maximum(\"%s\") (%s)",
					(char*) aP->varP, 
					(char*) aP->max,
					aP->option? paFrom(paList, (char*) aP->option) : "parameter");
		}
		break;

	case PaInt:
		LM_T(LmtPaLimits, ("checking '%s' (value %d): limits '%d' - '%d'",
						   aP->name, iVal, aP->min, aP->max));
		if ((lower && (iVal < (int) aP->min))
		||  (upper && (iVal > (int) aP->max)))
		{
			sprintf(valS, "%d", iVal);
			sprintf(w, "%ld <= %d <= %ld (%s)",
					aP->min,
					iVal,
					aP->max,
					aP->option? paFrom(paList, (char*) aP->option) : "parameter");
		}
		break;
		
	case PaIntU:
		LM_T(LmtPaLimits, ("checking '%s' (value %d): limits '%d' - '%d'",
						   aP->name, uiVal, aP->min, aP->max));
		if ((lower && (uiVal < (unsigned int) aP->min))
		||  (upper && (uiVal > (unsigned int) aP->max)))
		{
			sprintf(valS, "%u", uiVal);
			sprintf(w, "%lu <= %u <= %lu (%s)",
					aP->min,
					uiVal,
					aP->max,
					aP->option? paFrom(paList, (char*) aP->option) : "parameter");
		}
		break;
		
	case PaShort:
		LM_T(LmtPaLimits, ("checking '%s' (value %d): limits '%d' - '%d'",
						   aP->name, sVal, aP->min, aP->max));
		if ((lower && (sVal < (short) aP->min))
		||  (upper && (sVal > (short) aP->max)))
		{
			sprintf(valS, "%d", sVal);
			sprintf(w, "%ld <= %d <= %ld (%s)",
					aP->min,
					sVal,
					aP->max,
					aP->option? paFrom(paList, (char*) aP->option) : "parameter");
		}
		break;
		
	case PaShortU:
		LM_T(LmtPaLimits, ("checking '%s' (value %d): limits '%d' - '%d'",
						   aP->name, usVal, aP->min, aP->max));
		if ((lower && (usVal < (unsigned short) aP->min))
		||  (upper && (usVal > (unsigned short) aP->max)))
		{
			sprintf(valS, "%u", usVal);
			sprintf(w, "%lu <= %u <= %lu (%s)",
					aP->min,
					usVal,
					aP->max,
					aP->option? paFrom(paList, (char*) aP->option) : "parameter");
		}
		break;
		
	case PaChar:
		LM_T(LmtPaLimits, ("checking '%s' (value %d): limits '%d' - '%d'",
						   aP->name, cVal, aP->min, aP->max));
		if ((lower && (cVal < (char) aP->min))
		||  (upper && (cVal > (char) aP->max)))
		{
			sprintf(valS, "%d", cVal);
			sprintf(w, "%ld <= %d <= %ld (%s)",
					aP->min,
					cVal,
					aP->max,
					aP->option? paFrom(paList, (char*) aP->option) : "parameter");
		}
		break;
		
	case PaCharU:
		LM_T(LmtPaLimits, ("checking '%s' (value %d): limits '%d' - '%d'",
						   aP->name, ucVal, aP->min, aP->max));
		if ((lower && (ucVal < (unsigned char) aP->min))
		||  (upper && (ucVal > (unsigned char) aP->max)))
		{
			sprintf(valS, "%u", ucVal);
			sprintf(w, "%lu <= %u <= %lu (%s)",
					aP->min,
					ucVal,
					aP->max,
					aP->option? paFrom(paList, (char*) aP->option) : "parameter");
		}
		break;
		
	case PaFloat:
		LM_T(LmtPaLimits, ("checking '%s' (value %d): limits '%f' - '%f'",
						   aP->name, fVal, aP->min, aP->max));
		if ((lower && (fVal < (float) aP->min))
		||  (upper && (fVal > (float) aP->max)))
		{
			sprintf(valS, "%f", fVal);
			sprintf(w, "%ld.0 <= %f <= %ld.0 (%s)",
				aP->min,
				fVal,
				aP->max,
					aP->option? paFrom(paList, (char*) aP->option) : "parameter");
		}
		break;
		
	case PaDouble:
		LM_T(LmtPaLimits, ("checking '%s' (value %d): limits '%f' - '%f'",
						   aP->name, dVal, aP->min, aP->max));
		if ((lower && (dVal < (double) aP->min))
		||  (upper && (dVal > (double) aP->max)))
		{
			sprintf(valS, "%f", dVal);
			sprintf(w, "%ld.0 <= %f <= %ld.0 (%s)",
					aP->min,
					dVal,
					aP->max,
					aP->option? paFrom(paList, (char*) aP->option) : "parameter");
		}
		break;
		
	default:
		sprintf(w, "type %d unknown for %s", aP->type, aP->name);
		PA_WARNING(PasNoSuchOptType, w);
		return 0;
	}

	if (w[0] != 0)
	{
		char  spaces[80];
		int   len = strlen(progName) + 2;
		char  eString[1024];

		sprintf(eString, "%s: value %s for %s not within limits\n%s%s",
				progName,
				valS,
				aP->name,
				getSpaces(len, spaces),
				w);
		PA_WARNING(PasLimitError, eString);
	}
	
	return 0;
}



/* ****************************************************************************
*
* paLimitCheck - check limits for all options
*/
int paLimitCheck(PaArgument* paList)
{
	PaArgument* aP;

	LM_ENTRY();

	paIterateInit();
	while ((aP = paIterateNext(paList)) != NULL)
	{
		if (limits(paList, aP, NULL) == -1)
		{
			LM_EXIT();
			return -1;
		}
	}

	LM_EXIT();
	return 0;
}



