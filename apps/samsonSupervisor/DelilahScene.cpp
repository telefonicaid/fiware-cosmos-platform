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
#include "DelilahSource.h"      // DelilahSource
#include "DelilahResult.h"      // DelilahResult
#include "DelilahConnection.h"  // DelilahConnection
#include "QueueConfigWindow.h"  // QueueConfigWindow
#include "SourceConfigWindow.h" // SourceConfigWindow
#include "ResultConfigWindow.h" // ResultConfigWindow
#include "ConfigTab.h"          // ConfigTab::Off, ...
#include "DelilahSceneItem.h"   // Own DelilahSceneItem
#include "DelilahScene.h"       // Own interface



/* ****************************************************************************
*
* global variables
*/
DelilahQueue*             menuQueue              = NULL;
DelilahSource*            menuSource             = NULL;
DelilahResult*            menuResult             = NULL;
DelilahConnection*        menuConnection         = NULL;
static bool               removeRequested        = false;
static bool               connectionRequested    = false;
static DelilahSceneItem*  connectFrom            = NULL;
static bool               queueCreateRequested   = false;
static bool               sourceCreateRequested  = false;
static bool               resultCreateRequested  = false;
static QGraphicsItem*     selectedItem           = NULL;
static DelilahSceneItem*  si                     = NULL;



/* ****************************************************************************
*
* DelilahScene::DelilahScene - 
*/
DelilahScene::DelilahScene(QObject* parent) : QGraphicsScene(parent)
{
	highestInStack = NULL;

	renameAction = new QAction(tr("R&ename"), this);
	renameAction->setStatusTip(tr("Rename the current queue"));
	connect(renameAction, SIGNAL(triggered()), this, SLOT(rename()));

	removeAction = new QAction(tr("&Remove"), this);
	removeAction->setStatusTip(tr("Remove the current queue"));
	connect(removeAction, SIGNAL(triggered()), this, SLOT(removeFromMenu()));

	chainRemoveAction = new QAction(tr("Remove Chain"), this);
	chainRemoveAction->setStatusTip(tr("Remove the entire chain"));
	connect(chainRemoveAction, SIGNAL(triggered()), this, SLOT(chainRemove()));

	configAction = new QAction(tr("&Config"), this);
	configAction->setStatusTip(tr("Configure current queue"));
	connect(configAction, SIGNAL(triggered()), this, SLOT(config()));

	bindAction = new QAction(tr("&Bind"), this);
	bindAction->setStatusTip(tr("Bind current queue"));
	connect(bindAction, SIGNAL(triggered()), this, SLOT(bind()));

	checkAction = new QAction(tr("&Check"), this);
	checkAction->setStatusTip(tr("Check this pipeline"));
	connect(checkAction, SIGNAL(triggered()), this, SLOT(check()));

	executeAction = new QAction(tr("&Execute"), this);
	executeAction->setStatusTip(tr("Execute current queue"));
	connect(executeAction, SIGNAL(triggered()), this, SLOT(execute()));

	disableAction = new QAction(tr("&Disable"), this);
	disableAction->setStatusTip(tr("Disable entire chain"));
	connect(disableAction, SIGNAL(triggered()), this, SLOT(disable()));
	disableAction->setText("Disable");

	emptyAction = new QAction(tr("&Empty"), this);
	emptyAction->setStatusTip(tr("Empty current queue"));
	connect(emptyAction, SIGNAL(triggered()), this, SLOT(empty()));

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
	DelilahSource*      src;
	DelilahResult*      res;

	if (buttons == Qt::RightButton)
		point = mouseEvent->buttonDownScenePos(Qt::RightButton);
	else if (buttons == Qt::MidButton)
		point = mouseEvent->buttonDownScenePos(Qt::MidButton);
	else if (buttons == Qt::LeftButton)
		point = mouseEvent->buttonDownScenePos(Qt::LeftButton);

	item  = itemAt(point);
	q     = queueMgr->lookup(item);
	c     = connectionMgr->lookup(item);
	src   = sourceMgr->lookup(item);
	res   = resultMgr->lookup(item);

	if (buttons == Qt::LeftButton)
	{
		menuQueue       = NULL;
		menuConnection  = NULL;
		menuSource      = NULL;
		menuResult      = NULL;

		if (q != NULL)
			menuQueue = q;
		else if (c != NULL)
			menuConnection = c;
		else if (src != NULL)
			menuSource = src;
		else if (res != NULL)
			menuResult = res;
		else
			LM_RVE(("Double click on canvas/scene"));

		config();
	}
}



