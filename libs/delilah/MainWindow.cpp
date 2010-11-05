#include <iostream>
#include <string>

#include <QMenu>

#include "DelilahQtApp.h"
#include "MainWindow.h"
#include "ProcessView.h"
#include "ProcessScene.h"
#include "globals.h"

#include "NewTXTQueueDlg.h"

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
	connect(scene, SIGNAL(addQueueRequested(const QPoint &)), this, SLOT(addQueue(const QPoint &)));

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
	else if (action==ui.actionConnect)
		current_tool = TOOL_CONNECT;
	//else
	//	debug("Unknown action triggered");

	emit(toolChanged(current_tool));
}

void MainWindow::addQueue(const QPoint &pos)
{
	QMenu* menu = new QMenu(ui.tabWidget->currentWidget());
	menu->addAction("Existing Queue", this, SLOT(showAvailableQueues()));
	menu->addAction("New TXT Queue", this, SLOT(createTXTQueue()));
	menu->addAction("New KV Queue", this, SLOT(createKVQueue()));
	menu->exec(pos);
}

void MainWindow::showAvailableQueues()
{
	std::cout << "TODO!!!!!!!!!!!!!!!!!!\n";
}

void MainWindow::createTXTQueue()
{
	DelilahQtApp* a = (DelilahQtApp*)qApp;

	QList<QString> names = a->existingQueuesNames();

	NewTXTQueueDlg dlg(names, this);

	if (dlg.exec() == QDialog::Rejected)
		return;

	if (!dlg.getName().isEmpty())
	{
		// TODOD: create queue
		ProcessView* view = (ProcessView*)ui.tabWidget->currentWidget();
		ProcessScene* scene = (ProcessScene*)view->scene();
		// TODO: fix position
		scene->addQueue(QPoint(0, 0));
	}

}

void MainWindow::createKVQueue()
{

}

//void MainWindow::setName(QString x, QString &name)
//{
//	std::cout << "test\n";
//}
