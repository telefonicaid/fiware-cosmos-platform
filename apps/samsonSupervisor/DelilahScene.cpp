/* ****************************************************************************
*
* FILE                     DelilahScene.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 02 2011
*
*/
#include <stdio.h>              // printf

#include <QObject>
#include <QMenu>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Lmt*

#include "globals.h"            // connectionMgr, ...
#include "Popup.h"              // Popup
#include "misc.h"               // centerCoordinates
#include "DelilahQueue.h"       // DelilahQueue
#include "DelilahConnection.h"  // DelilahConnection
#include "DelilahScene.h"       // Own interface



/* ****************************************************************************
*
* popupMenu - 
*/
static QMenu*        popupMenu           = NULL;
static bool          removeRequested     = false;
static bool          connectionRequested = false;
static DelilahQueue* connectFrom         = NULL;
static bool          createRequested     = false;



/* ****************************************************************************
*
* DelilahScene::DelilahScene - 
*/
DelilahScene::DelilahScene(QObject* parent) : QGraphicsScene(parent)
{
	testq1     = NULL;
	testq2     = NULL;
}



QGraphicsItem* selectedItem = NULL;
/* ****************************************************************************
*
* DelilahScene::mousePressEvent - 
*/
void DelilahScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
	Qt::MouseButtons  buttons = mouseEvent->buttons();
	QPointF           point;

	if (popupMenu != NULL)
	{
		if (buttons == Qt::LeftButton)
		{		
			point        = mouseEvent->buttonDownScenePos(Qt::LeftButton);
			selectedItem = itemAt(point);

			if (selectedItem != NULL)
				LM_T(LmtPopupMenu, ("Mouse press with visible popup menu, pressed item %p", selectedItem));
		}

		LM_T(LmtPopupMenu, ("Mouse press with visible popup menu ..."));
		delete popupMenu;
		popupMenu = NULL;
	}
	else if (buttons == Qt::RightButton)
	{
		QGraphicsItem* item;
        DelilahQueue*  q;
		QAction*       bindAction;
		QAction*       renameAction;
		QAction*       deleteAction;

		point = mouseEvent->buttonDownScenePos(Qt::RightButton);
		item = itemAt(point);
		q    = queueMgr->lookup(item);

		if (item != NULL)
		{
			popupMenu = new QMenu(q->displayName);

			bindAction   = new QAction("&Bind", NULL);
			renameAction = new QAction("&Rename", NULL);
			deleteAction = new QAction("&Delete", NULL);

			connect(bindAction,   SIGNAL(triggered()), this, SLOT(bind()));
			connect(renameAction, SIGNAL(triggered()), this, SLOT(rename()));
			connect(deleteAction, SIGNAL(triggered()), this, SLOT(remove()));

			popupMenu->addAction(bindAction);
			popupMenu->addSeparator();
			popupMenu->addAction(renameAction);
			popupMenu->addAction(deleteAction);

			addWidget(popupMenu);
			popupMenu->popup(QPoint((int) point.x(), (int) point.y()), bindAction);
		}
	}
	else if (buttons == Qt::LeftButton)
	{
		DelilahQueue*      q = NULL;
		DelilahConnection* c = NULL;

		point        = mouseEvent->buttonDownScenePos(Qt::LeftButton);
		selectedItem = itemAt(point);
		
		q = queueMgr->lookup(selectedItem);
		c = connectionMgr->lookup(selectedItem);

		if (createRequested)
		{
			DelilahQueue* q;

			q = new DelilahQueue(this, "images/queue.png", NULL, point.x() - 64, point.y() - 64);
			queueMgr->insert(q);			
		}
		else if (q != NULL)
		{
			LM_TODO(("Make '%s' the top-most item in item stack", q->displayName));

			if (connectionRequested)
			{
				if (connectFrom == NULL)
				{
					connectFrom = q;
					LM_T(LmtQueueConnection, ("Connecting '%s' ...", q->displayName));
					setCursor("images/to.png");
				}
				else
				{
					LM_T(LmtQueueConnection, ("Connecting '%s' with '%s'", connectFrom->displayName, q->displayName));
					connectionMgr->insert(this, connectFrom, q);
					connectionRequested = false;
					connectFrom = NULL;
				}
			}
			else if (removeRequested == true)
			{
				LM_T(LmtQueueConnection, ("removing queue %s (but first its connections) ...", q->displayName));

				connectionMgr->remove(q);
				queueMgr->remove(q);
			}
		}
		else if (c != NULL)
		{
			if (removeRequested == true)
			{
				LM_T(LmtQueueConnection, ("removing connection '%s' -> '%s'", c->qFromP->displayName, c->qToP->displayName));

				connectionMgr->remove(c);
			}
		}
	}



	//
	// Putting it all back, unless a connection is half done
	//
	if (connectionRequested == false)
	{
		setCursor(NULL);
		createRequested     = false;
		removeRequested     = false;
	}
}


