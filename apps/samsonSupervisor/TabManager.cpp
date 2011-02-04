/* ****************************************************************************
*
* FILE                     TabManager.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 14 2010
*
*/
#include <sys/time.h>           // getimeofday

#include <QtGui>
#include <QPushButton>
#include <QVBoxLayout>

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "globals.h"            // networkP, ...
#include "ProcessListTab.h"     // ProcessListTab
#include "LogTab.h"             // LogTab
#include "DelilahTab.h"         // DelilahTab
#include "starterList.h"        // starterListShow
#include "TabManager.h"         // Own interface



/* ****************************************************************************
*
* TabManager::TabManager - 
*/
TabManager::TabManager(QWidget* window, QWidget* parent) : QWidget(parent)
{
	QVBoxLayout*     mainLayout = new QVBoxLayout;
	QPushButton*     quit       = new QPushButton("Quit");

	connect(quit, SIGNAL(clicked()), qApp, SLOT(quit()));

	tabWidget       = new QTabWidget();

	processListTab  = new ProcessListTab("Processes", window);
	logTab          = new LogTab();
	delilahTab      = new DelilahTab("Console");
	sceneTab        = new SceneTab("Delilah");

	tabWidget->addTab(processListTab, tr("Processes"));
	tabWidget->addTab(logTab,         tr("Log"));
	tabWidget->addTab(delilahTab,     tr("Console"));
	tabWidget->addTab(sceneTab,       tr("Delilah"));

	mainLayout->addWidget(tabWidget);
	mainLayout->addWidget(quit);

	logTab->setDisabled(true);

	window->setLayout(mainLayout);

	LM_T(LmtQtTimer, ("Starting timer for Network polling"));
	startTimer(10);  // 10 millisecond timer	
}



/* ****************************************************************************
*
* TabManager::timerEvent - 
*/
void TabManager::timerEvent(QTimerEvent* e)
{
	static struct timeval  lastShow = { 0, 0 };
	struct timeval         now;

	gettimeofday(&now, NULL);
	if (now.tv_sec - lastShow.tv_sec > 3)
	{
		if (networkP != NULL)
			networkP->endpointListShow("periodic");

		starterListShow("periodic");

		lastShow = now;
	}

	if (networkP != NULL)
		networkP->poll();
}
