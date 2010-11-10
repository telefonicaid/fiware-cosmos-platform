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

	createTab(QString("Workspace %1").arg(ui.tabWidget->count() + 1));
}

MainWindow::~MainWindow()
{
}

void MainWindow::createTab(QString name)
{
	if (name.isEmpty())
	{
		name = QString("Workspace %1").arg(ui.tabWidget->count() + 1);
	}

	Workspace* workspace = new Workspace();

	WorkspaceScene* scene = new WorkspaceScene(this);
	WorkspaceView* view = new WorkspaceView(scene);
	view->setRenderHints(QPainter::Antialiasing);
	view->show();

	connect(this, SIGNAL(toolChanged(int)), scene, SLOT(setTool(int)));
	connect(scene, SIGNAL(addQueueRequested(const QPointF &)), view, SLOT(selectQueueType(const QPointF &)));
	connect(view, SIGNAL(createQueueRequested(QueueType, const QPointF &, QString, QString, QString)),
			workspace, SLOT(createQueue(QueueType, const QPointF &, QString, QString, QString)));

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
	else if (action==ui.actionConnect)
		current_tool = TOOL_CONNECT;
	//else
	//	debug("Unknown action triggered");

	emit(toolChanged(current_tool));
}



//void MainWindow::setName(QString x, QString &name)
//{
//	std::cout << "test\n";
//}
