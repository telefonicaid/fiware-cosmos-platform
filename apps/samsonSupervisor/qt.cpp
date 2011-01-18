/* ****************************************************************************
*
* FILE                     qt.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 13 2011
*
*/
#include <QObject>
#include <QApplication>
#include <QWidget>
#include <QDesktopWidget>
#include <QIcon>
#include <QPushButton>

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // LMT_*
#include "globals.h"            // tabManager, ...

#include "TabManager.h"         // TabManager
#include "qt.h"                 // Own interface



/* ****************************************************************************
*
* Window geometry
*/
#define WIN_WIDTH    400
#define WIN_HEIGHT   600



/* ****************************************************************************
*
* mainWindow - 
*/
QWidget*         mainWindow = NULL;
QDesktopWidget*  desktop    = NULL;



/* ****************************************************************************
*
* mainWinCreate - 
*/
static void mainWinCreate(QApplication* app)
{
	int              screenWidth;
	int              screenHeight;
	int              x;
	int              y;

	desktop    = QApplication::desktop();
	mainWindow = new QWidget();

	// Window Geometry
	screenWidth  = desktop->width();
	screenHeight = desktop->height();

	x = (screenWidth  - WIN_WIDTH)  / 2;
	y = (screenHeight - WIN_HEIGHT) / 2;

	mainWindow->resize(WIN_WIDTH, WIN_HEIGHT);
	mainWindow->move(x, y);
	mainWindow->setWindowTitle("Samson Supervisor");
}



/* ****************************************************************************
*
* qtRun - 
*/
void qtRun(int argC, const char* argV[])
{
	QApplication app(argC, (char**) argV);

	LM_M(("I have %d workers", networkP->Workers));

	mainWinCreate(&app);

	
	LM_T(LMT_QT, ("Creating TabManager"));
	tabManager = new TabManager(mainWindow);
	LM_T(LMT_QT, ("tabManager at %p", tabManager));

	LM_T(LMT_QT, ("Showing main window"));
	mainWindow->show();

	LM_T(LMT_QT, ("running"));
	app.exec();
}
