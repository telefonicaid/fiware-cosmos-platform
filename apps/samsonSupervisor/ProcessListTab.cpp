#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

#include "logMsg.h"             // LM_*

#include "ButtonActions.h"      // ButtonActions
#include "ProcessListTab.h"     // Own interface



/* ****************************************************************************
*
* ProcessListTab::ProcessListTab - 
*/
ProcessListTab::ProcessListTab(const char* name, QWidget *parent) : QWidget(parent)
{
	QHBoxLayout* mainLayout  = new QHBoxLayout;

	setLayout(mainLayout);

	ButtonActions ba(mainLayout, parent);



	//
	// Spawner List
	//
	int        noOfSpawners;
	Spawner**  spawnerVec;

	spawnerVec = spawnerListGet(&noOfSpawners);
	LM_M(("Got %d Spawners", noOfSpawners));
	ba.spawnerListCreate(spawnerVec, noOfSpawners);



	//
    // Process List
    //
	int        noOfProcesses;
	Process**  processVec;

	processVec = processListGet(&noOfProcesses);
	LM_M(("Got %d Processes", noOfProcesses));
	ba.processListCreate(processVec, noOfProcesses);

	mainLayout->addStretch(400);
}
