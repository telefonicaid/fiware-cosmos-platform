/* ****************************************************************************
*
* FILE                     QsiManager.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 09 2011
*
*/
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>

#include "logMsg.h"             // LM_*

#include "QsiFunction.h"        // QsiFunction
#include "QsiCallback.h"        // QsiCallback
#include "QsiBlock.h"           // QsiBlock
#include "QsiManager.h"         // Own interface



namespace Qsi
{



/* ****************************************************************************
*
* Global variables
*/
QsiBlock* activeItem = NULL; 



/* ****************************************************************************
*
* QsiManager::QsiManager - 
*/
QsiManager::QsiManager(QVBoxLayout* layout, const char* homeDir, const char* background, int width, int height)
{
	view = new QGraphicsView(this);


	Home = strdup(homeDir);

	//
	// Menu callbacks
	//
	for (int ix = 0; ix < QSI_MENU_ACTIONS; ix++)
		menuAction[ix] = NULL;



	//
	// Background
	//
	QSize         pixmapSize;
	QPixmap*      bg;
	char          bgPath[256];

	snprintf(bgPath, sizeof(bgPath), "%s/%s", homeDir, background);
	LM_M(("Setting background to '%s'", bgPath));
	if (access(bgPath, R_OK) != 0)
	{
		LM_W(("background file '%s' is missing", bgPath));
		setSceneRect(QRectF(0, 0, width, height));
		view->setSceneRect(0, 0, width, height);
		view->setMaximumSize(width * 2, height * 2);

	}
	else
	{
		bg = new QPixmap(bgPath);

		addPixmap(*bg);

		pixmapSize = bg->size();

		LM_M(("Setting background size to %dx%d", pixmapSize.width(), pixmapSize.height()));
		setSceneRect(QRectF(0, 0, pixmapSize.width(), pixmapSize.height()));

		view->setSceneRect(0, 0, width, height);
		view->setMaximumSize(pixmapSize.width(), pixmapSize.height());
	}

	
	layout->addWidget(view);

	items           = 0;
	itemMax         = 20;
	item            = (QsiBlock**) calloc(itemMax, sizeof(QsiBlock*));

	itemCallbacks   = 0;
	itemCallbackMax = 10;
	itemCallback    = (QsiCallback**) calloc(itemCallbackMax, sizeof(QsiCallback*));
	

	x = 0;
	y = 0;
}



/* ****************************************************************************
*
* QsiManager::~QsiManager - 
*/
QsiManager::~QsiManager()
{
}



/* ****************************************************************************
*
* QsiManager::contextMenuEvent - 
*/
void QsiManager::contextMenuEvent(QGraphicsSceneContextMenuEvent* contextMenuEvent)
{
	QPointF             scenePos;
	QPoint              screenPos;
	QGraphicsItem*      gItem;

	screenPos     = contextMenuEvent->screenPos();
	scenePos      = contextMenuEvent->scenePos();
	gItem         = itemAt(scenePos);

	activeItem    = lookup(gItem);

	if (activeItem != NULL)
	{
		if (activeItem->menu)
		{
			QMenu menu(activeItem->name);

			LM_M(("Menu for '%s'", activeItem->name));
			for (int ix = 0; ix < QSI_MENU_ACTIONS; ix++)
			{

				if (activeItem->menuTitle[ix] == NULL)
					continue;

				LM_M(("Menu item %d for '%s': %s", ix, activeItem->name, activeItem->menuTitle[ix]));
				if (menuAction[ix] != NULL)
					delete menuAction[ix];

				menuAction[ix] = new QAction(activeItem->menuTitle[ix], this);
				menu.addAction(menuAction[ix]);
				if (ix == 0)
					connect(menuAction[ix], SIGNAL(triggered()), this, SLOT(menuActionFunc0()));
				else if (ix == 1)
					connect(menuAction[ix], SIGNAL(triggered()), this, SLOT(menuActionFunc1()));
				else if (ix == 2)
					connect(menuAction[ix], SIGNAL(triggered()), this, SLOT(menuActionFunc2()));
				else if (ix == 3)
					connect(menuAction[ix], SIGNAL(triggered()), this, SLOT(menuActionFunc3()));
				else if (ix == 4)
					connect(menuAction[ix], SIGNAL(triggered()), this, SLOT(menuActionFunc4()));
				else if (ix == 5)
					connect(menuAction[ix], SIGNAL(triggered()), this, SLOT(menuActionFunc5()));
				else if (ix == 6)
					connect(menuAction[ix], SIGNAL(triggered()), this, SLOT(menuActionFunc6()));
				else if (ix == 7)
					connect(menuAction[ix], SIGNAL(triggered()), this, SLOT(menuActionFunc7()));
				else if (ix == 8)
					connect(menuAction[ix], SIGNAL(triggered()), this, SLOT(menuActionFunc8()));
				else if (ix == 9)
					connect(menuAction[ix], SIGNAL(triggered()), this, SLOT(menuActionFunc9()));
			}
			menu.exec(screenPos);
		}
		else
			LM_M(("No associated menu for '%s'", activeItem->name));
	}
	else
		LM_M(("Menu for BACKGROUND"));

}


/* ****************************************************************************
*
* QsiManager::mousePressEvent - 
*/
void QsiManager::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
	Qt::MouseButtons    buttons = mouseEvent->buttons();
	QPointF             point;
	QGraphicsItem*      gItemP;

