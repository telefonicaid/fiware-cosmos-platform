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
#include "globals.h"            // tabManager, ...

#include "TabManager.h"         // TabManager
#include "qt.h"                 // Own interface



/* ****************************************************************************
*
* Window geometry
*/
#define WIN_WIDTH   1000
#define WIN_HEIGHT   700



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



void buttonTest(void)
{
	QVBoxLayout* mainLayout = new QVBoxLayout;
	QPushButton* quit       = new QPushButton("Quit");

	QObject::connect(quit, SIGNAL(clicked()), qApp, SLOT(quit()));

	mainLayout->addWidget(quit);
	mainWindow->setLayout(mainLayout);
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

#if 0
	buttonTest();
#else
	tabManager = new TabManager(mainWindow);
#endif

	mainWindow->show();

	app.exec();
}
