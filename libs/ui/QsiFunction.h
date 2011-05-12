#ifndef QSI_FUNCTION_H
#define QSI_FUNCTION_H

/* ****************************************************************************
*
* FILE                     QsiFunction.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 10 2011
*
*/



namespace Qsi
{



class QsiBlock;



/* ****************************************************************************
*
* QsiFunction - 
*/
typedef void (*QsiFunction)(QsiBlock* siP, void* param);

}

#endif
