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
typedef void (*ModalFunction)(Box* boxP, void* param);

}

#endif
