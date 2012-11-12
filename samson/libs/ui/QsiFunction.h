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



class Block;
class Box;



/* ****************************************************************************
*
* Function - 
*/
typedef void (*Function)(Block* qbP, void* param);
typedef void (*MenuFunction)(Block* qbP, void* param, const char* selection);
typedef void (*ModalFunction)(Box* boxP, void* param);

}

#endif
