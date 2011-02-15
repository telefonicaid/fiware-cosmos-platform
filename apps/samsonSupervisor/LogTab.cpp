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
	wi[2] = new QTableWidgetItem(host);
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
	QVBoxLayout*  mainLayout  = new QVBoxLayout(parent);

	Rows        = 3;
	tableWidget = new QTableWidget(Rows, 8);

	tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
	tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

	setHeaderLabels();
	tableWidget->resize(mainWinWidth, mainWinHeight);
	tableWidget->resizeColumnsToContents();

	mainLayout->addWidget(tableWidget);
	setLayout(mainLayout);
	tableWidget->setGridStyle(Qt::NoPen);
	row = 0;
}



/* ****************************************************************************
*
* logLineInsert - 
*/
void LogTab::logLineInsert(struct sockaddr_in* sAddr, ss::Message::Header* headerP, ss::Message::LogLineData* logLine)
{
	if (headerP->magic != 0xFEEDC0DE)
		LM_W(("Bad magic number ... 0x%08x", headerP->magic));

	char* host = inet_ntoa(sAddr->sin_addr);

	LM_W(("Got a log line (row %d): '%s'", row, logLine->text));

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
