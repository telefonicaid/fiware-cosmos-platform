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
* FILE                     QsiExpandList.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 17 2011
*
*/
#include <QGraphicsSimpleTextItem>

#include "logMsg/logMsg.h"
#include "traceLevels.h"

#include "QsiManager.h"         // Manager
#include "QsiBase.h"            // Base
#include "QsiBox.h"             // Box
#include "QsiBlock.h"           // Block
#include "QsiExpandList.h"      // Own interface



namespace Qsi
{



/* ****************************************************************************
*
* titleClicked - 
*/
static void titleClicked(Block* qbP, void* param, const char* nada)
{
	LM_T(LmtMouse, ("Text '%s' Clicked. Param: %p", qbP->name, param));
	if (qbP->isExpanded())
		qbP->hideOthers();
	else
		qbP->showOthers();

	if (param != NULL)
		((Function) param)(qbP, (void*) "title");

	nada = NULL;
}



/* ****************************************************************************
*
* ExpandList - 
*/
ExpandList::ExpandList(Manager* manager, Box* owner, const char* _title, int x, int y, int _xmargin, int _ymargin, MenuFunction onClick, bool _frame) : Box(manager, owner, _title, x, y)
{
	this->typeSet(ExpandListItem);

	xmargin     = _xmargin;
	ymargin     = _ymargin;
	title       = (Block*) textAdd("title",    _title, 0, 0);
	memberBox   = (Box*)   boxAdd("memberBox", xmargin, ymargin);
	memberSpace = 15;

	title->setBold(true);
	manager->siConnect(title, titleClicked, (void*) onClick);
	owner->add(this);
	title->boxMoveSet(true);
}



/* ****************************************************************************
*
* ExpandList destructor - 
*/
ExpandList::~ExpandList()
{
	delete title;
	delete memberBox;
}



/* ****************************************************************************
*
* addMember - 
*/
Base* ExpandList::addMember(const char* string, MenuFunction callback, const void* dataP, const char* mVec[])
{
	Block*  lastMember;
	Block*  sMember;
	int     gx, gy, gwidth, gheight;

	lastMember = (Block*) memberBox->lastAddedGet();

	memberBox->geometry(&gx, &gy, &gwidth, &gheight);
	sMember = (Block*) memberBox->textAdd(string, string, 0, gheight + memberSpace);
	if (lastMember != NULL)
		sMember->align(Alignment::South, lastMember, memberSpace);
	else
		sMember->align(Alignment::South, firstLine, memberSpace);

	if (callback != NULL)
		manager->siConnect(sMember, callback, dataP);

	if (mVec != NULL)
	{
		for (int ix = 0; mVec[ix] != NULL; ix++)
			sMember->menuAdd(mVec[ix], callback, (void*) mVec[ix]);
	}

	((Block*) sMember)->setMovable(false);
	return sMember;
}



/* ****************************************************************************
*
* addMember - 
*/
Base* ExpandList::addMember(Base* _member, MenuFunction callback, const void* dataP, const char* mVec[])
{
	int gx, gy, gwidth, gheight;

	memberBox->geometry(&gx, &gy, &gwidth, &gheight);
	_member->xSet(0);
	_member->ySet(gheight + memberSpace);
	add(_member);
	
	if (callback != NULL)
		manager->siConnect((Block*) _member, callback, dataP);

	if (mVec != NULL)
	{
		for (int ix = 0; mVec[ix] != NULL; ix++)
			((Block*) _member)->menuAdd(mVec[ix], callback, (void*) mVec[ix]);
	}

	((Block*) _member)->setMovable(false);
	return _member;
}



/* ****************************************************************************
*
* expand - 
*/
void ExpandList::expand(void)
{
	memberBox->show();
}



/* ****************************************************************************
*
* compress - 
*/
void ExpandList::compress(void)
{
	memberBox->hide();
}



/* ****************************************************************************
*
* exCoOnButtonPress - 
*/
void ExpandList::exCoOnButtonPress(bool _autoExCo)
{
	autoExCo = _autoExCo;
}



/* ****************************************************************************
*
* titleSet - 
*/
void ExpandList::titleSet(const char* _title)
{
	QGraphicsSimpleTextItem* qtitle = (QGraphicsSimpleTextItem*) title->gItemP;

	qtitle->setText(_title);
}



/* ****************************************************************************
*
* menu - 
*/
void ExpandList::menu(MenuFunction callback, const char* mVec[])
{
	for (int ix = 0; mVec[ix] != NULL; ix++)
	{
		LM_T(LmtMenu, ("mVec[%d]: '%s'", ix, mVec[ix]));
		title->menuAdd(mVec[ix], callback, (void*) mVec[ix]);
	}
}



/* ****************************************************************************
*
* setFrame - 
*/
void ExpandList::setFrame(int padding)
{
	if (padding == -19)
	{
		LM_W(("deleting frame"));

		if (memberBox->frame != NULL)
			delete memberBox->frame;
		memberBox->frame = NULL;
	}
	else
	{
		LM_T(LmtFrame, ("Creating Frame for memberBox"));
		memberBox->setFrame(padding);
	}
}

}
