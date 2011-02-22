/* ****************************************************************************
*
* FILE                     TabManager.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 14 2010
*
*/
#include <unistd.h>             // close
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <QtGui>
#include <QPushButton>
#include <QVBoxLayout>

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "globals.h"            // networkP, ...
#include "samson/Log.h"			// LogLineData
#include "permissions.h"        // UpSeeLogs, ...
#include "Popup.h"              // Popup
#include "ProcessListTab.h"     // ProcessListTab
#include "MrOperationsTab.h"    // MrOperationsTab
#include "LogTab.h"             // LogTab
#include "DelilahRawTab.h"      // DelilahRawTab
#include "ConfigTab.h"          // ConfigTab
#include "OldMrOperationsTab.h" // OldMrOperationsTab
#include "starterList.h"        // starterListShow
#include "processList.h"        // processListShow
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
	mrOperationsTab = new MrOperationsTab("MR Operations");
	logTab          = new LogTab();
	delilahRawTab   = new DelilahRawTab("Console");
	configTab       = new ConfigTab();
	delilahOldTab   = new OldMrOperationsTab("Old Delilah");

	tabWidget->addTab(processListTab,      tr("Processes"));
	tabWidget->addTab(mrOperationsTab,     tr("MR Operations"));
	tabWidget->addTab(logTab,              tr("Logging"));
	tabWidget->addTab(delilahRawTab,       tr("Raw Platform access"));
	tabWidget->addTab(configTab,           tr("Preferences"));
	tabWidget->addTab(delilahOldTab,       tr("Old MR Operations"));

	mainLayout->addWidget(tabWidget);
	mainLayout->addWidget(quit);

	if ((userP == NULL) || ((userP->permissions & UpSeeLogs) == 0))
	{
		LM_W(("disabling Log Tab (user: %p)", userP));
		logTab->setDisabled(true);
	}
	if ((userP == NULL) || ((userP->permissions & UpRawPlatformAccess) == 0))
		delilahRawTab->setDisabled(true);

	window->setLayout(mainLayout);
}
