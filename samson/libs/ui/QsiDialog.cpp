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
* FILE                     QsiDialog.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 18 2011
*
*/
#include "QsiManager.h"         // Manager
#include "QsiBox.h"             // Box
#include "QsiBlock.h"           // Block
#include "QsiDialog.h"          // Own interface



namespace Qsi
{



/* ****************************************************************************
*
* ok - 
*/
static void ok(Block* qbP, void* param, const char* nada)
{
	Dialog* dialog = (Dialog*) param;

	dialog->manager->box->remove(dialog, false);
	
	delete dialog;
	nada = NULL;
}



/* ****************************************************************************
*
* Constructor - 
*/
Dialog::Dialog(Manager* _manager, const char* _title, bool modal, bool final) : Box(_manager, NULL, _title, 0, 0)
{
	int wx;
	int wy;
	int wwidth;
	int wheight;

	this->typeSet(DialogItem);

	borderWidth  = 3;
	shadowX      = 10;
	shadowY      = 10;

	if (modal)
	{
		QPixmap  pixmap(manager->totalWidth, manager->totalHeight);

		pixmap.fill(QColor(0x0, 0x0, 0x0, 0xC0));
		background = manager->addPixmap(pixmap);
		background->setZValue(0.5);
	}
	else
		background = NULL;

	wx      = manager->winWidth  / 4;
	wy      = manager->winHeight / 4;
	wwidth  = manager->winWidth  / 2;
	wheight = manager->winHeight / 2;

	winBox = (Box*) boxAdd("dialogWinBox", wx, wy);

	shadow = (Block*) winBox->rectangleAdd("Shadow",   shadowX, shadowY, wwidth, wheight, QColor(0x00, 0x00, 0x00, 0xFF), QColor(0x00, 0x00, 0x00, 0xFF), 0);
	win    = (Block*) winBox->rectangleAdd("DialogWin", 0,  0, wwidth, wheight, QColor(0xD0, 0xD0, 0xFF, 0xFF), QColor(0x00, 0x00, 0x20, 0xFF), borderWidth);
	title  = (Block*) winBox->textAdd("DialogTitle", _title, 0, 0);

	win->setMovable(false);
	title->setMovable(false);
	shadow->setMovable(false);

	int tx, ty, tw, th;
	title->geometry(&tx, &ty, &tw, &th);
	
	title->moveRelative((wwidth - tw) / 2, th + 5);

	if (final)
	{
		manager->box->add(this);
		manager->siConnect(win, ok, this);

		if (modal)
			manager->grab(winBox);
	}

	shadow->gItemP->setZValue(0.6);
	win->gItemP->setZValue(0.65);
	title->gItemP->setZValue(0.7);

	manager->box->add(this);
}



/* ****************************************************************************
*
* Destructor - 
*/
Dialog::~Dialog()
{
	manager->box->remove(this, false);

	if (background != NULL)
	{
		manager->ungrab(winBox);
		delete background;
	}

	if (shadow)
		delete shadow;
	shadow = NULL;

	if (title)
		delete title;
	title = NULL;

	if (win)
		delete win;
	win = NULL;
}

}