/* ****************************************************************************
*
* DelilahScene::mousePressEvent - 
*/
void DelilahScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
	Qt::MouseButtons    buttons = mouseEvent->buttons();
	QPointF             point;
	QGraphicsItem*      item;
	DelilahQueue*       q;
	DelilahConnection*  c;
	DelilahSource*      s;
	DelilahResult*      r;

	if (buttons == Qt::RightButton)
	{
		point = mouseEvent->buttonDownScenePos(Qt::RightButton);
		item  = itemAt(point);
		q     = queueMgr->lookup(item);
		c     = connectionMgr->lookup(item);
		s     = sourceMgr->lookup(item);
		r     = resultMgr->lookup(item);
		si    = NULL;

		if (q)      si = q;
		else if (s) si = s;
		else if (r) si = r;

		if (q)
			LM_T(LmtMouseEvent, ("Right pressed QUEUE '%s'", q->displayName));
		else if (c)
			LM_T(LmtMouseEvent, ("Right pressed CONNECTION '%s' -> '%s'", c->qToP->displayName, c->qFromP->displayName));
		else if (s)
			LM_T(LmtMouseEvent, ("Right pressed SOURCE '%s'", s->displayName));
		else if (r)
			LM_T(LmtMouseEvent, ("Right pressed RESULT '%s'", r->displayName));
		else
			LM_T(LmtMouseEvent, ("Right pressed CANVAS/SCENE"));
	}
	else if (buttons == Qt::MidButton)
	{
		point         = mouseEvent->buttonDownScenePos(Qt::MidButton);
		selectedItem  = itemAt(point);
		q             = queueMgr->lookup(selectedItem);
		c             = connectionMgr->lookup(selectedItem);
		s             = sourceMgr->lookup(selectedItem);
		r             = resultMgr->lookup(selectedItem);
		si            = NULL;

		if (q)      si = q;
		else if (s) si = s;
		else if (r) si = r;
	}
	else if (buttons == Qt::LeftButton)
	{
		point        = mouseEvent->buttonDownScenePos(Qt::LeftButton);
		selectedItem = itemAt(point);

		q  = queueMgr->lookup(selectedItem);
		c  = connectionMgr->lookup(selectedItem);
		s  = sourceMgr->lookup(selectedItem);
		r  = resultMgr->lookup(selectedItem);
		si = NULL;

		if (q)      si = q;
		else if (s) si = s;
		else if (r) si = r;
		
		if (si && c == NULL)
		{
			if (highestInStack)
			{
				qreal z;

				z = highestInStack->pixmapItem->zValue();
				z += 0.01;
				si->pixmapItem->setZValue(z);
				si->nameItem->setZValue(z);

				si->nameItem->update();
				si->pixmapItem->update();

				highestInStack->pixmapItem->update();
				highestInStack->nameItem->update();
			}

			highestInStack = si;
		}

		if (queueCreateRequested)
		{
			q = new DelilahQueue(this, "images/queue.png", NULL, point.x() - 64, point.y() - 64);
			queueMgr->insert(q);			
		}
		else if (resultCreateRequested)
		{
			r = new DelilahResult(this, "images/Result.png", NULL, point.x() - 64, point.y() - 64);
			resultMgr->insert(r);
		}
		else if (sourceCreateRequested)
		{
			s = new DelilahSource(this, "images/Bomba.png", NULL, point.x() - 64, point.y() - 64);
			sourceMgr->insert(s);
		}
		else if (si != NULL)
		{
			LM_TODO(("Make '%s' the top-most item in item stack", q->displayName));

			if (connectionRequested)
			{
				if (connectFrom == NULL)
				{
					LM_T(LmtConnection, ("Connecting '%s' ...", si->displayName));
					if (si->type == DelilahSceneItem::Result)
					{
						new Popup("Bad Item", "Result items doesn't give any output");
						connectionRequested = false;
					}
					else
					{
						connectFrom = si;
						setCursor("images/to.png");
					}
				}
				else
				{
					LM_T(LmtConnection, ("Connecting '%s' with '%s'", connectFrom->displayName, si->displayName));
					if (si->type == DelilahSceneItem::Source)
						new Popup("Bad Item", "Sources cannot take input");
					else
					{
						if ((tabManager->configTab->typeCheck == ConfigTab::Off) || (strcmp(connectFrom->outType, si->inType) == 0))
							connectionMgr->insert(this, connectFrom, si);
						else
						{
							char eText[256];

							if (tabManager->configTab->typeCheck == ConfigTab::On)
							{
								snprintf(eText, sizeof(eText), "Cannot connect these two items:\n- '%s' with out-type '%s'\n- '%s' with in-type '%s'\nYou'll need to alter one of these types ...",
										 connectFrom->displayName, connectFrom->outType,
										 si->displayName,          si->inType);
								new Popup("Uncompatible Items", eText);
							}
							else if (tabManager->configTab->typeCheck == ConfigTab::Left)
							{
								LM_W(("Types differ - I choose the out-type of the left in the connection (%s)", connectFrom->outType));
								si->inTypeSet(connectFrom->outType, connectFrom->outTypeIndex);
								connectionMgr->insert(this, connectFrom, si);
							}
							else if (tabManager->configTab->typeCheck == ConfigTab::Popup)
							{
								char eText[256];

								snprintf(eText, sizeof(eText), "I must implement a POPUP to choose between\n%s's out-type '%s' and\n%s's in-type '%s'.",
										 connectFrom->displayName, connectFrom->outType, si->displayName, si->inType);

								new Popup("Not implemented", eText);
								// new TypeDiffWindow()
								// connectionMgr->insert(this, connectFrom, si);
							}
						}
					}

					connectionRequested = false;
					connectFrom = NULL;
				}
			}
			else if (removeRequested == true)
			{
				LM_T(LmtConnection, ("removing queue %s (but first its connections) ...", si->displayName));

				connectionMgr->remove(si);
				if (q) queueMgr->remove(q);
				if (r) resultMgr->remove(r);
				if (s) sourceMgr->remove(s);
			}
		}
		else if (c != NULL)
		{
			if (removeRequested == true)
			{
				LM_T(LmtConnection, ("removing connection '%s' -> '%s'", c->qFromP->displayName, c->qToP->displayName));

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
		queueCreateRequested   = false;
		sourceCreateRequested  = false;
		resultCreateRequested  = false;
		removeRequested        = false;
	}
}



/* ****************************************************************************
*
* DelilahScene::mouseMoveEvent - 
*/
void DelilahScene::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
	DelilahQueue*      q;
	DelilahResult*     r;
	DelilahSource*     s;
	Qt::MouseButtons   buttons    = mouseEvent->buttons();
	QPointF            lastPoint  = mouseEvent->lastScenePos();
	QPointF            point      = mouseEvent->scenePos();

	if (selectedItem == NULL)
	{
		LM_T(LmtSceneItemChain, ("Sorry, no selected item ..."));
		return;
	}

	q  = queueMgr->lookup(selectedItem);
	r  = resultMgr->lookup(selectedItem);
	s  = sourceMgr->lookup(selectedItem);
	si = NULL;

	if (q)      si = q;
	else if (s) si = s;
	else if (r) si = r;

	if (buttons == (Qt::MidButton | Qt::LeftButton))
	{
		queueMgr->move(point.x() - lastPoint.x(), point.y() - lastPoint.y());
		resultMgr->move(point.x() - lastPoint.x(), point.y() - lastPoint.y());
		sourceMgr->move(point.x() - lastPoint.x(), point.y() - lastPoint.y());
	}
	else if (buttons == Qt::MidButton)
	{
		LM_T(LmtSceneItemChain, ("Chain Moving from '%s'", si->displayName));

		queueMgr->markMoved(false);
		resultMgr->markMoved(false);
		sourceMgr->markMoved(false);

		si->chainMove(point.x() - lastPoint.x(), point.y() - lastPoint.y());
	}
	else if (buttons == Qt::LeftButton)
	{
		if (q != NULL)
		{
			LM_T(LmtMove, ("Calling moveTo for '%s'", q->displayName));
			q->moveTo(point.x() - lastPoint.x(), point.y() - lastPoint.y());
		}
		else if (r != NULL)
		{
			LM_T(LmtMove, ("Calling moveTo for '%s'", r->displayName));
			r->moveTo(point.x() - lastPoint.x(), point.y() - lastPoint.y());
		}
		else if (s != NULL)
		{
			LM_T(LmtMove, ("Calling moveTo for '%s'", s->displayName));
			s->moveTo(point.x() - lastPoint.x(), point.y() - lastPoint.y());
		}
		else if (si != NULL)
			LM_T(LmtMove, ("Not calling moveTo for '%s'", si->displayName));
		else
			LM_T(LmtMove, ("Not calling moveTo for unknown SceneItem"));
	}
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

	removeRequested        = false;
	connectionRequested    = false;
	selectedItem           = NULL;
	sourceCreateRequested  = false;
	resultCreateRequested  = false;
	queueCreateRequested   = true;
}



