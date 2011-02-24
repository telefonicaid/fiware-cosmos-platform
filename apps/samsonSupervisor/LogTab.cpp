/* ****************************************************************************
*
* FILE                     LogTab.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 02 2011
*
*/
#include <netinet/in.h>         // sockaddr_in
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <QWidget>
#include <QGridLayout>
#include <QTableView>
#include <QStandardItemModel>
#include <QTableWidget>


#include "logMsg.h"             // LM_*

#include "globals.h"            // mainWinWidth, ...
#include "Message.h"            // ss::Message::Header, ss::Message::LogLineData
#include "Endpoint.h"           // ss::Endpoint
#include "iomMsgSend.h"         // iomMsgSend
#include "samson/Log.h"			// LogLineData
#include "LogTab.h"             // Own interface



/* ****************************************************************************
*
* LogTab::logItemAdd - 
*/
void LogTab::logItemAdd
(
	int          row,
	const char   typ,
	const char*  date,
	const char*  host,
	const char*  pName,
	const char*  file,
	int          lineNo,
	const char*  fName,
	const char*  text,
	int          tLevel
)
{
	char               line[16];
	char               type[2];
	QTableWidgetItem*  wi[8];
	QFont              font("Courier", 10, QFont::Normal);
	Host*              hostP;

	hostP = networkP->hostMgr->lookup(host);
	if (hostP == NULL)
		LM_X(1, ("Cannot find host '%s' in Host Manager"));

	if (row >= Rows)
	{
		Rows += 3;
		tableWidget->setRowCount(Rows);
	}

	snprintf(line, sizeof(line), "%d", lineNo);
	type[0] = typ;
	type[1] = 0;

	wi[0] = new QTableWidgetItem(type);
	wi[1] = new QTableWidgetItem(date);
	wi[2] = new QTableWidgetItem(hostP->name);
	wi[3] = new QTableWidgetItem(pName);
	wi[4] = new QTableWidgetItem(file);
	wi[5] = new QTableWidgetItem(line);
	wi[6] = new QTableWidgetItem(fName);
	wi[7] = new QTableWidgetItem(text);

	for (unsigned int ix = 0; ix < sizeof(wi) / sizeof(wi[0]); ix++)
	{
		tableWidget->setItem(row, ix, wi[ix]);
		tableWidget->setFont(font);
	}
}



/* ****************************************************************************
*
* LogTab::LogTab - 
*/
LogTab::LogTab(QWidget* parent) : QWidget(parent)
{
	QVBoxLayout*  mainLayout    = new QVBoxLayout(parent);
	QHBoxLayout*  buttonLayout  = new QHBoxLayout();
	QPushButton*  logClear      = new QPushButton("Clear Log Window");
	QPushButton*  logFit        = new QPushButton("Fit Log Window");
	QPushButton*  logFile       = new QPushButton("Download Log File");
	QPushButton*  oldLogFile    = new QPushButton("Download Old Log File");
	
	Rows        = 3;
	tableWidget = new QTableWidget(Rows, 8);

	tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
	tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

	setHeaderLabels();
	tableWidget->resize(mainWinWidth, mainWinHeight);
	tableWidget->resizeColumnsToContents();

	mainLayout->addLayout(buttonLayout);
	mainLayout->addWidget(tableWidget);
	setLayout(mainLayout);
	tableWidget->setGridStyle(Qt::NoPen);
	row = 0;

	//
	// Pushbuttons
	//
	buttonLayout->addWidget(logClear);
	buttonLayout->addWidget(logFit);
	buttonLayout->addWidget(logFile);
	buttonLayout->addWidget(oldLogFile);


	connect(logClear,    SIGNAL(clicked()), this, SLOT(logViewClear()));
	connect(logFit,      SIGNAL(clicked()), this, SLOT(logViewFit()));
	connect(logFile,     SIGNAL(clicked()), this, SLOT(logFileDownload()));
	connect(oldLogFile,  SIGNAL(clicked()), this, SLOT(oldLogFileDownload()));
}