	if (buttons == Qt::LeftButton)
	{
		point       = mouseEvent->buttonDownScenePos(Qt::LeftButton);
		gItemP      = itemAt(point);
		activeItem  = lookup(gItemP);
		
		if (activeItem != NULL)
		{
			LM_M(("Left Mouse Press on '%s' - looking up callback", activeItem->name));

			QsiCallback* cb = itemCallbackLookup(activeItem);
			if (cb != NULL)
			{
				LM_M(("Found callback (%p) for '%s', function at %p", cb, activeItem->name, cb->func));
				cb->func(activeItem, cb->param);
			}
		}
		else
			LM_M(("Left Mouse Press on BACKGROUND"));
	}
	else if (buttons == Qt::MidButton)
	{
		point       = mouseEvent->buttonDownScenePos(Qt::MidButton);
		gItemP      = itemAt(point);
		activeItem  = lookup(gItemP);
	}
}



/* ****************************************************************************
*
* QsiManager::mouseMoveEvent - 
*/
void QsiManager::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
	Qt::MouseButtons    buttons    = mouseEvent->buttons();
	QPointF             lastPoint  = mouseEvent->lastScenePos();
	QPointF             point      = mouseEvent->scenePos();

	if (activeItem == NULL)
	{
		// LM_W(("Sorry, no selected item ..."));
		return;
	}

	if (buttons == (Qt::MidButton))
	{
		if (activeItem->getMovable() == true)
		{
			if ((activeItem->groupNext == NULL) && (activeItem->groupPrev == NULL))
				activeItem->move(point.x() - lastPoint.x(), point.y() - lastPoint.y());
			else
				groupMove(activeItem, point.x() - lastPoint.x(), point.y() - lastPoint.y());
		}
	}
}



/* ****************************************************************************
*
* QsiManager::add - 
*/
void QsiManager::add(QsiBlock* qbP)
{
	for (int ix = 0; ix < itemMax; ix++)
	{
		if (item[ix] != NULL)
			continue;

		LM_M(("Adding QsiBlock %02d '%s' (%p %p)", ix, qbP->name, qbP->gItemP, qbP->proxy));
		item[ix] = qbP;
		++items;
		return;
	}

	itemMax += 5;	
	LM_W(("REALLOC to %d items", itemMax));	
	item     = (QsiBlock**) realloc(item, itemMax * sizeof(QsiBlock*));

	for (int ix = itemMax - 5; ix < itemMax; ix++)
	   item[ix] = NULL;
	
	add(qbP);
}



/* ****************************************************************************
*
* QsiManager::remove - 
*/
void QsiManager::remove(QsiBlock* itemP)
{
	for (int ix = 0; ix < itemMax; ix++)
	{
		if (item[ix] != itemP)
			continue;

		if (item[ix]->groupNext != NULL)
			item[ix]->groupNext->groupPrev = item[ix]->groupPrev;
		if (item[ix]->groupPrev != NULL)
            item[ix]->groupPrev->groupNext = item[ix]->groupNext;


		item[ix] = NULL;
		--items;
		delete itemP;
		return;
	}
}



