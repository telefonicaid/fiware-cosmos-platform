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

#include "logMsg/logMsg.h"      // LM_*
#include "traceLevels.h"        // Lmt*

#include "QsiBase.h"            // Own interface



namespace Qsi
{



/* ****************************************************************************
*
* Base - 
*/
Base::Base(Box* _owner, Type _type, const char* _name, int _x, int _y, int _width, int _height)
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

	isBox    = false;
	initialMoves = 0;
}



/* ****************************************************************************
*
* ~Base - 
*/
Base::~Base()
{
	if (name)
	{
		LM_T(LmtRemove, ("destroying Base '%s'", name));
		free(name);
	}
	else
		LM_T(LmtRemove, ("destroying a nameless Base object"));
}



/* ****************************************************************************
*
* getOwner - 
*/
Box* Base::getOwner(void)
{
	return owner;
}



/* ****************************************************************************
*
* typeName - 
*/
const char* Base::typeName(Type typ)
{
	switch (typ)
	{
	case Line:             return "Line";
	case Rectangle:        return "Rectangle";
	case SimpleText:       return "SimpleText";
	case Image:            return "Image";

	case Button:           return "Button";
	case Input:            return "Input";
	case Combo:            return "Combo";

	case BoxItem:          return "Box";
	case ExpandListItem:   return "ExpandList";
	case InputLineItem:    return "InputLine";
	case DialogItem:       return "Dialog";
	case InputDialogItem:  return "InputDialog";
	}

	return "Unknown Type";
}



/* ****************************************************************************
*
* typeName - 
*/
const char* Base::typeName(void)
{
	return typeName(type);
}



/* ****************************************************************************
*
* xGet - 
*/
int Base::xGet(void)
{
	return x;
}



/* ****************************************************************************
*
* xSet - 
*/
void Base::xSet(int _x)
{
	x = _x;
}



/* ****************************************************************************
*
* yGet - 
*/
int Base::yGet(void)
{
	return y;
}



/* ****************************************************************************
*
* ySet - 
*/
void Base::ySet(int _y)
{
	y = _y;
}



/* ****************************************************************************
*
* typeSet - 
*/
void Base::typeSet(Type _type)
{
	type = _type;
}



/* ****************************************************************************
*
* isAncestor - 
*/
bool Base::isAncestor(Box* qbP)
{
	if (owner == NULL)
		return false;
	if (qbP == owner)
		return true;

	return isAncestor(owner);
}

}
