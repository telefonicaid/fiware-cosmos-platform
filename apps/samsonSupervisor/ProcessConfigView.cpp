/* ****************************************************************************
*
* FILE                     ProcessConfigView.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 09 2011
*
*/
#include <QGridLayout>
#include <QLabel>
#include <QSize>
#include <QCheckBox>
#include <QPushButton>
#include <QListWidget>

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "globals.h"            // networkP, ...
#include "iomMsgSend.h"         // iomMsgSend
#include "iomMsgAwait.h"        // iomMsgAwait
#include "iomMsgRead.h"         // iomMsgRead
#include "Endpoint.h"           // Endpoint
#include "Process.h"            // Process
#include "ProcessConfigView.h"  // Own interface



/* ****************************************************************************
*
* ProcessConfigView::~ProcessConfigView - 
*/
ProcessConfigView::~ProcessConfigView()
{
	int ix;

	for (ix = 0; ix < TRACE_LEVELS; ix++)
	{
		delete traceLevelItem[ix];
	}


	label->hide();
	sendButton->hide();
	traceLevelLabel->hide();
	traceLevelList->hide();
	verboseBox->hide();
	debugBox->hide();
	readsBox->hide();
	writesBox->hide();
	toDoBox->hide();
	allTraceLevelsItem->hide();

	grid->removeWidget(label);
	grid->removeWidget(sendButton);
	grid->removeWidget(traceLevelLabel);
	grid->removeWidget(traceLevelList);
	grid->removeWidget(verboseBox);
	grid->removeWidget(debugBox);
	grid->removeWidget(readsBox);
	grid->removeWidget(writesBox);
	grid->removeWidget(toDoBox);
	grid->removeWidget(allTraceLevelsItem);

	delete label;
	delete sendButton;
	delete traceLevelLabel;
	delete traceLevelList;
	delete verboseBox;
	delete debugBox;
	delete readsBox;
	delete writesBox;
	delete toDoBox;
	delete allTraceLevelsItem;
}



