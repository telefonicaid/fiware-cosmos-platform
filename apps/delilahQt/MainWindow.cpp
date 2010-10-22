#include <iostream>
#include <string>

#include "MainWindow.h"
#include "ProcessView.h"
#include "ProcessScene.h"
#include "globals.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

	ui.setupUi(this);
	tool_group = new QActionGroup(this);
	tool_group->addAction(ui.actionSelect);
	tool_group->addAction(ui.actionNewQueue);
	tool_group->addAction(ui.actionNewOperation);
	ui.actionSelect->setChecked(true);

	connect(tool_group, SIGNAL(triggered(QAction*)), this, SLOT(setToolForAction(QAction*)));

	createTab("New Process");
}

MainWindow::~MainWindow()
{
}

void MainWindow::createTab(QString name)
{
	if (name.isEmpty())
	{
		name = "New";
	}

	ProcessScene* scene = new ProcessScene(this);
	connect(this, SIGNAL(toolChanged(int)), scene, SLOT(setTool(int)));

	ProcessView* view = new ProcessView(scene);
	view->setRenderHints(QPainter::Antialiasing);
	view->show();

	ui.tabWidget->addTab(view, name);

	// initiate tool
	setToolForAction(tool_group->checkedAction());
}


void MainWindow::removeTab(int index)
{
	//TODO: check if process is running or scheduled. If yes, ask if cancel or stop before closing the tab.

//	QWidget* tab = ui.tabWidget->widget(index);
//	if (tab!=0)
//	{
//		delete tab;
//	}

	ui.tabWidget->removeTab(index);

}

void MainWindow::setToolForAction(QAction* action)
{
	int current_tool = 0;

	if (action==ui.actionSelect)
		current_tool = TOOL_SELECT;
	else if (action==ui.actionNewQueue)
		current_tool = TOOL_NEWQUEUE;
	else if (action==ui.actionNewOperation)
		current_tool = TOOL_NEWOPERATION;
	//else
	//	debug("Unknown action triggered");

	emit(toolChanged(current_tool));
}
