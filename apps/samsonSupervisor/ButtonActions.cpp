/* ****************************************************************************
*
* FILE                     ButtonActions.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 13 2011
*
*/
#include <QWidget>
#include <QPushButton>
#include <QLabel>

#include "logMsg.h"             // LM_*

#include "actions.h"            // help, list, start, ...
#include "Starter.h"            // Starter
#include "Spawner.h"            // Spawner
#include "Process.h"            // Process
#include "ButtonActions.h"      // Own interface



/* ****************************************************************************
*
* ButtonActions::ButtonActions
*/
ButtonActions::ButtonActions(QWidget* window) : QWidget(window)
{
	win                = window;
	layout             = new QHBoxLayout(window);
	spawnerListLayout  = new QVBoxLayout();
	processListLayout  = new QVBoxLayout();
	buttonLayout       = new QVBoxLayout();

	layout->addLayout(spawnerListLayout);
	layout->addLayout(processListLayout);
	layout->addLayout(buttonLayout);

	// Connect Button
	connectButton = new QPushButton("Connect");
	connectButton->connect(connectButton, SIGNAL(clicked()), this, SLOT(connect()));
	buttonLayout->addWidget(connectButton);

	// Start Button
	startButton = new QPushButton("Start");
	startButton->connect(startButton, SIGNAL(clicked()), this, SLOT(start()));
	buttonLayout->addWidget(startButton);

	// Quit Button
	quitButton = new QPushButton("Quit");
	quitButton->connect(quitButton, SIGNAL(clicked()), this, SLOT(quit()));
	buttonLayout->addWidget(quitButton);

	QLabel* spawnersLabel = new QLabel("Spawners");
	spawnerListLayout->addWidget(spawnersLabel);

	// spawnersLayout->setSpacing(10);
	// setLayout(layout);

	QLabel* processLabel = new QLabel("Processes");
    processListLayout->addWidget(processLabel);
}



/* ****************************************************************************
*
* spawnerListCreate - 
*/
void ButtonActions::spawnerListCreate(Spawner** spawnerV, int spawners)
{
	int ix;

	LM_M(("Creating %d spawners", spawners));

	for (ix = 0; ix < spawners; ix++)
	{
		Starter* starter;
		Spawner* spawnerP;

		if (spawnerV[ix] == NULL)
			continue;

		spawnerP = spawnerV[ix];
		starter  = new Starter("Spawner", spawnerP->host, false);
		spawnerListLayout->addWidget(starter);

		LM_M(("Creating spawner '%s'", spawnerP->host));
		starter->spawnerSet(spawnerP);
	}
}



/* ****************************************************************************
*
* processListCreate - 
*/
void ButtonActions::processListCreate(Process** processV, int process)
{
	int ix;

	LM_M(("Creating %d process", process));

	for (ix = 0; ix < process; ix++)
	{
		Starter* starter;
		Process* processP;
		char     name[128];

		if (processV[ix] == NULL)
			continue;

		processP = processV[ix];
		snprintf(name, sizeof(name), "%s@%s", processP->name, processP->host);
		starter  = new Starter("Process", name, false);
		processListLayout->addWidget(starter);

		LM_M(("Creating process '%s'", processP->name));
		starter->processSet(processP);
	}
}



/* ****************************************************************************
*
* ButtonActions::connect
*/
void ButtonActions::connect(void)
{
	connectToAllSpawners();
}



/* ****************************************************************************
*
* ButtonActions::start
*/
void ButtonActions::start(void)
{
	startAllProcesses();
}



/* ****************************************************************************
*
* ButtonActions::quit
*/
void ButtonActions::quit(void)
{
	LM_X(0, ("Quit button pressed - I quit !"));
}