/* ****************************************************************************
*
* DelilahScene::source -
*/
void DelilahScene::source(void)
{
	setCursor("images/Bomba32x29.png");

	removeRequested        = false;
	connectionRequested    = false;
	selectedItem           = NULL;
	queueCreateRequested   = false;
	sourceCreateRequested  = true;
	resultCreateRequested  = false;
}



/* ****************************************************************************
*
* DelilahScene::result -
*/
void DelilahScene::result(void)
{
	setCursor("images/Result32x22.png");

	removeRequested        = false;
	connectionRequested    = false;
	selectedItem           = NULL;
	queueCreateRequested   = false;
	sourceCreateRequested  = false;
	resultCreateRequested  = true;
}



const char* displayName = "cualquiera";
/* ****************************************************************************
*
* DelilahScene::remove - 
*/
void DelilahScene::remove(void)
{
	setCursor("images/queueDelete32x32.png");

	removeRequested        = true;
    connectionRequested    = false;
    queueCreateRequested   = false;
	sourceCreateRequested  = false;
	resultCreateRequested  = false;
	selectedItem           = NULL;
}



/* ****************************************************************************
*
* DelilahScene::chainRemove - 
*/
void DelilahScene::chainRemove(void)
{
	LM_T(LmtSceneItemChain, ("Remove Chain"));
	if (menuSource != NULL)
		menuSource->chainRemove();
	else
		LM_T(LmtSceneItemChain, ("menuSource == NULL"));
}



