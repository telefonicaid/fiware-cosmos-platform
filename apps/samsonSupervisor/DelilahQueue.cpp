/* ****************************************************************************
*
* FILE                     DelilahQueue.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 02 2011
*
*/
#include <stdio.h>             // snprintf

#include <QPixmap>
#include <QString>
#include <QGraphicsPixmapItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsLineItem>
#include <QMenu>
#include <QAction>

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "globals.h"            // connectionMgr, ...
#include "misc.h"               // centerCoordinates
#include "DelilahScene.h"       // DelilahScene
#include "DelilahQueue.h"       // Own interface



/* ****************************************************************************
*
* qNo - 
*/
static int qNo = 1;



/* ****************************************************************************
*
* DelilahQueue::DelilahQueue - 
*/
DelilahQueue::~DelilahQueue()
{
	if (displayName)     delete displayName;
	if (name)            delete name;
	if (pixmap)          delete pixmap;
	if (nameItem)        delete nameItem;
	if (pixmapItem)      delete pixmapItem;
}



/* ****************************************************************************
*
* DelilahQueue::DelilahQueue - 
*/
DelilahQueue::DelilahQueue(DelilahScene* sceneP, const char* imagePath, const char* displayNameP, int x, int y)
{
	char qName[32];

	snprintf(qName, sizeof(qName), "Queue %d", qNo);

	if (displayNameP == NULL)
		displayNameP = qName;

	
	scene       = sceneP;
	name        = strdup(qName);
	displayName = strdup(displayNameP);

	pixmap      = new QPixmap(imagePath);

	xpos        = (x == 0)? qNo * 100 : x;
	ypos        = (y == 0)? qNo *  50 : y;

	neighbor    = NULL;
	
	pixmapItem  = scene->addPixmap(*pixmap);
	nameItem    = scene->addSimpleText(QString(displayName));
	
	moveTo(xpos, ypos);

	// Center name ...
	int      xdiff;
	qreal    queueWidth;
	qreal    nameWidth;
	QRectF   rect;
	QPointF  point;
	qreal    rx;
	qreal    ry;
	qreal    height;

	rect = pixmapItem->boundingRect();
	rect.getRect(&rx, &ry, &queueWidth, &height);

	rect = nameItem->boundingRect();
	rect.getRect(&rx, &ry, &nameWidth, &height);
	

	xdiff = queueWidth/2 - nameWidth/2;

	nameItem->moveBy(xdiff, 20);

	++qNo;
}



/* ****************************************************************************
*
* moveTo - 
*/
void DelilahQueue::moveTo(int x, int y)
{
	pixmapItem->moveBy(x, y);
	nameItem->moveBy(x, y);

	connectionMgr->move(this);
}
