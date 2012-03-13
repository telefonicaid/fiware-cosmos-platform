#ifndef PA_GET_VAL_H
#define PA_GET_VAL_H

/* ****************************************************************************
*
* FILE                  paGetVal.h
*
* AUTHOR                Ken Zangelin
*
*/



/* ****************************************************************************
*
* errors returned by function getVal
*/
typedef enum PaGetVal
{
	PaError = -1,
	PaOk    = 0,
	PaHex,
	PaOct,
	PaDec,
	PaDef,
	PaNotOk
} PaGetVal;



/* ****************************************************************************
*
* paGetVal - calculate the integer value of a string
*/
extern void* paGetVal(char* string, int* error);

#endif