/* ****************************************************************************
*
* QsiManager::siConnect - 
*/
void QsiManager::siConnect(QsiBlock* qbP, QsiFunction func, void* param, bool persistent)
{
	for (int ix = 0; ix < itemCallbackMax; ix++)
    {
		if (itemCallback[ix] != NULL)
            continue;

		itemCallback[ix] = (QsiCallback*) malloc(sizeof(QsiCallback));		

		itemCallback[ix]->qbP        = qbP;
		itemCallback[ix]->func       = func;
		itemCallback[ix]->param      = param;
		itemCallback[ix]->persistent = persistent;

		LM_M(("Added callback for '%s'", qbP->name));
		return;
	}

	itemCallbackMax += 5;
	
    LM_W(("REALLOC to %d items", itemCallbackMax));
	itemCallback = (QsiCallback**) realloc(itemCallback, itemCallbackMax * sizeof(QsiCallback*));
	
	for (int ix = itemCallbackMax - 5; ix < itemCallbackMax; ix++)
	   itemCallback[ix] = NULL;

	siConnect(qbP, func, param, persistent);
}



/* ****************************************************************************
*
* QsiManager::siDisconnect - 
*/
void QsiManager::siDisconnect(QsiBlock* qbP)
{
	if (qbP == NULL)
		LM_RVE(("Cannot disconnect a NULL item"));
	
	for (int ix = 0; ix < itemCallbackMax; ix++)
	{
		if (itemCallback[ix]->qbP != qbP)
			continue;

		free(itemCallback[ix]);
		
		itemCallback[ix] = NULL;
		itemCallback[ix]->func       = NULL;
		itemCallback[ix]->param      = NULL;
		itemCallback[ix]->persistent = false;

		return;
	}

	LM_E(("Callback for item '%s' not found ...", qbP->name));
}



/* ****************************************************************************
*
* itemCallbackLookup - 
*/
QsiCallback* QsiManager::itemCallbackLookup(QsiBlock* qbP)
{
	for (int ix = 0; ix < itemCallbackMax; ix++)
	{
		if (itemCallback[ix] == NULL)
			continue;

		if (itemCallback[ix]->qbP != qbP)
			continue;

		LM_D(("Comparing %p with %p (%s)", itemCallback[ix]->qbP, qbP, qbP->name));
		return itemCallback[ix];
	}

	return NULL;
}



/* ****************************************************************************
*
* lookup - 
*/
QsiBlock* QsiManager::lookup(QGraphicsItem* gItemP)
{
	LM_M(("Looking up scene item with QGraphicsItem %p", gItemP));
	for (int ix = 0; ix < itemMax; ix++)
	{
		if (item[ix] == NULL)
			continue;

		LM_D(("Comparing '%s' (gItemP at %p AND proxy at %p) with the pressed item (%p)",
			  item[ix]->name, item[ix]->gItemP, item[ix]->proxy, gItemP));

		if (gItemP == item[ix]->gItemP)
		{
			LM_M(("Found gItem '%s' (%p %p)!", item[ix]->name, gItemP, item[ix]->gItemP));
			return item[ix];
		}
		if (gItemP == item[ix]->proxy)
		{
			LM_M(("Found proxy '%s'! (%p %p %p)", item[ix]->name, gItemP, item[ix]->proxy, item[ix]->w));
			return item[ix];
		}
	}

	return NULL;
}



/* ****************************************************************************
*
* QsiManager::textAdd - 
*/
QsiBlock* QsiManager::textAdd(const char* name, const char* txt, int _x, int _y)
{
	return new QsiBlock(this, Qsi::SimpleText, name, txt, _x, _y);
}



/* ****************************************************************************
*
* QsiManager::lineAdd - 
*/
QsiBlock* QsiManager::lineAdd(const char* name, int _x, int _y, int x2, int y2)
{
	return new QsiBlock(this, Qsi::Line, name, name, _x, _y, x2, y2);
}



/* ****************************************************************************
*
* buttonAdd - 
*/
QsiBlock* QsiManager::buttonAdd(const char* name, const char* txt, int _x, int _y, int width, int height)
{
	return new QsiBlock(this, Qsi::Button, name, txt, _x, _y, width, height);
}



