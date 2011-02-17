/* ****************************************************************************
*
* FILE                     OldMrOperationsTab.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 02 2011
*
*/
#include <QCursor>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QtGui>
#include <QMessageBox>
#include <QGraphicsItem>

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Lmt*
#include "globals.h"            // connectionMgr, queueMgr, ...

#include "Popup.h"              // Popup
#include "DelilahScene.h"       // DelilahScene
#include "ConnectionMgr.h"      // ConnectionMgr
#include "QueueMgr.h"           // QueueMgr
#include "OldMrOperationsTab.h" // Own interface



/* ****************************************************************************
*
* OldMrOperationsTab::OldMrOperationsTab - 
*/
OldMrOperationsTab::OldMrOperationsTab(const char* name, QWidget *parent) : QWidget(parent)
{
	QHBoxLayout*  mainLayout;
	QVBoxLayout*  boxLayout;



	//
	// Creating main widgets
	//
	mainLayout = new QHBoxLayout;
	boxLayout  = new QVBoxLayout;

	setLayout(mainLayout);



	//
	// Creating the buttons to the left
	//
	
	QIcon        sourceIcon("images/Bomba.png");
	QIcon        queueIcon("images/queueAdd128x128.png");
	QIcon        resultIcon("images/Result.png");
	QIcon        connectIcon("images/connect.png");
	QIcon        removeIcon("images/queueDelete.png");

	QIcon        helpIcon("images/help.png");
	QIcon        qViewIcon("images/eye.png");
	QIcon        commandIcon("images/operations.png");
	QIcon        dataTypesIcon("images/queue128x128.png");
	QIcon        jobsIcon("images/queue128x128.png");
	QIcon        workersIcon("images/worker.png");
	QIcon        envIcon("images/queue128x128.png");
	QIcon        uploadIcon("images/queue128x128.png");
	QIcon        downloadIcon("images/queue128x128.png");
	QIcon        loadIcon("images/info.png");



#if 0
	//
	// Queue Button Test
	//
	QToolButton* queueButton   = new QToolButton();
	QLabel*      qLabel        = new QLabel("New queue");
	QHBoxLayout* qLayout       = new QHBoxLayout();

	queueButton->setIcon(queueIcon);
	boxLayout->addLayout(qLayout);
	qLayout->addWidget(queueButton);
	qLayout->addWidget(qLabel);


	//
	// Connect button Test
	//
	QToolButton* connectButton = new QToolButton();
	QHBoxLayout* cLayout       = new QHBoxLayout();

	connectButton->setIcon(connectIcon);
	boxLayout->addLayout(cLayout);
	cLayout->addWidget(connectButton);
	connectButton->setToolTip("New Connection");



	//
	// Remove Button Test
	//
	QPushButton* removeButton    = new QPushButton();
	QHBoxLayout* rLayout         = new QHBoxLayout();
	QPixmap*     removePixmap    = new QPixmap("images/queueDelete32x32.png");
	QLabel*      removeLabel     = new QLabel("     Remove");
	QLabel*      removeImage     = new QLabel();

	removeImage->setPixmap(*removePixmap);
	rLayout->addWidget(removeImage);
	rLayout->addStretch(20);
	rLayout->addWidget(removeLabel);
	rLayout->addStretch(20);

	rLayout->setContentsMargins(10, 2, 10, 2);
	removeButton->setLayout(rLayout);


	// More Tests
	// helpButton->setLayoutDirection(Qt::RightToLeft);
	// jobsButton->setDisabled(true);
	// workersButton->setFlat(true);
	
	QHBoxLayout* queueLayout = (QHBoxLayout*) queueButton->layout();
	if (queueLayout == NULL)
	{
		LM_W(("queueButton has no layout"));
		queueLayout = new QHBoxLayout(NULL);
		queueButton->setLayout(queueLayout);
	}
	queueLayout->setAlignment(Qt::AlignRight);
	queueLayout->insertSpacing(1, 200);
#endif


	QPushButton* sourceButton    = new QPushButton(sourceIcon,    "Add Source             ");
	QPushButton* queueButton     = new QPushButton(queueIcon,     "Add Queue       ");
	QPushButton* resultButton    = new QPushButton(resultIcon,    "Add Result             ");
	QPushButton* connectButton   = new QPushButton(connectIcon,   "Add Operation  ");
	QPushButton* removeButton    = new QPushButton(removeIcon,    "Remove             ");

	QPushButton* helpButton      = new QPushButton(helpIcon,      "Help                  ");
	QPushButton* qViewButton     = new QPushButton(qViewIcon,     "View queues       ");
	QPushButton* commandButton   = new QPushButton(commandIcon,   "Command List      ");
	QPushButton* dataTypesButton = new QPushButton(dataTypesIcon, "Data Types         ");
	QPushButton* jobsButton      = new QPushButton(jobsIcon,      "Jobs                   ");
	QPushButton* workersButton   = new QPushButton(workersIcon,   "Workers              ");
	QPushButton* envButton       = new QPushButton(envIcon,       "Environment       ");
	QPushButton* uploadButton    = new QPushButton(uploadIcon,    "Upload Data       ");
	QPushButton* downloadButton  = new QPushButton(downloadIcon,  "Download Data Sets");
	QPushButton* loadButton      = new QPushButton(loadIcon,      "Load Info          ");

	sourceButton->connect(sourceButton,       SIGNAL(clicked()), this, SLOT(source()));
	queueButton->connect(queueButton,         SIGNAL(clicked()), this, SLOT(qCreate()));
	resultButton->connect(resultButton,       SIGNAL(clicked()), this, SLOT(result()));
	connectButton->connect(connectButton,     SIGNAL(clicked()), this, SLOT(connection()));
	removeButton->connect(removeButton,       SIGNAL(clicked()), this, SLOT(qDelete()));
	
	helpButton->connect(helpButton,           SIGNAL(clicked()), this, SLOT(help()));
	qViewButton->connect(qViewButton,         SIGNAL(clicked()), this, SLOT(qView()));
	commandButton->connect(commandButton,     SIGNAL(clicked()), this, SLOT(command()));
	dataTypesButton->connect(dataTypesButton, SIGNAL(clicked()), this, SLOT(dataTypes()));
	jobsButton->connect(jobsButton,           SIGNAL(clicked()), this, SLOT(jobs()));
	workersButton->connect(workersButton,     SIGNAL(clicked()), this, SLOT(workers()));
	envButton->connect(envButton,             SIGNAL(clicked()), this, SLOT(env()));
	uploadButton->connect(uploadButton,       SIGNAL(clicked()), this, SLOT(upload()));
	downloadButton->connect(downloadButton,   SIGNAL(clicked()), this, SLOT(download()));
	loadButton->connect(loadButton,           SIGNAL(clicked()), this, SLOT(load()));

	mainLayout->addLayout(boxLayout);

	boxLayout->addWidget(sourceButton);
	boxLayout->addWidget(queueButton);
	boxLayout->addWidget(resultButton);
	boxLayout->addWidget(connectButton);
	boxLayout->addWidget(removeButton);
	
	boxLayout->addStretch(500);
	boxLayout->addWidget(helpButton);
	boxLayout->addWidget(qViewButton);
	boxLayout->addWidget(commandButton);
	boxLayout->addWidget(dataTypesButton);
	boxLayout->addWidget(jobsButton);
	boxLayout->addWidget(workersButton);
	boxLayout->addWidget(envButton);
	boxLayout->addWidget(uploadButton);
	boxLayout->addWidget(downloadButton);
	boxLayout->addWidget(loadButton);



	//
	// Disabling those buttons not implemented ...
	//
	uploadButton->setDisabled(true);
	downloadButton->setDisabled(true);
	


	//
	// Creating Graphics Scene and View
	//
	scene  = new DelilahScene();
	view   = new QGraphicsView(scene);


	QSize         pixmapSize;
	QPixmap*      bg;
	QPixmap*      bg2;
	bg  = new QPixmap("images/background.png");
	bg2 = new QPixmap("images/background.png");

	sceneLayer0 = scene->addPixmap(*bg);
	sceneLayer1 = scene->addPixmap(*bg2);

	sceneLayer1->setOpacity(0);

	pixmapSize = bg->size();
	scene->setSceneRect(QRectF(0, 0, pixmapSize.width(), pixmapSize.height()));
	view->setMaximumSize(pixmapSize.width(), pixmapSize.height());

	mainLayout->addWidget(view);


	//
	// Creating help objects
	//
	connectionMgr = new ConnectionMgr(100);
	queueMgr      = new QueueMgr(100);
	sourceMgr     = new SourceMgr(100);
	resultMgr     = new ResultMgr(100);
}



