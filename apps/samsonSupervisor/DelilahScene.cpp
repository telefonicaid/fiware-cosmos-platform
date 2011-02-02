/* ****************************************************************************
*
* FILE                     DelilahScene.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 02 2011
*
*/
#include <stdio.h>             // printf

#include <QObject>
#include <QMenu>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>

#include "logMsg.h"            // LM_*
#include "traceLevels.h"       // Lmt*

#include "Popup.h"             // Popup
#include "misc.h"              // centerCoordinates
#include "DelilahScene.h"      // Own interface



/* ****************************************************************************
*
* popupMenu - 
*/
static QMenu*        popupMenu           = NULL;
static bool          removeRequested     = false;
static bool          connectionRequested = false;
static DelilahQueue* connectFrom         = NULL;
static DelilahQueue* connectTo           = NULL;



/* ****************************************************************************
*
* DelilahScene::DelilahScene - 
*/
DelilahScene::DelilahScene(QMenu* itemMenu, QObject* parent) : QGraphicsScene(parent)
{
	myItemMenu = itemMenu;

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

		return;
	}

	if (buttons == Qt::RightButton)
	{
		QGraphicsItem* item;
        DelilahQueue*  q;
		QAction*       bindAction;
		QAction*       renameAction;
		QAction*       deleteAction;

		point = mouseEvent->buttonDownScenePos(Qt::RightButton);
		item = itemAt(point);
		q    = lookup(item);

		if (item == NULL)
			return;

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
	else if (buttons == Qt::LeftButton)
	{
		DelilahQueue* q;

		point        = mouseEvent->buttonDownScenePos(Qt::LeftButton);
		selectedItem = itemAt(point);
		
		q = lookup(selectedItem);

		if (q != NULL)
		{
			LM_TODO(("Make '%s' the top-most item in item stack", q->displayName));

			if (connectionRequested)
			{
				if (connectFrom == NULL)
				{
					connectFrom = q;
					LM_T(LmtQueueConnection, ("Connecting '%s' ...", q->displayName));
				}
				else
				{
					connectTo = q;

					LM_T(LmtQueueConnection, ("Connecting '%s' with '%s'", connectFrom->displayName, q->displayName));

					connectFrom->neighbor = connectTo;
					connectTo->neighbor   = connectFrom;
					connectionDraw(connectFrom, connectTo);

					connectFrom         = NULL;
					connectTo           = NULL;
					connectionRequested = false;
				}
			}
			else if (removeRequested == true)
			{
				LM_T(LmtQueueConnection, ("removing queue %s ...", q->displayName));
				if (q->neighbor)
				{
					q->neighbor->neighbor       = NULL;
					q->neighbor->connectionLine = NULL;

					delete q->connectionLine;
				}

				delete q;

				if (q == testq1)
				   testq1 = NULL;
				else if (q == testq2)
				   testq2 = NULL;

				removeRequested = false;
			}
		}
		else
		{
			removeRequested = false;
		}
	}
}



/* ****************************************************************************
*
* DelilahScene::connectionDraw - 
*/
void DelilahScene::connectionDraw(DelilahQueue* qFromP, DelilahQueue* qToP)
{
	qreal   startX;
	qreal   startY;
	qreal   endX;
	qreal   endY;
	QPen    pen;

	pen.setWidth(3);

	centerCoordinates(qFromP->pixmapItem, &startX, &startY);
	centerCoordinates(qToP->pixmapItem,   &endX,   &endY);

	qFromP->connectionLine = addLine(startX, startY, endX, endY);
	qToP->connectionLine   = qFromP->connectionLine;

	qFromP->connectionLine->setPen(pen);

	qFromP->connectionLine->stackBefore(qFromP->pixmapItem);
	qFromP->connectionLine->stackBefore(qToP->pixmapItem);
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

	q = lookup(selectedItem);
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
* DelilahScene::qCreate -
*/
void DelilahScene::qCreate(void)
{
	DelilahQueue* q;

	if ((testq1 != NULL) && (testq2 != NULL))
	{
		new Popup("No more queues available", "The two test queues already created, sorry ...", false);
		return;
	}

	q = new DelilahQueue(this, "images/queue.png");

	if (testq1 == NULL)
		testq1 = q;
	else
		testq2 = q;
}



/* ****************************************************************************
*
* lookup - 
*/
DelilahQueue* DelilahScene::lookup(QGraphicsItem* gItemP)
{
	if (gItemP == NULL)
		return NULL;
	else if ((testq1 != NULL) && (gItemP == testq1->pixmapItem))
		return testq1;
	else if ((testq1 != NULL) && (gItemP == testq1->nameItem))
		return testq1;
	else if ((testq2 != NULL) && (gItemP == testq2->pixmapItem))
		return testq2;
	else if ((testq2 != NULL) && (gItemP == testq2->nameItem))
		return testq2;
	else
		return NULL;
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
	LM_TODO(("DELETE this very queue '%s' (if pressed in Queue-menu)", displayName));
	removeRequested = true;
}



/* ****************************************************************************
*
* DelilahScene::remove2 - 
*/
void DelilahScene::remove2(void)
{
	remove();
}



/* ****************************************************************************
*
* DelilahScene::connection -
*/
void DelilahScene::connection(void)
{
	connectFrom         = NULL;
	connectTo           = NULL;
	connectionRequested = true;

	LM_T(LmtQueueConnection, ("Connection Requested"));
}
