/* ****************************************************************************
*
* FILE                     QsiManager.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 09 2011
*
*/
#include <QApplication>
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Lmt*

#include "QsiFunction.h"        // Function
#include "QsiCallback.h"        // Callback
#include "QsiBlock.h"           // Block
#include "QsiBox.h"             // Box
#include "QsiManager.h"         // Own interface



namespace Qsi
{



/* ****************************************************************************
*
* Global variables
*/
static Block*             activeItem    = NULL; 
static Qt::MouseButtons   pendingButton = Qt::NoButton;
static QPointF            pressPoint;
static bool               moved         = false;



/* ****************************************************************************
*
* Manager - 
*/
Manager::Manager(QVBoxLayout* layout, const char* homeDir, const char* background, int _width, int _height, int _winWidth, int _winHeight)
{
	QSize size;

	totalWidth   = _width;
	totalHeight  = _height;
	winWidth     = _winWidth;
	winHeight    = _winHeight;

	view         = new QGraphicsView(this);
	modal        = NULL;
	Home         = strdup(homeDir);

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
		setSceneRect(QRectF(0, 0, totalWidth, totalHeight));
		view->setSceneRect(0, 0, totalWidth, totalHeight);
		view->setMaximumSize(totalWidth * 2, totalHeight * 2);

		sceneLayer0 = NULL;
		sceneLayer1 = NULL;
	}
	else
	{
		bg = new QPixmap(bgPath);

		sceneLayer0 = addPixmap(*bg);
		sceneLayer1 = addPixmap(*bg);
		sceneLayer1->setOpacity(0);

		pixmapSize = bg->size();

		LM_T(LmtImage, ("Setting background size to %d x %d", pixmapSize.width(), pixmapSize.height()));
		setSceneRect(QRectF(0, 0, pixmapSize.width(), pixmapSize.height()));

		view->setSceneRect(0, 0, totalWidth, totalHeight);
		view->setMaximumSize(pixmapSize.width(), pixmapSize.height());
	}

	
	layout->addWidget(view);

	items           = 0;
	itemMax         = 20;
	item            = (Block**) calloc(itemMax, sizeof(Block*));

	itemCallbacks   = 0;
	itemCallbackMax = 10;
	itemCallback    = (Callback**) calloc(itemCallbackMax, sizeof(Callback*));
	
	box = new Box(this, NULL, "topbox", 0, 0);
	x = 0;
	y = 0;


	for (int ix = 0; ix < QSI_MENU_ACTIONS; ix++)
	{
		menuTitle[ix]  = NULL;
		menuFunc[ix]   = NULL;
		menuParam[ix]  = NULL;
	}	
}



/* ****************************************************************************
*
* ~Manager - 
*/
Manager::~Manager()
{
}