/* ****************************************************************************
*
* logLineInsert - 
*/
void LogTab::logLineInsert(struct sockaddr_in* sAddr, ss::Message::Header* headerP, ss::LogLineData* logLine)
{
	if (headerP->magic != 0xFEEDC0DE)
		LM_W(("Bad magic number ... 0x%08x", headerP->magic));

	char* host = inet_ntoa(sAddr->sin_addr);

	if (strcmp(host, "127.0.1.1") == 0)
		host = (char*) "127.0.0.1";

	logItemAdd(row, logLine->type, logLine->date, host, logLine->processName, logLine->file, logLine->lineNo, logLine->fName, logLine->text, logLine->tLev);

	if (row == 0)
		tableWidget->resizeColumnsToContents();

	++row;
}



/* ****************************************************************************
*
* setHeaderLabels - 
*/
void LogTab::setHeaderLabels(void)
{
	QStringList   headerLabels;

	headerLabels << "Type" << "Date" << "Host" << "Process" << "File" << "Line No" << "Function" << "Log Text";
	tableWidget->setHorizontalHeaderLabels(headerLabels);
}



/* ****************************************************************************
*
* LogTab::clear
*/
void LogTab::clear(void)
{
	tableWidget->clear();
	row = 0;
	setHeaderLabels();
	tableWidget->setRowCount(0);
	Rows = 0;
}



/* ****************************************************************************
*
* LogTab::logViewClear - 
*/
void LogTab::logViewClear(void)
{
	clear();
}



/* ****************************************************************************
*
* LogTab::logViewFit - 
*/
void LogTab::logViewFit(void)
{
	tableWidget->resizeColumnsToContents();
}



/* ****************************************************************************
*
* getHostAndProcess - 
*/
int LogTab::getHostAndProcess(char* host, char* processName)
{
	QTableWidgetItem* processItem;
	QTableWidgetItem* hostItem;
	int               currentRow = tableWidget->currentRow();
	
	if (currentRow == -1)
	{
		new InfoWin("Nothing selected", "Please select a row.\nThis way indicates which process you want the log file from.");
		return -1;
	}

	processItem = tableWidget->item(currentRow, 3);
	hostItem    = tableWidget->item(currentRow, 2);

	if ((processItem == NULL) || (hostItem == NULL))
	{
		new InfoWin("Internal error", "Sorry, cannot deduct what process to use ...");
		return -1;
	}

	strcpy(host, hostItem->text().toStdString().c_str());
	strcpy(processName, processItem->text().toStdString().c_str());

	return 0;
}



/* ****************************************************************************
*
* LogTab::logFileDownload - 
*/
void LogTab::logFileDownload(void)
{
	char           host[64];
	char           processName[64];
	ss::Endpoint*  ep;
	int            s;

	if (getHostAndProcess(host, processName) == -1)
		return;

	ep = networkP->endpointLookup(processName, host);
	if (ep == NULL)
	{
		new InfoWin("Process lookup error", "Sorry, cannot find process - please check that it is connected.");
		return;
	}

	if (ep->state != ss::Endpoint::Connected)
	{
		new InfoWin("Process not connected", "Sorry, the selected process is no longer connected");
		return;
	}
	
	s = iomMsgSend(ep, networkP->endpoint[0], ss::Message::EntireLogFile);
	if (s != 0)
		new InfoWin("Internal Error", "Error sending message to endpoint");
}



/* ****************************************************************************
*
* LogTab::oldLogFileDownload - 
*/
void LogTab::oldLogFileDownload(void)
{
	char           host[64];
	char           processName[64];
	ss::Endpoint*  ep;
	int            s;

	if (getHostAndProcess(host, processName) == -1)
		return;
	
	ep = networkP->endpointLookup(processName, host);
	if (ep == NULL)
	{
		new InfoWin("Process lookup error", "Sorry, cannot find process - please check that it is connected.");
		return;
	}

	if (ep->state != ss::Endpoint::Connected)
	{
		new InfoWin("Process not connected", "Sorry, the selected process is no longer connected");
		return;
	}

	s = iomMsgSend(ep, networkP->endpoint[0], ss::Message::EntireOldLogFile);
	if (s != 0)
		new InfoWin("Internal Error", "Error sending message to endpoint");
}
