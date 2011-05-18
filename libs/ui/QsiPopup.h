#ifndef QSI_POPUP_H
#define QSI_POPUP_H

/* ****************************************************************************
*
* FILE                     QsiPopup.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 17 2011
*
*/
#include <QGraphicsRectItem>

#include "QsiManager.h"         // Manager



namespace Qsi
{



/* ****************************************************************************
*
* - 
*/
class Manager;



/* ****************************************************************************
*
* Popup - 
*/
class Popup
{
private:
	QGraphicsPixmapItem*      background;
	QGraphicsSimpleTextItem*  titleItem;
	QGraphicsSimpleTextItem*  messageItem;

public:
	Popup(Manager* manager, const char* _title, const char* message);
	~Popup();

	QGraphicsRectItem*  win;
	Manager*            manager;
};

}

#endif