/* ****************************************************************************
*
* inputAdd - 
*/
QsiBlock* QsiManager::inputAdd(const char* name, const char* txt, int _x, int _y, int width, int height)
{
	return new QsiBlock(this, Qsi::Input, name, txt, _x, _y, width, height);
}



/* ****************************************************************************
*
* imageAdd - 
*/
QsiBlock* QsiManager::imageAdd(const char* name, const char* path, int _x, int _y, int width, int height)
{
	return new QsiBlock(this, Qsi::Image, name, path, _x, _y, width, height);
}



/* ****************************************************************************
*
* menuActionFunc0 - 
*/
void QsiManager::menuActionFunc0(void)
{
	LM_M(("In 'menuActionFunc0': calling menu action '%s'", activeItem->menuTitle[0]));

	activeItem->menuFunc[0](activeItem, activeItem->menuParam[0]);
}



/* ****************************************************************************
*
* menuActionFunc1 - 
*/
void QsiManager::menuActionFunc1()
{
	LM_M(("In 'menuActionFunc1': calling menu action '%s'", activeItem->menuTitle[1]));

	activeItem->menuFunc[1](activeItem, activeItem->menuParam[1]);
}



/* ****************************************************************************
*
* menuActionFunc2 - 
*/
void QsiManager::menuActionFunc2()
{
	LM_M(("In 'menuActionFunc2': calling menu action '%s'", activeItem->menuTitle[2]));

	activeItem->menuFunc[2](activeItem, activeItem->menuParam[2]);
}



/* ****************************************************************************
*
* menuActionFunc3 - 
*/
void QsiManager::menuActionFunc3()
{
	LM_M(("In 'menuActionFunc3': calling menu action '%s'", activeItem->menuTitle[3]));
}



/* ****************************************************************************
*
* menuActionFunc4 - 
*/
void QsiManager::menuActionFunc4()
{
	LM_M(("In 'menuActionFunc4': calling menu action '%s'", activeItem->menuTitle[4]));
}



/* ****************************************************************************
*
* menuActionFunc5 - 
*/
void QsiManager::menuActionFunc5()
{
	LM_M(("In 'menuActionFunc5': calling menu action '%s'", activeItem->menuTitle[5]));
}



/* ****************************************************************************
*
* menuActionFunc6 - 
*/
void QsiManager::menuActionFunc6()
{
	LM_M(("In 'menuActionFunc6': calling menu action '%s'", activeItem->menuTitle[6]));
}



/* ****************************************************************************
*
* menuActionFunc7 - 
*/
void QsiManager::menuActionFunc7()
{
	LM_M(("In 'menuActionFunc7': calling menu action '%s'", activeItem->menuTitle[7]));
}



/* ****************************************************************************
*
* menuActionFunc8 - 
*/
void QsiManager::menuActionFunc8()
{
	LM_M(("In 'menuActionFunc8': calling menu action '%s'", activeItem->menuTitle[8]));
}



/* ****************************************************************************
*
* menuActionFunc9 - 
*/
void QsiManager::menuActionFunc9()
{
	LM_M(("In 'menuActionFunc9': calling menu action '%s'", activeItem->menuTitle[9]));
}



/* ****************************************************************************
*
* group - 
*
* si1 - pointer to QsiBlock that is already in a group (or not)
* si2 - pointer to QsiBlock that is NOT in a group
*/
void QsiManager::group(QsiBlock* si1, QsiBlock* si2)
{
	QsiBlock* last = si1;

	if ((si2->groupNext != NULL) || (si2->groupPrev != NULL))
		LM_RVE(("Cannot add '%s' to the group of '%s' - '%s' seems to belong to a group already", si2->name, si1->name, si2->name));

	while (last->groupNext != NULL)
		last = last->groupNext;

	last->groupNext = si2;
	si2->groupPrev  = last;
}



/* ****************************************************************************
*
* groupMove - 
*/
void QsiManager::groupMove(QsiBlock* si, int x, int y)
{
	// 1. find first item of group
	// 2. move all of them

	while (si->groupPrev != NULL)
		si = si->groupPrev;

	while (si != NULL)
	{
		if (si->getMovable() == false)
		{
			si->setMovable(true);
			si->move(x, y);
			si->setMovable(false);
		}
		else
			si->move(x, y);

		si = si->groupNext;
	}
}

}
