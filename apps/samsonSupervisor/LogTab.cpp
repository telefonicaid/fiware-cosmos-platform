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
#include "Popup.h"              // Popup
#include "LogTab.h"             // Own interface



/* ****************************************************************************
*
* LogItem - 
*/
typedef struct LogItem
{
	QTableWidgetItem* type;
	QTableWidgetItem* date;
	QTableWidgetItem* host;
	QTableWidgetItem* process;
	QTableWidgetItem* file;
	QTableWidgetItem* line;
	QTableWidgetItem* fName;
	QTableWidgetItem* text;
} LogItem;



/* ****************************************************************************
*
* LogItem list variables
*/
LogItem** logItemV    = NULL;
int       logItemMax  = 500;
int       logItems    = 0;



/* ****************************************************************************
*
* logItemInit - 
*/
static void logItemInit()
{
	logItemV = (LogItem**) calloc(logItemMax, sizeof(LogItem*));
	if (logItemV == NULL)
		LM_X(1, ("calloc(%d, %d): %s", logItemMax, sizeof(LogItem*), strerror(errno)));

	logItems = 0;
}



/* ****************************************************************************
*
* logItemInsert - 
*/
static void logItemInsert(LogItem* logItemP)
{
	if (logItems >= logItemMax)
	{
		logItemMax += 100;
		logItemV = (LogItem**) realloc(logItemV, logItemMax * sizeof(LogItem*));
		if (logItemV == NULL)
			LM_X(1, ("malloc(%d): %s", logItemMax * sizeof(LogItem*), strerror(errno)));
	}

	logItemV[logItems] = logItemP;
	++logItems;
}


#if 0
/* ****************************************************************************
*
* logItemFree - 
*/
static void logItemFree(LogItem* logItemP)
{
	if (logItemP == NULL)
		return;

	if (logItemP->type    != NULL)  delete logItemP->type;
	if (logItemP->date    != NULL)  delete logItemP->date;
	if (logItemP->host    != NULL)  delete logItemP->host;
	if (logItemP->process != NULL)  delete logItemP->process;
	if (logItemP->file    != NULL)  delete logItemP->file;
	if (logItemP->line    != NULL)  delete logItemP->line;
	if (logItemP->fName   != NULL)  delete logItemP->fName;
	if (logItemP->text    != NULL)  delete logItemP->text;
}
#endif


