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
