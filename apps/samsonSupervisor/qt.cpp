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
#include "ButtonActions.h"      // ButtonActions
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
* qtRun - 
*/
void qtRun(int argC, const char* argV[])
{
	QApplication     app(argC, (char**) argV);
	int              screenWidth;
	int              screenHeight;
	int              x;
	int              y;
	QWidget          window;
	QDesktopWidget*  desktop = QApplication::desktop();

	// Window Geometry
	screenWidth  = desktop->width();
	screenHeight = desktop->height();

	x = (screenWidth  - WIN_WIDTH)  / 2;
	y = (screenHeight - WIN_HEIGHT) / 2;

	window.resize(WIN_WIDTH, WIN_HEIGHT);
	window.move(x, y);
	window.setWindowTitle("Samson Supervisor");

	TabManager tabManager(&window);

	window.show();
	app.exec();
}


#if 0
/* ****************************************************************************
*
* qtRun2 - 
*/
void qtRun2(int argC, const char* argV[])
{
	QApplication     app(argC, (char**) argV);
	int              screenWidth;
	int              screenHeight;
	int              x;
	int              y;
	QWidget          window;
	QDesktopWidget*  desktop = QApplication::desktop();

	// Window Geometry
	screenWidth  = desktop->width();
	screenHeight = desktop->height();

	x = (screenWidth  - WIN_WIDTH)  / 2;
	y = (screenHeight - WIN_HEIGHT) / 2;

	window.resize(WIN_WIDTH, WIN_HEIGHT);
	window.move(x, y);
	window.setWindowTitle("Samson Supervisor");
	window.setWindowIcon(QIcon("web.png"));

	ButtonActions ba(&window);


	//
	// Spawner List
	//
	int        noOfSpawners;
	Spawner**  spawnerVec;

	spawnerVec = spawnerListGet(&noOfSpawners);

	ba.spawnerListCreate(spawnerVec, noOfSpawners);



	//
    // Process List
    //
	int        noOfProcesses;
	Process**  processVec;

	processVec = processListGet(&noOfProcesses);

	ba.processListCreate(processVec, noOfProcesses);

	window.show();

	app.exec();
}
#endif