/* ****************************************************************************
*
* contextMenuEvent - 
*/
void Manager::contextMenuEvent(QGraphicsSceneContextMenuEvent* contextMenuEvent)
{
	QPointF             scenePos;
	QPoint              screenPos;
	QGraphicsItem*      gItem;

	if (modal != NULL)
	{
		LM_T(LmtModal, ("Sorry, modal is set (%p)", modal));
		return;
	}

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
	{
		QMenu menu("background");
		LM_T(LmtMenu, ("Menu for BACKGROUND"));
		for (int ix = 0; ix < QSI_MENU_ACTIONS; ix++)
		{
			if (menuTitle[ix] == NULL)
				continue;

			LM_T(LmtMenu, ("Menu item %d for '%s': %s", ix, "BACKGROUND", menuTitle[ix]));
			if (menuAction[ix] != NULL)
				delete menuAction[ix];

			menuAction[ix] = new QAction(menuTitle[ix], this);
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
}



static float oldZ = -1;
/* ****************************************************************************
*
* mousePressEvent - 
*/
void Manager::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
	QGraphicsItem*      gItemP;

	LM_T(LmtMousePress, ("Some Mouse button pressed"));

	pendingButton = mouseEvent->buttons();
	moved         = false;

	if (pendingButton == Qt::LeftButton)
	{
		pressPoint  = mouseEvent->buttonDownScenePos(Qt::LeftButton);
		gItemP      = itemAt(pressPoint);
		activeItem  = lookup(gItemP);

		if (activeItem)
		{
			oldZ = activeItem->getZValue();
			activeItem->setZValue(0.99);
		}
		else
			oldZ = -1;

		if (gItemP == sceneLayer0)
			LM_T(LmtMousePress, ("PRESS ON sceneLayer0"));
		else if (gItemP == sceneLayer1)
			LM_T(LmtMousePress, ("PRESS ON sceneLayer1"));
		else if (activeItem != NULL)
		{
			if (activeItem->w.vP != NULL)
				QGraphicsScene::mousePressEvent(mouseEvent);

			LM_T(LmtMousePress, ("PRESS ON %s '%s'", activeItem->typeName(), activeItem->name));
		}
		else
		{
			LM_T(LmtMousePress, ("PRESS ON Unidentified Graphics Item %p - giving it focus", gItemP));
			gItemP->setFocus();
			QGraphicsScene::mousePressEvent(mouseEvent);
			return;
		}

		if (activeItem != NULL)
			LM_T(LmtMousePress, ("Left Mouse Press on '%s' - looking up callback", activeItem->name));
		else
			LM_T(LmtMousePress, ("Left Mouse Press on BACKGROUND (gItemP: %p)", gItemP));
	}
	else if (pendingButton == Qt::MidButton)
	{
		pressPoint  = mouseEvent->buttonDownScenePos(Qt::MidButton);
		gItemP      = itemAt(pressPoint);
		activeItem  = lookup(gItemP);

		if (activeItem != NULL)
			LM_T(LmtMousePress, ("Mid Mouse Press on %s '%s'", activeItem->typeName(), activeItem->name));
		else
			LM_T(LmtMousePress, ("Mid Mouse Press on BACKGROUND (gItemP: %p)", gItemP));
	}
	else if (pendingButton == Qt::RightButton)
	{
		pressPoint  = mouseEvent->buttonDownScenePos(Qt::LeftButton);
		gItemP      = itemAt(pressPoint);
		activeItem  = lookup(gItemP);

		if (activeItem != NULL)
			LM_T(LmtMousePress, ("Right Mouse Press on '%s' - looking up callback", activeItem->name));
		else
			LM_T(LmtMousePress, ("Right Mouse Press on BACKGROUND (gItemP: %p)", gItemP));
	}
}



/* ****************************************************************************
*
* mouseReleaseEvent - 
*/
void Manager::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
	Qt::MouseButtons    buttons = mouseEvent->buttons();
	QPointF             point;
	QGraphicsItem*      gItemP;
	Block*              released;

	update(0, 0, totalWidth, totalHeight);

	LM_T(LmtMousePress, ("Some Mouse button released"));

	if (moved == true)
	{
		LM_W(("No button-release action taken - a move has been performed"));
		pendingButton = Qt::NoButton;
		return;
	}

	if (pendingButton == Qt::LeftButton)
	{
		point       = mouseEvent->buttonDownScenePos(Qt::LeftButton);
		gItemP      = itemAt(point);
		released    = lookup(gItemP);
		
		if (oldZ != -1)
			activeItem->setZValue(oldZ);
		oldZ = -1;

		LM_T(LmtPress, ("Left Mouse Button released: %p, activeItem: %p", released, activeItem));

		if (gItemP == sceneLayer0)
			LM_T(LmtMousePress, ("PRESS ON sceneLayer0"));
		else if (gItemP == sceneLayer1)
			LM_T(LmtMousePress, ("PRESS ON sceneLayer1"));
		else if (activeItem != NULL)
		{
			if (activeItem->w.vP != NULL)
				QGraphicsScene::mousePressEvent(mouseEvent);
			LM_T(LmtMousePress, ("PRESS ON %s '%s'", activeItem->typeName(), activeItem->name));
		}
		else
		{
			LM_T(LmtMousePress, ("PRESS ON Unidentified Graphics Item %p - giving it focus", gItemP));
			gItemP->setFocus();
			QGraphicsScene::mouseReleaseEvent(mouseEvent);
			return;
		}

		if ((released == activeItem) && (activeItem != NULL))
		{
			LM_T(LmtMousePress, ("Left Mouse Press & Release on '%s' - looking up callback", activeItem->name));

			if (modal != NULL)
			{
				if (!released->isAncestor(modal))
				{
					LM_T(LmtModal, ("Modal is on for box %s", modal->name));
					return;
				}

				LM_T(LmtModal, ("Press & Release on %s '%s' - modal is on for box %s but that's an ancestor of mine ...",
								released->typeName(), released->name, modal->name));
			}

			if (released->type == Input)
			{
				LM_T(LmtFocus, ("%s '%s' takes focus", released->typeName(), released->name));
				released->w.lineEdit->setFocus();
			}

			if (released->type == Combo)
			{
				LM_T(LmtFocus, ("%s '%s' takes focus", released->typeName(), released->name));
				released->w.combo->setFocus();
				released->w.combo->showPopup();
			}

			Callback* cb = itemCallbackLookup(activeItem);
			if (cb != NULL)
			{
				LM_T(LmtMenu, ("Found callback (%p) for '%s', function at %p", cb, activeItem->name, cb->func));
				cb->func(activeItem, cb->param);
			}
		}
		else
		   LM_T(LmtMousePress, ("Left Mouse Press on BACKGROUND (gItemP: %p)", gItemP));
	}
	else if (pendingButton == Qt::MidButton)
	{
		point       = mouseEvent->buttonDownScenePos(Qt::MidButton);
		gItemP      = itemAt(point);
		released    = lookup(gItemP);

		LM_T(LmtPress, ("Mid Mouse Button released: %p, activeItem: %p", released, activeItem));

		if (released != NULL)
			LM_T(LmtMousePress, ("Mid Mouse Press on %s '%s'", released->typeName(), released->name));
		else
			LM_T(LmtMousePress, ("Mid Mouse Press on BACKGROUND (gItemP: %p)", gItemP));
	}
	else if (pendingButton == Qt::RightButton)
	{
		point       = mouseEvent->buttonDownScenePos(Qt::MidButton);
		gItemP      = itemAt(point);
		released    = lookup(gItemP);

		LM_T(LmtPress, ("Right Mouse Button released: %p, activeItem: %p", released, activeItem));

		if (released != NULL)
			LM_T(LmtMousePress, ("Right Mouse Press on %s '%s'", released->typeName(), released->name));
		else
		   LM_T(LmtMousePress, ("Right Mouse Press on BACKGROUND (gItemP: %p)", gItemP));
	}

	pendingButton = Qt::NoButton;
}



