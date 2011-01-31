/* ****************************************************************************
*
* FILE                     ConfigWindow.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 25 2011
*
*/
#include <QGridLayout>
#include <QLabel>
#include <QSize>
#include <QDesktopWidget>
#include <QDialogButtonBox>
#include <QApplication>
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
#include "ConfigWindow.h"       // Own interface



/* ****************************************************************************
*
* ConfigWindow - 
*/
ConfigWindow::ConfigWindow(ss::Endpoint* endpoint)
{
	QGridLayout*              layout;
	QLabel*                   label;
	QSize                     size;
	int                       screenWidth;
	int                       screenHeight;
	int                       x;
	int                       y;
	QDesktopWidget*           desktop = QApplication::desktop();
	QDialogButtonBox*         buttonBox;
	char                      processName[256];
	QPushButton*              sendButton;  // To be included in QDialogButtonBox
	QLabel*                   traceLevelLabel;
	QListWidget*              traceLevelList;
	QFont                     labelFont("Times", 20, QFont::Normal);
	QFont                     traceFont("Helvetica", 10, QFont::Normal);
	void*                     dataP = NULL;
	int                       s;
	ss::Message::Header       header;
	ss::Message::MessageCode  code;
	ss::Message::MessageType  type;
	int                       dataLen;

	this->endpoint      = endpoint;
	allTraceLevelsState = false;

	setModal(true);

	layout = new QGridLayout();

	snprintf(processName, sizeof(processName), "%s@%s", endpoint->name.c_str(), endpoint->ip.c_str());
	
	label              = new QLabel(processName);
	buttonBox          = new QDialogButtonBox(QDialogButtonBox::Ok);

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

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(sendButton, SIGNAL(clicked()), this, SLOT(send()));
	setWindowTitle("Samson Process Configuration");

	layout->addWidget(label,      0, 1, 1, -1);

	layout->addWidget(verboseBox, 1, 0);
	layout->addWidget(debugBox,   2, 0);
	layout->addWidget(readsBox,   3, 0);
	layout->addWidget(writesBox,  4, 0);
	layout->addWidget(toDoBox,    5, 0);

	layout->addWidget(sendButton, 7, 0);
	layout->addWidget(buttonBox,  8, 0);

	layout->addWidget(traceLevelLabel, 1, 1);
	layout->addWidget(allTraceLevelsItem, 2, 1);
	layout->addWidget(traceLevelList,  3, 1);

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

	this->setLayout(layout);
	this->show();

	// Window Geometry
	size = this->size();

	screenWidth  = desktop->width();
	screenHeight = desktop->height();

	x = (screenWidth  - size.width())  / 2;
	y = (screenHeight - size.height()) / 2;

	this->move(x, y);

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
			s = full_read(endpoint->rFd, (char*) &header, sizeof(header));
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

	this->show();
}



/* ****************************************************************************
*
* send - 
*/
void ConfigWindow::send(void)
{
	ss::Message::ConfigData configData;

	configData.verbose = (verboseBox->checkState() == Qt::Checked)? true : false;
	configData.debug   = (debugBox->checkState()   == Qt::Checked)? true : false;
	configData.reads   = (readsBox->checkState()   == Qt::Checked)? true : false;
	configData.writes  = (writesBox->checkState()  == Qt::Checked)? true : false;
	configData.toDo    = (toDoBox->checkState()    == Qt::Checked)? true : false;

	for (int ix = 0; ix < TRACE_LEVELS; ix++)
	{
		if (traceLevelItem[ix] != NULL)
			configData.traceLevels[ix] = (traceLevelItem[ix]->checkState() == Qt::Checked)? true : false;
		else
			configData.traceLevels[ix] = false;
	}

	iomMsgSend(endpoint, networkP->endpoint[0], ss::Message::ConfigSet, ss::Message::Ack, &configData, sizeof(configData));
}



/* ****************************************************************************
*
* all - 
*/
void ConfigWindow::all(void)
{
	allTraceLevelsState = (allTraceLevelsState == true)? false : true;

	for (int ix = 0; ix < TRACE_LEVELS; ix++)
	{
		if (traceLevelItem[ix] != NULL)
			traceLevelItem[ix]->setCheckState((allTraceLevelsState == true)? Qt::Checked : Qt::Unchecked);
	}
}
