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
	QPushButton*     quit            = new QPushButton("Quit");
	ProcessListTab*  processListTab  = new ProcessListTab("Processes", window);
	LogTab*          logTab          = new LogTab("Log");
	DelilahTab*      delilahTab      = new DelilahTab("Delilah");

	tabWidget = new QTabWidget();

	tabWidget->addTab(processListTab, tr("Processes"));
	tabWidget->addTab(logTab,         tr("Log"));
	tabWidget->addTab(delilahTab,     tr("Delilah"));

	connect(quit, SIGNAL(clicked()), qApp, SLOT(quit()));

	QVBoxLayout* mainLayout = new QVBoxLayout;

	mainLayout->addWidget(tabWidget);
	mainLayout->addWidget(quit);
	window->setLayout(mainLayout);
}