/* ****************************************************************************
*
* mouseMoveEvent - 
*/
void Manager::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
	Qt::MouseButtons    buttons    = mouseEvent->buttons();
	QPointF             lastPoint  = mouseEvent->lastScenePos();
	QPointF             point      = mouseEvent->scenePos();

	if (activeItem == NULL)
	{
		// LM_W(("Sorry, no selected item ..."));
		return;
	}

	if (pendingButton == (Qt::LeftButton))
	{
		if (activeItem->getMovable() == true)
		{
			LM_T(LmtMove, ("moving %s '%s'", activeItem->typeName(), activeItem->name));

			// Don't be so sensitive to movements - at least 10 pixels ...
			if ((abs(pressPoint.x() - point.x()) < 10) && (abs(pressPoint.y() - point.y()) < 10))
				return;

			moved = true;
			if (activeItem->boxMoveGet() == true)
			{
				LM_T(LmtMove, ("%s '%s' has the property 'BOX MOVE' set  - move entire '%s' box", activeItem->typeName(), activeItem->name, activeItem->getOwner()->name));
				activeItem->getOwner()->moveRelative(point.x() - lastPoint.x(), point.y() - lastPoint.y());
			}
			else
				activeItem->moveRelative(point.x() - lastPoint.x(), point.y() - lastPoint.y());
		}
		else
			LM_T(LmtMove, ("Sorry, %s '%s' is set to non-movable", activeItem->typeName(), activeItem->name));
	}
}



