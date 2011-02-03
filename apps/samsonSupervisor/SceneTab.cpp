/* ****************************************************************************
*
* FILE                     SceneTab.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 02 2011
*
*/
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QtGui>
#include <QMessageBox>

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Lmt*
#include "globals.h"            // connectionMgr, queueMgr, ...

#include "Popup.h"              // Popup
#include "DelilahScene.h"       // DelilahScene
#include "ConnectionMgr.h"      // ConnectionMgr
#include "QueueMgr.h"           // QueueMgr
#include "SceneTab.h"           // Own interface



/* ****************************************************************************
*
* SceneTab::SceneTab - 
*/
SceneTab::SceneTab(const char* name, QWidget *parent) : QWidget(parent)
{
	QHBoxLayout*  mainLayout;
	QVBoxLayout*  boxLayout;

	//
	// Creating the actions for the menu items
	//
	deleteAction = new QAction(QIcon(":/images/queueDelete.png"), tr("&Delete"), this);
	deleteAction->setShortcut(tr("Delete"));
	deleteAction->setStatusTip(tr("Delete queue"));
	connect(deleteAction, SIGNAL(triggered()), this, SLOT(qDelete()));

	exitAction = new QAction(tr("E&xit"), this);
	exitAction->setShortcut(tr("Ctrl+X"));
	exitAction->setStatusTip(tr("Quit Scenediagram example"));
	connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

	aboutAction = new QAction(tr("A&bout"), this);
	aboutAction->setShortcut(tr("Ctrl+B"));
	connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));


#if 0
	//
	// Creating menus
	//
	fileMenu  = menuBar()->addMenu(tr("&File"));
	itemMenu  = menuBar()->addMenu(tr("&Item"));
	aboutMenu = menuBar()->addMenu(tr("&Help"));

	fileMenu->addAction(exitAction);

	itemMenu->addAction(deleteAction);
	itemMenu->addSeparator();
	itemMenu->addAction(deleteAction);

	aboutMenu->addAction(aboutAction);
#endif


	//
	// Creating main widgets
	//
	mainLayout = new QHBoxLayout;
	boxLayout  = new QVBoxLayout;

	setLayout(mainLayout);


	//
	// Creating the buttons to the left
	//
	
	QIcon        queueIcon("images/queueAdd128x128.png");
	QIcon        connectIcon("images/connect.gif");
	QIcon        removeIcon("images/queueDelete.png");
	QIcon        qViewIcon("images/queue128x128.png");
	QIcon        commandIcon("images/operations.gif");

	QPushButton* queueButton   = new QPushButton(queueIcon,   "New queue");
	QPushButton* connectButton = new QPushButton(connectIcon, "Connection");
	QPushButton* removeButton  = new QPushButton(removeIcon,  "Remove Queue");
	QPushButton* qViewButton   = new QPushButton(qViewIcon,   "View queues");
	QPushButton* commandButton = new QPushButton(commandIcon, "Command List");

	queueButton->connect(queueButton, SIGNAL(clicked()), this, SLOT(qCreate()));
	connectButton->connect(connectButton, SIGNAL(clicked()), this, SLOT(connection()));
	removeButton->connect(removeButton, SIGNAL(clicked()), this, SLOT(qDelete()));
	qViewButton->connect(qViewButton, SIGNAL(clicked()), this, SLOT(qView()));
	commandButton->connect(commandButton, SIGNAL(clicked()), this, SLOT(command()));

	mainLayout->addLayout(boxLayout);
	boxLayout->addWidget(queueButton);
	boxLayout->addWidget(connectButton);
	boxLayout->addWidget(removeButton);
	boxLayout->addWidget(qViewButton);
	boxLayout->addWidget(commandButton);

	boxLayout->addStretch(500);


	//
	// Creating Graphics Scene and View
	//
	scene  = new DelilahScene(itemMenu);
	view   = new QGraphicsView(scene);

	scene->setSceneRect(QRectF(0, 0, 5000, 5000));

	mainLayout->addWidget(view);


	//
	// Creating help objects
	//
	connectionMgr = new ConnectionMgr(100);
	queueMgr      = new QueueMgr(100);
}



/* ****************************************************************************
*
* SceneTab::about
*/
void SceneTab::about(void)
{
	QMessageBox::about(this, tr("About Samson Delilah"),
					   tr("The <b>Samson Delilah Tab test</b> example shows "
						  "how samsonSupervisor will implement the graphical delilah."));
}



/* ****************************************************************************
*
* SceneTab::qDelete - 
*/
void SceneTab::qDelete(void)
{
	scene->remove2();
}



/* ****************************************************************************
*
* SceneTab::qCreate - 
*/
void SceneTab::qCreate(void)
{
	scene->qCreate();
}



/* ****************************************************************************
*
* SceneTab::connection - 
*/
void SceneTab::connection(void)
{
	scene->connection();
}



/* ****************************************************************************
*
* SceneTab::qView - 
*/
void SceneTab::qView(void)
{
   new Popup("Not Implemented", "Sorry, Queue View window is not implemented");
}



/* ****************************************************************************
*
* SceneTab::command - 
*/
void SceneTab::command(void)
{
   new Popup("Not Implemented", "Sorry, Command window is not implemented");
}
