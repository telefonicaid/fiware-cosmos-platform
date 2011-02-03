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
	
	pixmapItem->moveBy(xpos, ypos);

	QRectF nameRect = nameItem->boundingRect();
	qreal  rx;
	qreal  ry;
	qreal  rwidth;
	qreal  rheight;

	nameRect.getRect(&rx, &ry, &rwidth, &rheight);

	nameItem->moveBy(xpos + 128/2 - ((int) rwidth)/2, ypos + 20);

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