/* ****************************************************************************
*
* LogTab::LogTab - 
*/
LogTab::LogTab(QWidget* parent) : QWidget(parent)
{
	QFont         font("Courier", 10, QFont::Normal);

	QVBoxLayout*  mainLayout    = new QVBoxLayout(parent);
	QHBoxLayout*  buttonLayout  = new QHBoxLayout();
	QHBoxLayout*  filterLayout  = new QHBoxLayout();

	QPushButton*  logClear      = new QPushButton("Clear Log Window");
	QPushButton*  logFit        = new QPushButton("Fit Log Window");
	QPushButton*  logFile       = new QPushButton("Download Log File");
	QPushButton*  oldLogFile    = new QPushButton("Download Old Log File");

	QLabel*       filterLabel   = new QLabel("Filter");
	QPushButton*  filterButton  = new QPushButton("Update");

	filter = new QLineEdit();


	//
	// Initializing log item list
	//
	logItemInit();



	//
	// Creating Table Widget (holding the log lines)
	//
	Rows        = 3;
	row         = 0;
	tableWidget = new QTableWidget(Rows, 8);

	tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
	tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

	setHeaderLabels();
	tableWidget->resize(mainWinWidth, mainWinHeight);
	tableWidget->resizeColumnsToContents();
	tableWidget->setGridStyle(Qt::NoPen);
	tableWidget->setFont(font);



	//
	// Filling Page
	//	
	mainLayout->addLayout(buttonLayout);
	mainLayout->addWidget(tableWidget);
	mainLayout->addLayout(filterLayout);
	setLayout(mainLayout);



	//
	// Placing Command Pushbuttons
	//
	buttonLayout->addWidget(logClear);
	buttonLayout->addWidget(logFit);
	buttonLayout->addWidget(logFile);
	buttonLayout->addWidget(oldLogFile);



	//
	// Placing Filter items
	//
	filterLayout->addWidget(filterLabel);
	filterLayout->addWidget(filter);
	filterLayout->addWidget(filterButton);



	//
	// Connect buttons
	//
	connect(logClear,     SIGNAL(clicked()), this, SLOT(logViewClear()));
	connect(logFit,       SIGNAL(clicked()), this, SLOT(logViewFit()));
	connect(logFile,      SIGNAL(clicked()), this, SLOT(logFileDownload()));
	connect(oldLogFile,   SIGNAL(clicked()), this, SLOT(oldLogFileDownload()));
	connect(filterButton, SIGNAL(clicked()), this, SLOT(filterLog()));
}



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
	int          tLevel,
	bool         addToList
)
{
	char               line[16];
	char               type[2];
	LogItem*           logItemP;
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

	logItemP = (LogItem*) calloc(1, sizeof(LogItem));
	if (logItemP == NULL)
	{
		new Popup("Memory allocation error", "Error allocating memory for a Log item.\nCannot continue, sorry", true);
		LM_X(1, ("calloc(%d bytes): %s", sizeof(LogItem), strerror(errno)));
	}

	logItemP->type    = new QTableWidgetItem(type);
	logItemP->date    = new QTableWidgetItem(date);
	logItemP->host    = new QTableWidgetItem(hostP->name);
	logItemP->process = new QTableWidgetItem(pName);
	logItemP->file    = new QTableWidgetItem(file);
	logItemP->line    = new QTableWidgetItem(line);
	logItemP->fName   = new QTableWidgetItem(fName);
	logItemP->text    = new QTableWidgetItem(text);

	tableWidget->setItem(row, 0, logItemP->type);
	tableWidget->setItem(row, 1, logItemP->date);
	tableWidget->setItem(row, 2, logItemP->host);
	tableWidget->setItem(row, 3, logItemP->process);
	tableWidget->setItem(row, 4, logItemP->file);
	tableWidget->setItem(row, 5, logItemP->line);
	tableWidget->setItem(row, 6, logItemP->fName);
	tableWidget->setItem(row, 7, logItemP->text);

	if (addToList)
		logItemInsert(logItemP);
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
* LogTab::empty
*/
void LogTab::empty(void)
{
	tableWidget->clearContents();
	row = 0;
	setHeaderLabels();
	tableWidget->setRowCount(0);
	Rows = 0;
}



/* ****************************************************************************
*
* LogTab::clear
*/
void LogTab::clear(void)
{
    tableWidget->clear();
	empty();

#if 0
	// Seems that tableWidget->clear() takes care of this ...

	for (int ix = 0; ix < logItems; ix++)
	{
		logItemFree(logItemV[ix]);
		free(logItemV[ix]);
	}
#endif
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



/* ****************************************************************************
*
* LogTab::filterLog - 
*/
void LogTab::filterLog(void)
{
	int    ix;
	char*  filterText = strdup(filter->displayText().toStdString().c_str());

	if (filterText == NULL)
		return;

	if ((filterText[0] == '^') && (filterText[1] == 0))
		filterText[0] = 0;

	for (ix = 0; ix < logItems; ix++)
	{
		char wholeLine[1024];

		if (logItemV[ix] == NULL)
		{
			LM_W(("logItem %d is NULL!", ix));
			continue;
		}

		snprintf(wholeLine, sizeof(wholeLine), "%s:%s:%s:%s:%s:%s:%s:%s",
				 logItemV[ix]->type->text().toStdString().c_str(),
				 logItemV[ix]->date->text().toStdString().c_str(),
				 logItemV[ix]->host->text().toStdString().c_str(),
				 logItemV[ix]->process->text().toStdString().c_str(),
				 logItemV[ix]->file->text().toStdString().c_str(),
				 logItemV[ix]->line->text().toStdString().c_str(),
				 logItemV[ix]->fName->text().toStdString().c_str(),
				 logItemV[ix]->text->text().toStdString().c_str());
				 
		tableWidget->hideRow(ix);

		if ((strstr(wholeLine, filterText) != NULL)
		|| ((filterText[0] == '^') && (strncmp(&filterText[1], logItemV[ix]->text->text().toStdString().c_str(), strlen(&filterText[1])) == 0))
		||  (filterText[0] == 0))
			tableWidget->showRow(ix);
	}

	free(filterText);
}
