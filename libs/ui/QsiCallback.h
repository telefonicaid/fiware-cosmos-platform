#ifndef QSI_CALLBACK_H
#define QSI_CALLBACK_H

/* ****************************************************************************
*
* FILE                     QsiCallback.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 10 2011
*
*/
#include "QsiFunction.h"



namespace Qsi
{



class QsiBlock;



/* ****************************************************************************
*
* QsiCallback
*/
typedef struct QsiCallback
{
	QsiBlock*    qbP;
	QsiFunction  func;
	void*        param;
	bool         persistent;
} QsiCallback;

}

#endif
