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
#include <QMessageBox>
#include <QInputDialog>
#include <QDir>

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Lmt*

#include "globals.h"            // connectionMgr, ...
#include "Popup.h"              // Popup
#include "misc.h"               // centerCoordinates
#include "DelilahQueue.h"       // DelilahQueue
#include "DelilahConnection.h"  // DelilahConnection
#include "QueueConfigWindow.h"  // QueueConfigWindow
#include "DelilahScene.h"       // Own interface



/* ****************************************************************************
*
* global variables
*/
DelilahQueue*          menuQueue           = NULL;
DelilahConnection*     menuConnection      = NULL;
static bool            removeRequested     = false;
static bool            connectionRequested = false;
static DelilahQueue*   connectFrom         = NULL;
static bool            createRequested     = false;
static QGraphicsItem*  selectedItem        = NULL;



/* ****************************************************************************
*
* DelilahScene::DelilahScene - 
*/
DelilahScene::DelilahScene(QObject* parent) : QGraphicsScene(parent)
{
	renameAction = new QAction(tr("R&ename"), this);
	renameAction->setStatusTip(tr("Rename the current queue"));
	connect(renameAction, SIGNAL(triggered()), this, SLOT(rename()));

	removeAction = new QAction(tr("&Remove"), this);
	removeAction->setStatusTip(tr("Remove the current queue"));
	connect(removeAction, SIGNAL(triggered()), this, SLOT(removeFromMenu()));

	configAction = new QAction(tr("&Config"), this);
	configAction->setStatusTip(tr("Configure current queue"));
	connect(configAction, SIGNAL(triggered()), this, SLOT(config()));

	bindAction = new QAction(tr("&Bind"), this);
	bindAction->setStatusTip(tr("Bind current queue"));
	connect(bindAction, SIGNAL(triggered()), this, SLOT(bind()));

	clearAction = new QAction(tr("Remove &All"), this);
	clearAction->setStatusTip(tr("Clear the screen - removing all queues and connections"));
	connect(clearAction, SIGNAL(triggered()), this, SLOT(clear()));

	queueAddAction = new QAction(tr("&Add Queue"), this);
	queueAddAction->setStatusTip(tr("Add a new queue"));
	connect(queueAddAction, SIGNAL(triggered()), this, SLOT(queueAdd()));

	aboutAction = new QAction(tr("&About"), this);
	aboutAction->setStatusTip(tr("About this application"));
	connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
}



/* ****************************************************************************
*
* DelilahScene::mouseDoubleClickEvent - 
*/
void DelilahScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
	Qt::MouseButtons    buttons = mouseEvent->buttons();
	QPointF             point;
	QGraphicsItem*      item;
	DelilahQueue*       q;
	DelilahConnection*  c;

	if (buttons == Qt::RightButton)
		point = mouseEvent->buttonDownScenePos(Qt::RightButton);
	else if (buttons == Qt::MidButton)
		point = mouseEvent->buttonDownScenePos(Qt::MidButton);
	else if (buttons == Qt::LeftButton)
		point = mouseEvent->buttonDownScenePos(Qt::LeftButton);

	item  = itemAt(point);
	q     = queueMgr->lookup(item);
	c     = connectionMgr->lookup(item);

	if ((q != NULL) && (buttons == Qt::LeftButton))
	{
		menuQueue = q;
		DelilahScene::config();
	}
}