/* ****************************************************************************
*
* wheelEvent - 
*/
void Manager::wheelEvent(QGraphicsSceneWheelEvent* event)
{
	QPointF          point    = event->scenePos();
	int              dy       = (event->delta() > 0)? 10 : -10;
	QGraphicsItem*   gItemP   = itemAt(point);
	Base*            qbP      = lookup(gItemP);
	Box*             xbox;

	LM_M(("Mouse wheel dy == %d", dy));

	ScrollArea* saP;
	saP = box->scrollAreaLookup(point.x(), point.y());
	if (saP != NULL)
	{
		LM_M(("Found scroll area"));
		saP->box->scroll(dy);
	}

	if ((qbP == NULL) || (((Block*) qbP)->w.vP != NULL))
	{
		LM_M(("NULL Qsi pointer"));
		QGraphicsScene::wheelEvent(event);
		return;
	}
	
	xbox = qbP->owner;

	if (xbox->scrollable == true)
	{
		LM_M(("box is scrollable"));
		xbox->scroll(dy);
	}
}



/* ****************************************************************************
*
* siConnect - 
*/
void Manager::siConnect(Block* qbP, Function func, const void* param, bool persistent)
{
	for (int ix = 0; ix < itemCallbackMax; ix++)
    {
		if (itemCallback[ix] != NULL)
            continue;

		itemCallback[ix] = (Callback*) malloc(sizeof(Callback));		

		itemCallback[ix]->qbP        = qbP;
		itemCallback[ix]->func       = func;
		itemCallback[ix]->param      = (void*) param;
		itemCallback[ix]->persistent = persistent;

		LM_T(LmtMouse, ("Added callback for '%s'", qbP->name));
		return;
	}

	itemCallbackMax += 5;
	
    LM_W(("REALLOC to %d items", itemCallbackMax));
	itemCallback = (Callback**) realloc(itemCallback, itemCallbackMax * sizeof(Callback*));
	
	for (int ix = itemCallbackMax - 5; ix < itemCallbackMax; ix++)
	   itemCallback[ix] = NULL;

	siConnect(qbP, func, param, persistent);
}



/* ****************************************************************************
*
* siDisconnect - 
*/
void Manager::siDisconnect(Block* qbP)
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
Callback* Manager::itemCallbackLookup(Block* qbP)
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
Block* Manager::lookup(QGraphicsItem* gItemP)
{
	LM_D(("lookup Block with gItem %p", gItemP));

	return box->lookup(gItemP);
}



/* ****************************************************************************
*
* menuActionFunc0 - 
*/
void Manager::menuActionFunc0(void)
{
	if (activeItem)
	{
		LM_T(LmtMenu, ("In 'menuActionFunc0': calling menu action '%s'", activeItem->menuTitle[0]));
		activeItem->menuFunc[0](activeItem, activeItem->menuParam[0]);
	}
	else
		menuFunc[0](NULL, menuParam[0]);
}



/* ****************************************************************************
*
* menuActionFunc1 - 
*/
void Manager::menuActionFunc1()
{
	if (activeItem)
	{
		LM_T(LmtMenu, ("In 'menuActionFunc1': calling menu action '%s'", activeItem->menuTitle[1]));
		activeItem->menuFunc[1](activeItem, activeItem->menuParam[1]);
	}
	else
		menuFunc[1](NULL, menuParam[1]);
}



/* ****************************************************************************
*
* menuActionFunc2 - 
*/
void Manager::menuActionFunc2()
{
	if (activeItem)
	{
		LM_T(LmtMenu, ("In 'menuActionFunc2': calling menu action '%s'", activeItem->menuTitle[2]));
		activeItem->menuFunc[2](activeItem, activeItem->menuParam[2]);
	}
	else
		menuFunc[2](NULL, menuParam[2]);
}



/* ****************************************************************************
*
* menuActionFunc3 - 
*/
void Manager::menuActionFunc3()
{
	if (activeItem)
	{
		LM_T(LmtMenu, ("In 'menuActionFunc3': calling menu action '%s'", activeItem->menuTitle[3]));
		activeItem->menuFunc[3](activeItem, activeItem->menuParam[3]);
	}
	else
		menuFunc[3](NULL, menuParam[3]);
}



/* ****************************************************************************
*
* menuActionFunc4 - 
*/
void Manager::menuActionFunc4()
{
	if (activeItem)
	{
		LM_T(LmtMenu, ("In 'menuActionFunc4': calling menu action '%s'", activeItem->menuTitle[4]));
		activeItem->menuFunc[4](activeItem, activeItem->menuParam[4]);
	}
	else
		menuFunc[4](NULL, menuParam[4]);
}



