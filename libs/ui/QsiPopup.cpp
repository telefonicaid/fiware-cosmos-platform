/* ****************************************************************************
*
* FILE                     QsiPopup.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 17 2011
*
*/
#include <QPixmap>
#include <QColor>
#include <QGraphicsPixmapItem>
#include <QGraphicsSimpleTextItem>

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Lmt*

#include "QsiManager.h"         // Manager
#include "QsiPopup.h"           // Own interface



namespace Qsi
{



/* ****************************************************************************
*
* Popup - 
*/
Popup::Popup(Manager* manager, const char* title, const char* message)
{
	QGraphicsPixmapItem*      pixmapItem;
	QGraphicsSimpleTextItem*  titleItem;
	QGraphicsSimpleTextItem*  messageItem;
	QGraphicsRectItem*        win;

	LM_T(LmtPopup, ("Creating a pixmap with dimensions %dx%d", manager->totalWidth, manager->totalHeight));
	LM_T(LmtPopup, ("width: %d, height: %d", (int) manager->winWidth, (int) manager->winHeight));

	QPixmap  pixmap(manager->totalWidth, manager->totalHeight);

	pixmap.fill(QColor(0x0, 0x0, 0x0, 0xC0));

	win         = manager->addRect(manager->winWidth  / 4, 
								   manager->winHeight / 4,
								   manager->winWidth  / 2,
								   manager->winHeight / 2,
								   QPen(QColor(0, 0, 0, 0xFF)),
								   QBrush(QColor(0xD0, 0xD0, 0xFF, 0xFF)));

	pixmapItem  = manager->addPixmap(pixmap);
	titleItem   = manager->addSimpleText(title);
	messageItem = manager->addSimpleText(message);

	pixmapItem->setPos(0, 0);

	int tx, ty, tw, th;
	QRectF rect;

	rect = titleItem->boundingRect();
	tx   = rect.x();
	ty   = rect.y();
	tw   = rect.width();
	th   = rect.height();
	titleItem->setPos(manager->winWidth / 2 - tw / 2, manager->winHeight / 4 + th * 2);
	
	rect = messageItem->boundingRect();
	tx   = rect.x();
	ty   = rect.y();
	tw   = rect.width();
	th   = rect.height();
	messageItem->setPos(manager->winWidth / 2 - tw / 2, manager->winHeight / 4 + th * 5);
	
	pixmapItem->setZValue(0.5);
	win->setZValue(0.6);
	titleItem->setZValue(0.7);
	messageItem->setZValue(0.7);
}

}
