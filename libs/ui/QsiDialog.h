#ifndef QSI_DIALOG_H
#define QSI_DIALOG_H

/* ****************************************************************************
*
* FILE                     QsiDialog.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 18 2011
*
*/
#include "QsiManager.h"         // Manager
#include "QsiBox.h"             // Box
#include "QsiBlock.h"           // Block



namespace Qsi
{



/* ****************************************************************************
*
* - 
*/
class Manager;



/* ****************************************************************************
*
* Dialog - 
*/
class Dialog : public Box
{
private:
	QGraphicsPixmapItem*  background;
	Block*                shadow;

public:
	Box*                  winBox;
	Block*                win;
	Block*                title;
	int                   borderWidth;
	int                   shadowX;
	int                   shadowY;

public:
	Dialog(Manager* _manager, const char* _title, bool modal, bool final = true);
	~Dialog();
};

}

#endif