/* ****************************************************************************
*
* menuActionFunc5 - 
*/
void Manager::menuActionFunc5()
{
	if (activeItem)
	{
		LM_T(LmtMenu, ("In 'menuActionFunc5': calling menu action '%s'", activeItem->menuTitle[5]));
		activeItem->menuFunc[5](activeItem, activeItem->menuParam[5]);
	}
	else
		menuFunc[5](NULL, menuParam[5]);
}



/* ****************************************************************************
*
* menuActionFunc6 - 
*/
void Manager::menuActionFunc6()
{
	if (activeItem)
	{
		LM_T(LmtMenu, ("In 'menuActionFunc6': calling menu action '%s'", activeItem->menuTitle[6]));
		activeItem->menuFunc[6](activeItem, activeItem->menuParam[6]);
	}
	else
		menuFunc[6](NULL, menuParam[6]);
}



/* ****************************************************************************
*
* menuActionFunc7 - 
*/
void Manager::menuActionFunc7()
{
	if (activeItem)
	{
		LM_T(LmtMenu, ("In 'menuActionFunc7': calling menu action '%s'", activeItem->menuTitle[7]));
		activeItem->menuFunc[7](activeItem, activeItem->menuParam[7]);
	}
	else
		menuFunc[7](NULL, menuParam[7]);
}



/* ****************************************************************************
*
* menuActionFunc8 - 
*/
void Manager::menuActionFunc8()
{
	if (activeItem)
	{
		LM_T(LmtMenu, ("In 'menuActionFunc8': calling menu action '%s'", activeItem->menuTitle[8]));
		activeItem->menuFunc[8](activeItem, activeItem->menuParam[8]);
	}
	else
		menuFunc[8](NULL, menuParam[8]);
}



/* ****************************************************************************
*
* menuActionFunc9 - 
*/
void Manager::menuActionFunc9()
{
	if (activeItem)
	{
		LM_T(LmtMenu, ("In 'menuActionFunc9': calling menu action '%s'", activeItem->menuTitle[9]));
		activeItem->menuFunc[9](activeItem, activeItem->menuParam[9]);
	}
	else
		menuFunc[9](NULL, menuParam[9]);
}



/* ****************************************************************************
*
* menuAdd - 
*/
void Manager::menuAdd(const char* title, Function func, void* param)
{
	for (int ix = 0; ix < QSI_MENU_ACTIONS; ix++)
	{
		if (menuFunc[ix] != NULL)
			continue;

		menuTitle[ix]  = strdup(title);
		menuFunc[ix]   = func;
		menuParam[ix]  = param;
		return;
	}

	LM_W(("Maximum %d menu entries", QSI_MENU_ACTIONS));
}



/* ****************************************************************************
*
* menuClear - 
*/
void Manager::menuClear(void)
{
	for (int ix = 0; ix < QSI_MENU_ACTIONS; ix++)
	{
		if (menuFunc[ix] == NULL)
			continue;
		
		free(menuTitle[ix]);
		menuTitle[ix]  = NULL;
		menuFunc[ix]   = NULL;
		menuParam[ix]  = NULL;
	}
}



/* ****************************************************************************
*
* grab - 
*/
void Manager::grab(Base* base)
{
	if (base == NULL)
		LM_E(("Cannot grab with a NULL pointer!"));
	else if (modal == NULL)
	{
		if (base->isBox == true)
			modal = (Box*) base;
		else
			modal = base->owner;
		
		LM_T(LmtModal, ("Set modal to %p", modal));
	}
	else
		LM_W(("Can't set %p as modal, modal is already set to %p", base, modal));
}



/* ****************************************************************************
*
* ungrab - 
*/
void Manager::ungrab(Box* box)
{
	if (box == NULL)
		LM_E(("Cannot ungrab with a NULL pointer!"));
	else if (modal == NULL)
		LM_W(("Not in modal mode (try to ungrab %p)", box));
	else 
	{
		if (box != modal)
			LM_E(("Cannot ungrab with distinct object: (modal: %p) vs (wanted: %p)", modal, box));
		else
		{
			LM_T(LmtModal, ("ungrabbed using %p", modal));
			modal      = NULL;
		}
	}
}

}
