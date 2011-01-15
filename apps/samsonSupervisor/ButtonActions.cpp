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
ButtonActions::ButtonActions(QVBoxLayout* mainLayout, QWidget* window) : QWidget(window)
{
	win                = window;
	spawnerListLayout  = new QVBoxLayout();
	processListLayout  = new QVBoxLayout();
	buttonLayout       = new QVBoxLayout();

	mainLayout->addLayout(spawnerListLayout);
	mainLayout->addLayout(processListLayout);
	mainLayout->addLayout(buttonLayout);
}



/* ****************************************************************************
*
* spawnerListCreate - 
*/
void ButtonActions::spawnerListCreate(Spawner** spawnerV, int spawners)
{
	int ix;

	LM_M(("Creating %d spawners", spawners));

    QLabel* spawnersLabel = new QLabel("Spawners");
    spawnerListLayout->addWidget(spawnersLabel);

	for (ix = 0; ix < spawners; ix++)
	{
		Starter* starter;
		Spawner* spawnerP;

		if (spawnerV[ix] == NULL)
			continue;

		spawnerP = spawnerV[ix];
		starter  = new Starter("Spawner", spawnerP->host, false);

		LM_M(("Creating spawner '%s'", spawnerP->host));
		starter->spawnerSet(spawnerP);
		spawnerListLayout->addWidget(starter->checkbox);
	}

	// Connect Button
	connectButton = new QPushButton("Connect To All Spawners");
	connectButton->connect(connectButton, SIGNAL(clicked()), this, SLOT(connect()));
	spawnerListLayout->addWidget(connectButton);
}



/* ****************************************************************************
*
* processListCreate - 
*/
void ButtonActions::processListCreate(Process** processV, int process)
{
	int ix;

	LM_M(("Creating %d process", process));

    QLabel* processLabel = new QLabel("Processes");
    processListLayout->addWidget(processLabel);
	
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

		LM_M(("Creating process '%s'", processP->name));
		starter->processSet(processP);
		processListLayout->addWidget(starter->checkbox);
	}

	// Start Button
	startButton = new QPushButton("Start All Processes");
	startButton->connect(startButton, SIGNAL(clicked()), this, SLOT(start()));
	processListLayout->addWidget(startButton);
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
