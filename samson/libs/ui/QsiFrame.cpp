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
* FILE                     QsiFrame.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 16 2011
*
*/
#include "logMsg/logMsg.h"             // LM_*
#include "traceLevels.h"        // Lmt*

#include "QsiBox.h"             // Box
#include "QsiManager.h"         // Manager
#include "QsiFrame.h"           // Own interface


namespace Qsi
{



/* ****************************************************************************
*
* Frame - 
*/
Frame::Frame(Box* _box, int _padding)
{
	box        = _box;
	padding    = _padding;

	north      = NULL;
	west       = NULL;
	south      = NULL;
	east       = NULL;

	paint();
}



/* ****************************************************************************
*
* ~Frame - 
*/ 
Frame::~Frame()
{
	if (north)      delete north;
	if (south)      delete south;
	if (east)       delete east;
	if (west)       delete west;

	north = NULL;
	south = NULL;
	east  = NULL;
	west  = NULL;
}



/* ****************************************************************************
*
* paint - 
*/
void Frame::paint(void)
{
	int bx, by, bwidth, bheight;

	box->geometry(&bx, &by, &bwidth, &bheight);
	LM_T(LmtFrame, ("Got geometry of box '%s': { %d, %d } %dx%d", box->name, bx, by, bwidth, bheight));

	if (north == NULL)      north  = box->managerGet()->addLine(bx - padding,          by,                     bx + bwidth + padding, by);
	if (south == NULL)      south  = box->managerGet()->addLine(bx - padding,          by + bheight + padding, bx + bwidth + padding, by + bheight + padding);
	if (east  == NULL)      east   = box->managerGet()->addLine(bx + bwidth + padding, by,                     bx + bwidth + padding, by + bheight + padding);
	if (west  == NULL)      west   = box->managerGet()->addLine(bx - padding,          by,                     bx - padding,          by + bheight + padding);
}



void Frame::moveRelative(int x, int y)
{
	north->moveBy(x, y);
	south->moveBy(x, y);
	west->moveBy(x, y);
	east->moveBy(x, y);
}

void Frame::moveAbsolute(int x, int y)
{
	north->setPos(x, y);
	south->setPos(x, y);
	west->setPos(x, y);
	east->setPos(x, y);
}

void Frame::hide(void)
{
	north->setVisible(false);
	south->setVisible(false);
	west->setVisible(false);
	east->setVisible(false);
}

void Frame::show(void)
{
	north->setVisible(true);
	south->setVisible(true);
	west->setVisible(true);
	east->setVisible(true);
}
}