/* ****************************************************************************
*
* DelilahScene::mousePressEvent - 
*/
void DelilahScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
	Qt::MouseButtons  buttons = mouseEvent->buttons();
	QPointF           point;

	if (buttons == Qt::RightButton)
	{
		QGraphicsItem*      item;
        DelilahQueue*       q;
		DelilahConnection*  c;

		point = mouseEvent->buttonDownScenePos(Qt::RightButton);
		item  = itemAt(point);
		q     = queueMgr->lookup(item);
		c     = connectionMgr->lookup(item);

		if (q)
			LM_T(LmtMouseEvent, ("Right pressed queue '%s'", q->displayName));
		else if (c)
			LM_T(LmtMouseEvent, ("Right pressed connection '%s' -> '%s'", c->qToP->displayName, c->qFromP->displayName));
		else
			LM_T(LmtMouseEvent, ("Right pressed canvas"));
	}
	else if (buttons == Qt::MidButton)
	{
		
	}
	else if (buttons == Qt::LeftButton)
	{
		DelilahQueue*      q = NULL;
		DelilahConnection* c = NULL;

		point        = mouseEvent->buttonDownScenePos(Qt::LeftButton);
		selectedItem = itemAt(point);
		
		q = queueMgr->lookup(selectedItem);
		c = connectionMgr->lookup(selectedItem);

#if 0
		if (q)
			q->pixmap->raise();
#else
		LM_TODO(("Raise the selected item (if Queue) in the stacking"));
#endif

		if (createRequested)
		{
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
	DelilahQueue*     q;
	Qt::MouseButtons  buttons = mouseEvent->buttons();

    if (buttons == Qt::MidButton)
	{
		QPointF lastPoint = mouseEvent->lastScenePos();
		QPointF point     = mouseEvent->scenePos();

		queueMgr->move(point.x() - lastPoint.x(), point.y() - lastPoint.y());
	}

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
* DelilahScene::remove - 
*/
void DelilahScene::remove(void)
{
	setCursor("images/queueDelete32x32.png");

    connectionRequested = false;
    createRequested     = false;
	removeRequested     = true;
	selectedItem        = NULL;
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



/* ****************************************************************************
*
* DelilahScene::contextMenuEvent - 
*/
void DelilahScene::contextMenuEvent(QGraphicsSceneContextMenuEvent* contextMenuEvent)
{
	QPointF             scenePos;
	QPoint              screenPos;
	QMenu               menu("Scene Menu");
	QGraphicsItem*      item;

	screenPos = contextMenuEvent->screenPos();
	scenePos  = contextMenuEvent->scenePos();

	item           = itemAt(scenePos);
	menuQueue      = queueMgr->lookup(item);
	menuConnection = connectionMgr->lookup(item);

	if (item == NULL)
	{
		menu.addAction(clearAction);
		menu.addAction(queueAddAction);
		menu.addAction(aboutAction);
	}
	else if (menuQueue != NULL)
	{
		menu.addAction(renameAction);
		menu.addAction(removeAction);
		menu.addAction(configAction);
		menu.addAction(bindAction);
	}
	else if (menuConnection != NULL)
	{
		menu.addAction(removeAction);
	}

	menu.exec(screenPos);
}



/* ****************************************************************************
*
* DelilahScene::about - 
*/
void DelilahScene::about(void)
{
	QMessageBox::about(NULL, "Delilah",
					   "The <b>Delilah</b> executable\nStarts, Stops & Configures the Samson platform.");
}



/* ****************************************************************************
*
* DelilahScene::clear - 
*/
void DelilahScene::clear(void)
{
	queueMgr->removeAll();
	connectionMgr->removeAll();	
}



/* ****************************************************************************
*
* DelilahScene::queueAdd - 
*/
void DelilahScene::queueAdd(void)
{
	qCreate();
}



/* ****************************************************************************
*
* DelilahScene::removeFromMenu - 
*/
void DelilahScene::removeFromMenu(void)
{
	if (menuQueue != NULL)
	{
		LM_T(LmtQueue, ("REMOVE queue '%s'", menuQueue->displayName));
		connectionMgr->remove(menuQueue);
		queueMgr->remove(menuQueue);
	}
	else if (menuConnection != NULL)
	{
		LM_T(LmtQueue, ("REMOVE connection '%s' -> '%s'", menuConnection->qFromP->displayName, menuConnection->qToP->displayName));
		connectionMgr->remove(menuConnection);
	}
	else
		LM_W(("No menu item active - this is a bug!"));
}



/* ****************************************************************************
*
* DelilahScene::config - 
*/
void DelilahScene::config(void)
{
	if (menuQueue != NULL)
		new QueueConfigWindow(menuQueue);
	else
		LM_W(("No menu item active - this is a bug!"));
}



/* ****************************************************************************
*
* DelilahScene::bind - 
*/
void DelilahScene::bind(void)
{
	if (menuQueue == NULL)
		LM_RVE(("No menu queue"));

	LM_T(LmtQueueConnection, ("BIND queue '%s' to the next queue selected by mouse", menuQueue->displayName));

	connectFrom         = menuQueue;
	connectionRequested = true;

	setCursor("images/to.png");
}



/* ****************************************************************************
*
* DelilahScene::rename - 
*/
void DelilahScene::rename(void)
{
	if (menuQueue == NULL)
		LM_RVE(("No menu queue"));

	bool    ok;
	QString text = QInputDialog::getText(NULL, "Renaming a Queue",
										 "Queue Name:", QLineEdit::Normal,
										 menuQueue->displayName, &ok);

	if (ok && !text.isEmpty())
		menuQueue->displayNameSet(text.toStdString().c_str());
}