/* ****************************************************************************
*
* ProcessConfigView::ProcessConfigView - 
*/
ProcessConfigView::ProcessConfigView(QGridLayout* grid, Process* process)
{
	char                      processName[256];
	QFont                     labelFont("Times", 18, QFont::Normal);
	QFont                     traceFont("Helvetica", 10, QFont::Normal);
	void*                     dataP = NULL;
	int                       s;
	ss::Message::Header       header;
	ss::Message::MessageCode  code;
	ss::Message::MessageType  type;
	int                       dataLen;
	ss::Endpoint*             endpoint = process->endpoint;

	this->grid    = grid;
	this->process = process;

	snprintf(processName, sizeof(processName), "%s@%s", endpoint->name.c_str(), endpoint->ip.c_str());
	
	label              = new QLabel(processName);

	verboseBox         = new QCheckBox("Verbose");
	debugBox           = new QCheckBox("Debug");
	readsBox           = new QCheckBox("Reads");
	writesBox          = new QCheckBox("Writes");
	toDoBox            = new QCheckBox("ToDo");

	sendButton         = new QPushButton("Send");
	traceLevelLabel    = new QLabel("Trace Levels");
	traceLevelList     = new QListWidget();
	allTraceLevelsItem = new QPushButton("ALL/None");

	label->setFont(labelFont);
	traceLevelLabel->setFont(traceFont);
	traceLevelList->setFont(traceFont);

	connect(sendButton, SIGNAL(clicked()), this, SLOT(send()));

	grid->addWidget(label,      0, 0, 1, -1);

	grid->addWidget(traceLevelLabel,    1, 0, 1, 1);
	grid->addWidget(allTraceLevelsItem, 1, 2, 1, 2);
	grid->addWidget(traceLevelList,     3, 0, 5, 4);

	grid->addWidget(verboseBox, 9,  0);
	grid->addWidget(debugBox,   10, 0);
	grid->addWidget(readsBox,   9,  1);
	grid->addWidget(writesBox,  10, 1);
	grid->addWidget(toDoBox,     9, 2);

	grid->addWidget(sendButton, 13, 0, 1, 4);

	memset(traceLevelItem, 0, sizeof(traceLevelItem));

	connect(allTraceLevelsItem, SIGNAL(clicked()), this, SLOT(all()));

	for (int ix = 0; ix < TRACE_LEVELS; ix++)
	{
		char* name;
		char  levelName[256];

		name = traceLevelName((TraceLevels) ix);
		if (name == NULL)
			continue;

		snprintf(levelName, sizeof(levelName), "%s (%d)", name, ix);
		traceLevelItem[ix] = new QListWidgetItem(levelName);
		traceLevelList->addItem(traceLevelItem[ix]);
		traceLevelItem[ix]->setCheckState(Qt::Checked);
	}
	// traceLevelList->adjustSize();
	traceLevelList->setFixedWidth(400);
	traceLevelList->setFixedHeight(500);

	memset(&header, 0, sizeof(header));

	s = iomMsgSend(endpoint, networkP->endpoint[0], ss::Message::ConfigGet, ss::Message::Msg);

	if (s != 0)
		LM_E(("iomMsgSend error: %d", s));
	else
	{
		s = iomMsgAwait(endpoint->rFd, 2, 0);
		if (s != 1)
			LM_E(("iomMsgAwait error: %d", s));
		else
		{
			s = iomMsgPartRead(endpoint, "header", (char*) &header, sizeof(header));
			if (s != sizeof(header))
				LM_E(("Bad length of header read (read len: %d)", s));
			else
			{
				ss::Message::ConfigData  configData;
				ss::Message::ConfigData* configDataP;
				
				dataP   = &configData;
				dataLen = sizeof(configData);

				s = iomMsgRead(endpoint, &header, &code, &type, &dataP, &dataLen);
				configDataP = (ss::Message::ConfigData*) dataP;
				if (s != 0)
					LM_E(("iomMsgRead returned %d", s));
				else
				{
					// Fill checkboxes and trace levels according to 'dataP' contents
					if (dataP == NULL)
						LM_E(("iomMsgRead didn't fill the data pointer ..."));
					else
					{
						verboseBox->setCheckState((configDataP->verbose == true)? Qt::Checked : Qt::Unchecked);
						debugBox->setCheckState((configDataP->debug     == true)? Qt::Checked : Qt::Unchecked);
						readsBox->setCheckState((configDataP->reads     == true)? Qt::Checked : Qt::Unchecked);
						writesBox->setCheckState((configDataP->writes   == true)? Qt::Checked : Qt::Unchecked);
						toDoBox->setCheckState((configDataP->toDo       == true)? Qt::Checked : Qt::Unchecked);
						
						for (int ix = 0; ix < TRACE_LEVELS; ix++)
						{
							if (traceLevelItem[ix])
								traceLevelItem[ix]->setCheckState((configDataP->traceLevels[ix] == true)? Qt::Checked : Qt::Unchecked);
						}
					}
				}
			}
		}
	}
}



/* ****************************************************************************
*
* send - 
*/
void ProcessConfigView::send(void)
{
	ss::Message::ConfigData configData;

	configData.verbose    = (verboseBox->checkState() == Qt::Checked)? true : false;
	configData.debug      = (debugBox->checkState()   == Qt::Checked)? true : false;
	configData.reads      = (readsBox->checkState()   == Qt::Checked)? true : false;
	configData.writes     = (writesBox->checkState()  == Qt::Checked)? true : false;
	configData.toDo       = (toDoBox->checkState()    == Qt::Checked)? true : false;

	for (int ix = 0; ix < TRACE_LEVELS; ix++)
	{
		if (traceLevelItem[ix] != NULL)
			configData.traceLevels[ix] = (traceLevelItem[ix]->checkState() == Qt::Checked)? true : false;
		else
			configData.traceLevels[ix] = false;
	}

	iomMsgSend(process->endpoint, networkP->endpoint[0], ss::Message::ConfigSet, ss::Message::Ack, &configData, sizeof(configData));
}



/* ****************************************************************************
*
* all - 
*/
void ProcessConfigView::all(void)
{
	allTraceLevelsState = (allTraceLevelsState == true)? false : true;

	for (int ix = 0; ix < TRACE_LEVELS; ix++)
	{
		if (traceLevelItem[ix] != NULL)
			traceLevelItem[ix]->setCheckState((allTraceLevelsState == true)? Qt::Checked : Qt::Unchecked);
	}
}