/* ****************************************************************************
*
* OldMrOperationsTab::qCreate - 
*/
void OldMrOperationsTab::qCreate(void)
{
	scene->qCreate();
}



/* ****************************************************************************
*
* OldMrOperationsTab::source - 
*/
void OldMrOperationsTab::source(void)
{
	scene->source();
}



/* ****************************************************************************
*
* OldMrOperationsTab::result - 
*/
void OldMrOperationsTab::result(void)
{
	scene->result();
}



/* ****************************************************************************
*
* OldMrOperationsTab::connection - 
*/
void OldMrOperationsTab::connection(void)
{
	scene->connection();
}



/* ****************************************************************************
*
* OldMrOperationsTab::qDelete - 
*/
void OldMrOperationsTab::qDelete(void)
{
	scene->remove();
}


#if 0 // Not used 

/* ****************************************************************************
*
* helpTextReceiver - 
*/
static void helpTextReceiver(const char* type, const char* text)
{
	new Popup("Help", text, false, 1);
}



/* ****************************************************************************
*
* qViewTextReceiver - 
*/
static void qViewTextReceiver(const char* type, const char* text)
{
	new Popup("Samson Queues / Data Sets", text, false, 2);
}



/* ****************************************************************************
*
* commandTextReceiver - 
*/
static void commandTextReceiver(const char* type, const char* text)
{
	new Popup("Samson Commands", text, false, 2);
}



