/* ****************************************************************************
*
* FILE                     InfoWin.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 12 2011
*
*/
#include <time.h>               // struct timeval
#include <sys/time.h>           // gettimeofday

#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QSize>
#include <QDesktopWidget>
#include <QPlainTextEdit>
#include <QListWidget>

#include "logMsg.h"             // LM_X, ...
#include "globals.h"            // qtAppRunning, ...
#include "InfoWin.h"            // Own interface



/* ****************************************************************************
*
* InfoWin::InfoWin - 
*/
InfoWin::InfoWin(const char* title, const char* text, unsigned int secs, unsigned int usecs)
{
	QVBoxLayout*       layout;
	QLabel*            label      = NULL;

	QSize              size;
	int                screenWidth;
	int                screenHeight;
	int                x;
	int                y;
	QDesktopWidget*    desktop = QApplication::desktop();

	if (usecs >= 1000000)
		LM_X(1, ("Bad paramater - usecs == %d"));

	setModal(false);
	layout = new QVBoxLayout();

	label = new QLabel(text);
	layout->addWidget(label);

	setWindowTitle(title);

	this->setLayout(layout);
	this->show();

	// Window Geometry
	size = this->size();

	screenWidth  = desktop->width();
	screenHeight = desktop->height();

	x = (screenWidth  - size.width())  / 2;
	y = (screenHeight - size.height()) / 2;

	this->move(x, y);

	if (qtAppRunning == false)
		qApp->exec();

	infoWin = this;
	gettimeofday(&bornAt, NULL);

	dieAt = bornAt;
	dieAt.tv_sec  += secs;
	dieAt.tv_usec += usecs;
	if (dieAt.tv_usec > 1000000)
	{
		dieAt.tv_sec  += 1;
		dieAt.tv_usec -= 1000000;
	}
}
