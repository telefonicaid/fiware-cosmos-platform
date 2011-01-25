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
#include "traceLevels.h"        // LMT_*

#include "Endpoint.h"           // Endpoint
#include "ConfigWindow.h"       // Own interface



/* ****************************************************************************
*
* ConfigWindow - 
*/
ConfigWindow::ConfigWindow(ss::Endpoint* endpoint)
{
	QGridLayout*      layout;
	QLabel*           label;
	QSize             size;
	int               screenWidth;
	int               screenHeight;
	int               x;
	int               y;
	QDesktopWidget*   desktop = QApplication::desktop();
	QDialogButtonBox* buttonBox;
	char              processName[256];
	QCheckBox*        readsBox;
	QCheckBox*        writesBox;
	QCheckBox*        debugBox;
	QCheckBox*        verboseBox;
	QPushButton*      sendButton;  // To be included in QDialogButtonBox
	QLabel*           traceLevelLabel;
	QListWidget*      traceLevelList;
	QFont             traceFont("Helvetica", 10, QFont::Normal);
	QListWidgetItem*  traceLevel[256];

	this->endpoint = endpoint;

	setModal(true);

	layout = new QGridLayout();

	snprintf(processName, sizeof(processName), "%s@%s", endpoint->name.c_str(), endpoint->ip.c_str());
	
	label            = new QLabel(processName);
	buttonBox        = new QDialogButtonBox(QDialogButtonBox::Ok);
	readsBox         = new QCheckBox("Reads");
	writesBox        = new QCheckBox("Writes");
	debugBox         = new QCheckBox("Debug");
	verboseBox       = new QCheckBox("Verbose");
	sendButton       = new QPushButton("Send");
	traceLevelLabel  = new QLabel("Trace Levels");
	traceLevelList   = new QListWidget();

	traceLevelList->setFont(traceFont);
	traceLevelLabel->setFont(traceFont);

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

	setWindowTitle("Samson Process Configuration");

	layout->addWidget(label,      0, 0);
	layout->addWidget(readsBox,   1, 0);
	layout->addWidget(writesBox,  2, 0);
	layout->addWidget(debugBox,   3, 0);
	layout->addWidget(verboseBox, 4, 0);
	layout->addWidget(sendButton, 5, 0);
	layout->addWidget(buttonBox,  6, 0);
	layout->addWidget(traceLevelLabel, 1, 1);
	layout->addWidget(traceLevelList,  2, 1);

	for (int ix = 0; ix < 256; ix++)
	{
		char name[64];

		snprintf(name, sizeof(name), "Trace Level %03d", ix);
		traceLevel[ix] = new QListWidgetItem(name);
		traceLevelList->addItem(traceLevel[ix]);
		traceLevel[ix]->setCheckState(Qt::Checked);
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

	// iomMsgSend(endpoint, networkP->me, ss::Message::ConfigInfo, ss::Message::Msg)
	// iomMsgAwait(tmo: 2s);
	// iomMsgRead(endpoint, &header, &code, &type, &dataP, &dataLen);
	// Fill checkboxes and trace levels according to 'dataP' contents

	this->show();
}