/* ****************************************************************************
*
* DelilahScene::mouseMoveEvent - 
*/
void DelilahScene::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
	DelilahQueue* q;

	if (selectedItem == NULL)
		return;

	QPointF lastPoint = mouseEvent->lastScenePos();
	QPointF point     = mouseEvent->scenePos();

	q = queueMgr->lookup(selectedItem);
	if (q != NULL)
		q->moveTo(point.x() - lastPoint.x(), point.y() - lastPoint.y());
}



/* ****************************************************************************
*
* DelilahScene::mouseReleaseEvent - 
*/
void DelilahScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
	selectedItem = NULL;
	mouseEvent   = NULL;
}



/* ****************************************************************************
*
* DelilahScene::wheelEvent - 
*/
void DelilahScene::wheelEvent(QGraphicsSceneWheelEvent* wheelEvent)
{
	wheelEvent = NULL;
}



/* ****************************************************************************
*
* DelilahScene::setCursor - 
*/
void DelilahScene::setCursor(const char* cursorPath)
{
	if (cursorPath == NULL)
	{
		tabManager->sceneTab->view->setCursor(Qt::ArrowCursor);
		return;
	}

	QPixmap  pixmap(cursorPath);
	QCursor  cursor(pixmap, -1, -1);

	tabManager->sceneTab->view->setCursor(cursor);
}



/* ****************************************************************************
*
* DelilahScene::qCreate -
*/
void DelilahScene::qCreate(void)
{
	setCursor("images/queue32x32.png");

	removeRequested     = false;
	connectionRequested = false;
	selectedItem        = NULL;
	createRequested     = true;
}



const char* displayName = "cualquiera";
/* ****************************************************************************
*
* DelilahScene::bind - 
*/
void DelilahScene::bind(void)
{
	LM_TODO(("BIND queue '%s' to the next queue selected by mouse", displayName));
}



/* ****************************************************************************
*
* DelilahScene::rename - 
*/
void DelilahScene::rename(void)
{
	LM_TODO(("RENAME queue '%s' using a popup ... ?", displayName));
}



/* ****************************************************************************
*
* DelilahScene::remove - 
*/
void DelilahScene::remove(void)
{
    connectionRequested = false;
    createRequested     = false;
	removeRequested     = true;
	selectedItem        = NULL;
}



/* ****************************************************************************
*
* DelilahScene::remove2 - 
*/
void DelilahScene::remove2(void)
{
	setCursor("images/queueDelete32x32.png");
	remove();
}



/* ****************************************************************************
*
* DelilahScene::connection -
*/
void DelilahScene::connection(void)
{
	int queues = queueMgr->queues();

	removeRequested     = false;
	createRequested     = false;
	connectionRequested = false;
	selectedItem        = NULL;

	if (queues < 2)
	{
		new Popup("Not enough queues", "You cannot create a connection if you don't have\nat least two queues");
		return;
	}
	else if (queues == 2)
	{
		if (connectionMgr->connections() == 1)
			new Popup("Not enough queues", "You cannot create a connection if you don't have\nat least two (unconnected) queues");
		else
			connectionMgr->insert(this, queueMgr->lookup(1), queueMgr->lookup(2));
	}
	else
	{
		connectFrom         = NULL;
		connectionRequested = true;
		selectedItem        = NULL;

		setCursor("images/from.png");
		LM_T(LmtQueueConnection, ("Connection Requested"));
	}
}