/* ****************************************************************************
*
* DelilahScene::connection -
*/
void DelilahScene::connection(void)
{
	int queues       = queueMgr->queues();
	int sources      = sourceMgr->sources();
	int results      = resultMgr->results();
	int connections  = connectionMgr->connections();
	int sceneItems   = queues + sources + results;

	LM_T(LmtConnection, ("We have %d queues, %d sources and %d results (%d scene items and %d connections)", queues, sources, results, sceneItems, connections));

	removeRequested        = false;
	queueCreateRequested   = false;
	sourceCreateRequested  = false;
	resultCreateRequested  = false;
	connectionRequested    = false;
	selectedItem           = NULL;

	if (sceneItems < 2)
	{
		new Popup("Not enough items", "You cannot create a connection if you don't have\nat least two items queue/source/result");
		return;
	}
	else if ((sceneItems == 2) && connections == 1)
	{
		new Popup("Not enough items", "You cannot create a connection if you don't have\nat least two (unconnected) items");
		return;
	}
	else
	{
		connectFrom         = NULL;
		connectionRequested = true;
		selectedItem        = NULL;

		setCursor("images/from.png");
		LM_T(LmtConnection, ("Connection Requested"));
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
	menuSource     = sourceMgr->lookup(item);
	menuResult     = resultMgr->lookup(item);
	menuConnection = connectionMgr->lookup(item);

	if (menuSource != NULL)
	{
		menu.addAction(renameAction);
		menu.addAction(removeAction);
		menu.addAction(chainRemoveAction);
		menu.addAction(configAction);
		menu.addAction(bindAction);
		menu.addAction(checkAction);
		menu.addAction(executeAction);
		menu.addAction(disableAction);
	}
	else if (menuQueue != NULL)
	{
		menu.addAction(renameAction);
		menu.addAction(removeAction);
		menu.addAction(configAction);
		menu.addAction(bindAction);
		menu.addAction(emptyAction);
		menu.addAction(disableAction);
	}
	else if (menuResult != NULL)
	{
		menu.addAction(renameAction);
		menu.addAction(removeAction);
		menu.addAction(configAction);
		menu.addAction(disableAction);
	}
	else if (menuConnection != NULL)
	{
		menu.addAction(removeAction);
	}
	else
	{
		LM_E(("item: %p", item));
		menu.addAction(clearAction);
		menu.addAction(queueAddAction);
		menu.addAction(aboutAction);
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
	sourceMgr->removeAll();
	resultMgr->removeAll();
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
    else if (menuSource != NULL)
	{
		LM_T(LmtSource, ("REMOVE source '%s'", menuSource->displayName));
		connectionMgr->remove(menuSource);
		sourceMgr->remove(menuSource);
	}
	else if (menuResult != NULL)
	{
		LM_T(LmtResult, ("REMOVE result '%s'", menuResult->displayName));
		connectionMgr->remove(menuResult);
		resultMgr->remove(menuResult);
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
	else if (menuSource != NULL)
		new SourceConfigWindow(menuSource);
	else if (menuResult != NULL)
		new ResultConfigWindow(menuResult);
	else if (menuConnection != NULL)
		LM_W(("Implement the Connection Config Window!"));
	else
		LM_W(("No menu item active - this is a bug!"));
}



/* ****************************************************************************
*
* DelilahScene::config - 
*/
void DelilahScene::config(DelilahSceneItem* itemP)
{
	if (itemP->type == DelilahSceneItem::Queue)
		new QueueConfigWindow((DelilahQueue*) itemP);
	else if (itemP->type == DelilahSceneItem::Source)
		new SourceConfigWindow((DelilahSource*) itemP);
	else if (itemP->type == DelilahSceneItem::Result)
		new ResultConfigWindow((DelilahResult*) itemP);
	else
		LM_W(("BUG!"));
}



/* ****************************************************************************
*
* DelilahScene::empty - 
*/
void DelilahScene::empty(void)
{
	if (menuQueue != NULL)
	   LM_W(("Empty queue '%s'", menuQueue->displayName));
}



/* ****************************************************************************
*
* DelilahScene::bind - 
*/
void DelilahScene::bind(void)
{
	if (menuQueue != NULL)
		connectFrom = menuQueue;
    else if (menuSource != NULL)
		connectFrom = menuSource;
    else if (menuResult != NULL)
		connectFrom = menuResult;
	else
		LM_RVE(("No bind-source item found ..."));

	LM_T(LmtConnection, ("BIND '%s' to the next item selected by mouse", si->displayName));

	connectionRequested = true;

	setCursor("images/to.png");
}



/* ****************************************************************************
*
* DelilahScene::check - 
*/
void DelilahScene::check(void)
{
	if (menuSource == NULL)
		LM_RVE(("Not a Source item"));

	LM_TODO(("Check pipeline starting at source '%s'", menuSource->displayName));
}



/* ****************************************************************************
*
* DelilahScene::execute - 
*/
void DelilahScene::execute(void)
{
	if (menuSource == NULL)
		LM_RVE(("Not a Source item"));

	LM_TODO(("Execute pipeline starting at source '%s'", menuSource->displayName));
}



/* ****************************************************************************
*
* DelilahScene::disable - 
*/
void DelilahScene::disable(void)
{
	if (si)
		si->disable();
}



/* ****************************************************************************
*
* DelilahScene::rename - 
*/
void DelilahScene::rename(void)
{
	bool    ok;

	if (menuQueue != NULL)
	{
		QString text = QInputDialog::getText(NULL, "Renaming a Queue",
											 "Queue Name:", QLineEdit::Normal,
											 menuQueue->displayName, &ok);
		if (ok && !text.isEmpty())
			menuQueue->displayNameSet(text.toStdString().c_str());
	}
	else if (menuConnection != NULL)
	{
		LM_W(("Can Connections be renamed ?"));
	}
	else if (menuSource != NULL)
    {
		QString text = QInputDialog::getText(NULL, "Renaming a Source",
											 "Source Name:", QLineEdit::Normal,
											 menuSource->displayName, &ok);
		if (ok && !text.isEmpty())
			menuSource->displayNameSet(text.toStdString().c_str());
    }
	else if (menuResult != NULL)
    {
		QString text = QInputDialog::getText(NULL, "Renaming a Result",
											 "Result Name:", QLineEdit::Normal,
											 menuResult->displayName, &ok);
		if (ok && !text.isEmpty())
			menuResult->displayNameSet(text.toStdString().c_str());
    }
	else
		LM_W(("No item selected for RENAME ..."));
}
