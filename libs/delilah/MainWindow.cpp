#include <iostream>
#include <string>

#include <QMenu>

#include "MainWindow.h"
#include "Workspace.h"
#include "WorkspaceView.h"
#include "WorkspaceScene.h"
#include "globals.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
	ui.setupUi(this);
	tool_group = new QActionGroup(this);
	tool_group->addAction(ui.actionSelect);
	tool_group->addAction(ui.actionNewQueue);
	tool_group->addAction(ui.actionNewOperation);
	tool_group->addAction(ui.actionConnect);
	ui.actionSelect->setChecked(true);

	connect(tool_group, SIGNAL(triggered(QAction*)), this, SLOT(setToolForAction(QAction*)));

	tab_id = 0;
	createNewWorkspace();
}

MainWindow::~MainWindow()
{
}

void MainWindow::createNewWorkspace(QString name)
{
	// increase tab counter
	tab_id++;

	if (name.isEmpty())
	{
		name = QString("Workspace %1").arg(tab_id);
	}

	Workspace* workspace = new Workspace(name);
	WorkspaceView* view = new WorkspaceView(workspace);
	view->setRenderHints(QPainter::Antialiasing);
	view->show();

	connect(this, SIGNAL(toolChanged(int)), workspace, SLOT(setTool(int)));

	ui.tabWidget->addTab(view, name);
}

void MainWindow::removeTab(int index)
{
	//TODO: check if process is running or scheduled. If yes, ask if cancel or stop before closing the tab.

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
	else if (action==ui.actionConnect)
		current_tool = TOOL_CONNECT;
	//else
	//	debug("Unknown action triggered");

	emit(toolChanged(current_tool));
}

