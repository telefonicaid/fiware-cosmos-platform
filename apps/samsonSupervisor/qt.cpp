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
#include <QTextEdit>
#include <QSize>
#include <QVBoxLayout>
#include <QListWidget>
#include <QLabel>

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels
#include "globals.h"            // tabManager, ...

#include "iomMsgRead.h"         // iomMsgRead
#include "TabManager.h"         // TabManager
#include "qt.h"                 // Own interface



/* ****************************************************************************
*
* Window geometry
*/
#define MAIN_WIN_WIDTH     400
#define MAIN_WIN_HEIGHT    600



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

	x = (screenWidth  - MAIN_WIN_WIDTH)  / 2;
	y = (screenHeight - MAIN_WIN_HEIGHT) / 2;

	mainWindow->resize(MAIN_WIN_WIDTH, MAIN_WIN_HEIGHT);
	mainWindow->move(x, y);
	mainWindow->setWindowTitle("Samson Supervisor");
}



/* ****************************************************************************
*
* qtRun - 
*/
void qtRun(void)
{
}
