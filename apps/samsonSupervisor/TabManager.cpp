/* ****************************************************************************
*
* FILE                     TabManager.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 14 2010
*
*/
#include <QtGui>
#include <QPushButton>

#include "logMsg.h"             // LM_*


#include "ProcessListTab.h"     // ProcessListTab
#include "LogTab.h"             // LogTab
#include "DelilahTab.h"         // DelilahTab
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
	delilahTab      = new DelilahTab("Delilah");

	tabWidget->addTab(processListTab, tr("Processes"));
	tabWidget->addTab(logTab,         tr("Log"));
	tabWidget->addTab(delilahTab,     tr("Delilah"));

	mainLayout->addWidget(tabWidget);
	mainLayout->addWidget(quit);

	window->setLayout(mainLayout);
}
