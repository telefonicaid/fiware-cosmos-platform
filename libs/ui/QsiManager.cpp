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
#include "traceLevels.h"        // Lmt*

#include "QsiFunction.h"        // QsiFunction
#include "QsiCallback.h"        // QsiCallback
#include "QsiBlock.h"           // QsiBlock
#include "QsiBox.h"             // QsiBox
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
	LM_T(LmtImage, ("Setting background to '%s'", bgPath));
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

		LM_T(LmtImage, ("Setting background size to %d x %d", pixmapSize.width(), pixmapSize.height()));
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
	
	box = new QsiBox(this, NULL, "topbox", 0, 0);
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

			LM_T(LmtMenu, ("Menu for '%s'", activeItem->name));
			for (int ix = 0; ix < QSI_MENU_ACTIONS; ix++)
			{

				if (activeItem->menuTitle[ix] == NULL)
					continue;

				LM_T(LmtMenu, ("Menu item %d for '%s': %s", ix, activeItem->name, activeItem->menuTitle[ix]));
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
			LM_W(("No associated menu for '%s'", activeItem->name));
	}
	else
		LM_T(LmtMenu, ("Menu for BACKGROUND"));

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

	LM_T(LmtMousePress, ("Some Mouse button pressed"));

	if (buttons == Qt::LeftButton)
	{
		point       = mouseEvent->buttonDownScenePos(Qt::LeftButton);
		gItemP      = itemAt(point);
		activeItem  = lookup(gItemP);
		
		if (activeItem != NULL)
		{
			LM_T(LmtMousePress, ("Left Mouse Press on '%s' - looking up callback", activeItem->name));

			QsiCallback* cb = itemCallbackLookup(activeItem);
			if (cb != NULL)
			{
				LM_T(LmtMenu, ("Found menu callback (%p) for '%s', function at %p", cb, activeItem->name, cb->func));
				cb->func(activeItem, cb->param);
			}
		}
		else
			LM_T(LmtMousePress, ("Left Mouse Press on BACKGROUND"));
	}
	else if (buttons == Qt::MidButton)
	{
		point       = mouseEvent->buttonDownScenePos(Qt::MidButton);
		gItemP      = itemAt(point);
		activeItem  = lookup(gItemP);

		if (activeItem != NULL)
			LM_T(LmtMousePress, ("Mid Mouse Press on %s '%s'", activeItem->typeName(), activeItem->name));
		else
			LM_T(LmtMousePress, ("Mid Mouse Press on BACKGROUND"));
	}
	else if (buttons == Qt::RightButton)
	{
		LM_T(LmtMousePress, ("Right Mouse Press"));
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
			LM_T(LmtMove, ("moving %s '%s'", activeItem->typeName(), activeItem->name));

			if (activeItem->getBoxMove() == true)
			{
				LM_T(LmtMove, ("%s '%s' has the property 'BOX MOVE' set  - move entire '%s' box", activeItem->typeName(), activeItem->name, activeItem->getOwner()->name));
				activeItem->getOwner()->moveRelative(point.x() - lastPoint.x(), point.y() - lastPoint.y());
			}
			else
				activeItem->moveRelative(point.x() - lastPoint.x(), point.y() - lastPoint.y());
		}
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

		LM_T(LmtMouse, ("Added callback for '%s'", qbP->name));
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
	LM_D(("lookup QsiBlock with gItem %p", gItemP));

	return box->lookup(gItemP);
}



/* ****************************************************************************
*
* menuActionFunc0 - 
*/
void QsiManager::menuActionFunc0(void)
{
	LM_T(LmtMenu, ("In 'menuActionFunc0': calling menu action '%s'", activeItem->menuTitle[0]));

	activeItem->menuFunc[0](activeItem, activeItem->menuParam[0]);
}



/* ****************************************************************************
*
* menuActionFunc1 - 
*/
void QsiManager::menuActionFunc1()
{
	LM_T(LmtMenu, ("In 'menuActionFunc1': calling menu action '%s'", activeItem->menuTitle[1]));

	activeItem->menuFunc[1](activeItem, activeItem->menuParam[1]);
}



/* ****************************************************************************
*
* menuActionFunc2 - 
*/
void QsiManager::menuActionFunc2()
{
	LM_T(LmtMenu, ("In 'menuActionFunc2': calling menu action '%s'", activeItem->menuTitle[2]));

	activeItem->menuFunc[2](activeItem, activeItem->menuParam[2]);
}



/* ****************************************************************************
*
* menuActionFunc3 - 
*/
void QsiManager::menuActionFunc3()
{
	LM_T(LmtMenu, ("In 'menuActionFunc3': calling menu action '%s'", activeItem->menuTitle[3]));
}



/* ****************************************************************************
*
* menuActionFunc4 - 
*/
void QsiManager::menuActionFunc4()
{
	LM_T(LmtMenu, ("In 'menuActionFunc4': calling menu action '%s'", activeItem->menuTitle[4]));
}



/* ****************************************************************************
*
* menuActionFunc5 - 
*/
void QsiManager::menuActionFunc5()
{
	LM_T(LmtMenu, ("In 'menuActionFunc5': calling menu action '%s'", activeItem->menuTitle[5]));
}



/* ****************************************************************************
*
* menuActionFunc6 - 
*/
void QsiManager::menuActionFunc6()
{
	LM_T(LmtMenu, ("In 'menuActionFunc6': calling menu action '%s'", activeItem->menuTitle[6]));
}



/* ****************************************************************************
*
* menuActionFunc7 - 
*/
void QsiManager::menuActionFunc7()
{
	LM_T(LmtMenu, ("In 'menuActionFunc7': calling menu action '%s'", activeItem->menuTitle[7]));
}



/* ****************************************************************************
*
* menuActionFunc8 - 
*/
void QsiManager::menuActionFunc8()
{
	LM_T(LmtMenu, ("In 'menuActionFunc8': calling menu action '%s'", activeItem->menuTitle[8]));
}



/* ****************************************************************************
*
* menuActionFunc9 - 
*/
void QsiManager::menuActionFunc9()
{
	LM_T(LmtMenu, ("In 'menuActionFunc9': calling menu action '%s'", activeItem->menuTitle[9]));
}

}
