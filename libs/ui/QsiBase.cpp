/* ****************************************************************************
*
* FILE                     QsiBase.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 15 2011
*
*/
#include <stdlib.h>             // free

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Lmt*

#include "QsiBase.h"            // Own interface



namespace Qsi
{



/* ****************************************************************************
*
* QsiBase - 
*/
QsiBase::QsiBase(QsiBox* _owner, QsiType _type, const char* _name, int _x, int _y, int _width, int _height)
{
	owner    = _owner;
	type     = _type;
	name     = strdup(_name);
	xInitial = _x;
	yInitial = _y;
	x        = 0;
	y        = 0;
	width    = _width;
	height   = _height;
}



/* ****************************************************************************
*
* ~QsiBase - 
*/
QsiBase::~QsiBase()
{
	if (name)
	{
		LM_T(LmtRemove, ("destroying QsiBase '%s'", name));
		free(name);
	}
	else
		LM_T(LmtRemove, ("destroying a nameless QsiBase object"));
}



/* ****************************************************************************
*
* getOwner - 
*/
QsiBox* QsiBase::getOwner(void)
{
	return owner;
}



/* ****************************************************************************
*
* typeName - 
*/
const char* QsiBase::typeName(void)
{
	switch (type)
	{
	case Box:              return "Box";
	case SimpleText:       return "SimpleText";
	case Line:             return "Line";
	case Image:            return "Image";
	case Label:            return "Label";
	case Button:           return "Button";
	case Input:            return "Input";
	}

	return "Unknown Type";
}



/* ****************************************************************************
*
* xGet - 
*/
int QsiBase::xGet(void)
{
	return x;
}



/* ****************************************************************************
*
* yGet - 
*/
int QsiBase::yGet(void)
{
	return y;
}

}
