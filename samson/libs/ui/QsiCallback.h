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



class Block;



/* ****************************************************************************
*
* Callback
*/
typedef struct Callback
{
	Block*        qbP;
	MenuFunction  func;
	void*         param;
	bool          persistent;
} Callback;

}

#endif
