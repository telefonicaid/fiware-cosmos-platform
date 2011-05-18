/* ****************************************************************************
*
* FILE                     QsiDialog.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 18 2011
*
*/
#include "QsiManager.h"         // Manager
#include "QsiBox.h"             // Box
#include "QsiBlock.h"           // Block
#include "QsiDialog.h"          // Own interface



namespace Qsi
{



/* ****************************************************************************
*
* ok - 
*/
static void ok(Block* qbP, void* param)
{
	Dialog* dialog = (Dialog*) param;

	dialog->manager->box->remove(dialog, false);
	
	delete dialog;
}



/* ****************************************************************************
*
* Constructor - 
*/
Dialog::Dialog(Manager* _manager, const char* _title, bool modal) : Box(_manager, NULL, _title, 0, 0)
{
	int wx;
	int wy;
	int wwidth;
	int wheight;

	if (modal)
	{
		QPixmap  pixmap(manager->totalWidth, manager->totalHeight);

		pixmap.fill(QColor(0x0, 0x0, 0x0, 0xC0));
		background = manager->addPixmap(pixmap);
		background->setZValue(0.5);
	}
	else
		background = NULL;

	wx      = manager->winWidth  / 4;
	wy      = manager->winHeight / 4;
	wwidth  = manager->winWidth  / 2;
	wheight = manager->winHeight / 2;

	winBox = (Box*) boxAdd("dialogWinBox", wx, wy);
	
	shadow = (Block*) winBox->rectangleAdd("Shadow",   10, 10, wwidth, wheight, QColor(0x00, 0x00, 0x00, 0xFF), QColor(0x00, 0x00, 0x00, 0xFF), 0);
	win    = (Block*) winBox->rectangleAdd("DialogWin", 0,  0, wwidth, wheight, QColor(0xD0, 0xD0, 0xFF, 0xFF), QColor(0x00, 0x00, 0x20, 0xFF), 3);
	title  = (Block*) winBox->textAdd("DialogTitle", _title, 0, 0);

	int tx, ty, tw, th;
	title->geometry(&tx, &ty, &tw, &th);
	
	title->moveRelative((wwidth - tw) / 2, th + 5);
	
	if (modal)
		manager->grab(win->gItemP, ok, this);
	else
	{
		manager->box->add(this);
		manager->siConnect(win, ok, this);
	}

	shadow->gItemP->setZValue(0.6);
	win->gItemP->setZValue(0.65);
	title->gItemP->setZValue(0.7);
}



/* ****************************************************************************
*
* Destructor - 
*/
Dialog::~Dialog()
{
	if (background != NULL)
	{
		manager->ungrab(win->gItemP);
		delete background;
	}

	delete shadow;
	delete win;
	delete title;
}

}