/* ****************************************************************************
*
* dataTypesTextReceiver - 
*/
static void dataTypesTextReceiver(const char* type, const char* text)
{
	new Popup("Samson Data Types", text, false, 2);
}



/* ****************************************************************************
*
* jobsTextReceiver - 
*/
static void jobsTextReceiver(const char* type, const char* text)
{
	new Popup("Samson Jobs", text, false, 2);
}



/* ****************************************************************************
*
* workersTextReceiver - 
*/
static void workersTextReceiver(const char* type, const char* text)
{
	new Popup("Samson Workers", text, false, 1);
}



/* ****************************************************************************
*
* envTextReceiver - 
*/
static void envTextReceiver(const char* type, const char* text)
{
	new Popup("Samson Environment Variables", text, false, 1);
}



/* ****************************************************************************
*
* loadTextReceiver - 
*/
static void loadTextReceiver(const char* type, const char* text)
{
	new Popup("Samson Load", text, false, 0);
}
#endif


/* ****************************************************************************
*
* OldMrOperationsTab::help
*/
void OldMrOperationsTab::help(void)
{
	// helpDialog = new HelpDialog();

	//delilahConsole->writeCallbackSet(helpTextReceiver);
	//delilahConsole->evalCommand("help");
}



/* ****************************************************************************
*
* qView - 
*/
void OldMrOperationsTab::qView(void)
{
	//delilahConsole->writeCallbackSet(qViewTextReceiver);
	//delilahConsole->evalCommand("ls");
}



/* ****************************************************************************
*
* command - 
*/
void OldMrOperationsTab::command(void)
{
	//delilahConsole->writeCallbackSet(commandTextReceiver);
	//delilahConsole->evalCommand("operations");
}



/* ****************************************************************************
*
* dataTypes - 
*/
void OldMrOperationsTab::dataTypes(void)
{
	//delilahConsole->writeCallbackSet(dataTypesTextReceiver);
	//delilahConsole->evalCommand("datas");
}



/* ****************************************************************************
*
* jobs - 
*/
void OldMrOperationsTab::jobs(void)
{
	//delilahConsole->writeCallbackSet(jobsTextReceiver);
	//delilahConsole->evalCommand("jobs");
}



/* ****************************************************************************
*
* workers - 
*/
void OldMrOperationsTab::workers(void)
{
	//delilahConsole->writeCallbackSet(workersTextReceiver);
	//delilahConsole->evalCommand("workers");
}



/* ****************************************************************************
*
* env - 
*/
void OldMrOperationsTab::env(void)
{
	//delilahConsole->writeCallbackSet(envTextReceiver);
	//delilahConsole->evalCommand("set");
}



/* ****************************************************************************
*
* upload - 
*/
void OldMrOperationsTab::upload(void)
{
	new Popup("Samson Upload", "Sorry, this window is under construction");
}



/* ****************************************************************************
*
* download - 
*/
void OldMrOperationsTab::download(void)
{
	new Popup("Samson Download", "Sorry, this window is under construction");
}



/* ****************************************************************************
*
* load - 
*/
void OldMrOperationsTab::load(void)
{
	//delilahConsole->writeCallbackSet(loadTextReceiver);
	//delilahConsole->evalCommand("load");
}
