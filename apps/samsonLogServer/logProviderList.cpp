/* ****************************************************************************
*
* FILE                     logProviderList.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 20 2011
*
*/
#include <QApplication>
#include <QDesktopWidget>
#include <QWidget>
#include <QSize>
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // LMT_*

#include "globals.h"            // mainLayout, idleLabel
#include "ports.h"              // LOG_SERVER_PORT
#include "Message.h"            // ss::Message::MessageCode, ...
#include "iomMsgRead.h"         // iomMsgRead
#include "iomMsgSend.h"         // iomMsgSend
#include "Popup.h"              // Popup
#include "LogProvider.h"        // LogProvider
#include "logProviderList.h"    // Own interface



extern char* ipGet(void);



/* ****************************************************************************
*
*
*/
#define DELIMITER_PATH    "/opt/samson/share/LogServer/images/delimiter.jpg"



/* ****************************************************************************
*
* global vars
*/
static LogProvider**    logProviderV    = NULL;
static unsigned int     logProviderMax  = 0;
static unsigned int     logProviders    = 0;
extern QListWidget*     logList;



/* ****************************************************************************
*
* logProviderListInit - 
*/
void logProviderListInit(unsigned int lpMax)
{
	logProviderMax = lpMax;
	logProviderV   = (LogProvider**) calloc(logProviderMax, sizeof(LogProvider*));
}



/* ****************************************************************************
*
* logProviderNameSet - 
*/
void logProviderNameSet(LogProvider* lpP, const char* name, const char* ip)
{
	LM_M(("Setting name ..."));

	if ((name != NULL) && (ip == NULL))
		return;

	if (name != NULL)
	{
		if (lpP->name != NULL)
			free(lpP->name);
		lpP->name = strdup(name);
	}

	if (ip != NULL)
	{
		if (lpP->host != NULL)
			free(lpP->host);
		lpP->host = strdup(ip);
	}

	LM_M(("Setting name to %s@%s", lpP->name, lpP->host));
	lpP->nameLabel->setText(QString(lpP->name) + QString("@") + QString(lpP->host));
}



/* ****************************************************************************
*
* logProviderAdd - 
*/
static void logProviderAdd(LogProvider* lpP)
{
	unsigned int ix;

	LM_T(LMT_LOG_PROVIDER_LIST, ("Adding logProvider for host '%s'", lpP->host));



	//
	// Hiding 'idle label' if necessary
	//
	if (logProviders == 0)
		idleLabel->hide();



	//
	// Finding next free slot in provider vector
	//
	for (ix = 0; ix < logProviderMax; ix++)
	{
		if (logProviderV[ix] == NULL)
			break;
	}

	if (ix >= logProviderMax)
		LM_X(1, ("No room for more LogProviders (max index is %d) - change and recompile!", logProviderMax));

	if (logProviderLookup(lpP->name, lpP->host) != NULL)
		LM_X(1, ("logProvider '%s' in host '%s' already in logProvider list", lpP->name, lpP->host));

	logProviderV[ix] = lpP;

	LM_T(LMT_LOG_PROVIDER_LIST, ("LogProvider '%s' at '%s' added", lpP->name, lpP->host));



	//
	// Creating the widgets
	//
	lpP->headerLayout = new QHBoxLayout();
	lpP->nameLabel    = new QLabel(QString(lpP->name) + QString("@") + QString(lpP->host));
	lpP->stateLabel   = new QLabel(QString("alive"));
	lpP->foldButton   = new QPushButton("hide");
	lpP->clearButton  = new QPushButton("clear");
	lpP->pauseButton  = new QPushButton("pause");
	lpP->stopButton   = new QPushButton("stop");
	lpP->removeButton = new QPushButton("remove");
	lpP->list         = new QListWidget();
	lpP->delimiter    = new QImage(DELIMITER_PATH);


	//
	// Setting sizes of push-buttons
	//
	lpP->foldButton->setMaximumSize(QSize(50, 30));
	lpP->clearButton->setMaximumSize(QSize(50, 30));
	lpP->removeButton->setMaximumSize(QSize(50, 30));



	//
	// Fonts
	//
	QFont labelFont("Times",   10, QFont::Normal);
	QFont buttonFont("Times",  10, QFont::Bold);
	QFont listFont("Courier",   8, QFont::Normal);

	lpP->nameLabel->setFont(labelFont);
	lpP->stateLabel->setFont(labelFont);

	lpP->foldButton->setFont(buttonFont);
	lpP->clearButton->setFont(buttonFont);
	lpP->pauseButton->setFont(buttonFont);
	lpP->stopButton->setFont(buttonFont);
	lpP->removeButton->setFont(buttonFont);

	lpP->list->setFont(listFont);



	//
	// Connecting button callbacks 
	//
	lpP->connect(lpP->foldButton,   SIGNAL(clicked()), lpP, SLOT(fold()));
	lpP->connect(lpP->clearButton,  SIGNAL(clicked()), lpP, SLOT(clear()));
	lpP->connect(lpP->pauseButton,  SIGNAL(clicked()), lpP, SLOT(pause()));
	lpP->connect(lpP->stopButton,   SIGNAL(clicked()), lpP, SLOT(stop()));
	lpP->connect(lpP->removeButton, SIGNAL(clicked()), lpP, SLOT(remove()));



	// Packing widgets in its layouts
	//
	mainLayout->addLayout(lpP->headerLayout);

	lpP->headerLayout->addWidget(lpP->nameLabel);
	lpP->headerLayout->addWidget(lpP->stateLabel);
	lpP->headerLayout->addWidget(lpP->foldButton);
	lpP->headerLayout->addWidget(lpP->clearButton);
	lpP->headerLayout->addWidget(lpP->pauseButton);
	lpP->headerLayout->addWidget(lpP->stopButton);
	lpP->headerLayout->addWidget(lpP->removeButton);

	lpP->headerLayout->addStretch(2);

	mainLayout->addWidget(lpP->list);
	// mainLayout->addWidget(lpP->delimiter);
	
	++logProviders;
}



/* ****************************************************************************
*
* logProviderAdd - 
*/
void logProviderAdd(ss::Endpoint* endpoint, const char* name, const char* host, int fd)
{
	LogProvider* lpP;

	LM_M(("Adding provider '%s' at '%s'", name, host));

	lpP = new LogProvider(endpoint, name, host, fd);
	if (lpP == NULL)
		LM_X(1, ("calloc: %s", strerror(errno)));

	logProviderAdd(lpP);
}



/* ****************************************************************************
*
* logProviderRemove - 
*/
void logProviderRemove(LogProvider* lpP)
{
	LM_M(("deleting widgets"));
	delete lpP->headerLayout;

	delete lpP->foldButton;
	delete lpP->stateLabel;
	delete lpP->nameLabel;
	delete lpP->clearButton;
	delete lpP->pauseButton;
	delete lpP->stopButton;
	delete lpP->removeButton;

	delete lpP->list;

	if (lpP->fd != -1)
	{
		LM_M(("Sending 'IDie' to endpoint that is to be removed"));
		iomMsgSend(lpP->fd, lpP->name, "logServer", ss::Message::IDie, ss::Message::Msg);
	}

	LM_M(("deleting LogProvider"));
	delete lpP;
	LM_M(("LogProvider deleted"));

	for (unsigned int ix = 0; ix < logProviderMax; ix++)
	{
		if (logProviderV[ix] == NULL)
			continue;

		if (logProviderV[ix] == lpP)
		{
			logProviderV[ix] = NULL;

			--logProviders;

			if (logProviders == 0)
				idleLabel->show();

			return;
		}
	}
}



/* ****************************************************************************
*
* logProviderLookup - 
*/
LogProvider* logProviderLookup(unsigned int ix)
{
	if (ix > logProviderMax)
		LM_X(1, ("cannot return logProvider %d - max logProvider id is %d", logProviderMax));

	return logProviderV[ix];
}



/* ****************************************************************************
*
* logProviderLookup - 
*/
LogProvider* logProviderLookup(char* name, char* host)
{
	for (unsigned int ix = 0; ix < logProviderMax; ix++)
	{
		if (logProviderV[ix] == NULL)
			continue;

		LM_T(LMT_LOG_PROVIDER_LIST, ("[%d] Comparing '%s' '%s' ...", ix, name, host));
		LM_T(LMT_LOG_PROVIDER_LIST, ("... to '%s' '%s'", logProviderV[ix]->name, logProviderV[ix]->host));

		if ((strcmp(logProviderV[ix]->host, host) == 0) &&  (strcmp(logProviderV[ix]->name, name) == 0))
			return logProviderV[ix];
	}

	return NULL;
}



/* ****************************************************************************
*
* logProviderLookup - 
*/
LogProvider* logProviderLookup(ss::Endpoint* ep)
{
	for (unsigned int ix = 0; ix < logProviderMax; ix++)
	{
		if (logProviderV[ix] == NULL)
			continue;

		if (logProviderV[ix]->endpoint == ep)
			return logProviderV[ix];
	}

	return NULL;
}



/* ****************************************************************************
*
* logProviderMaxGet - 
*/
unsigned int logProviderMaxGet(void)
{
	return logProviderMax;
}



/* ****************************************************************************
*
* logProviderListGet - 
*/
LogProvider** logProviderListGet(void)
{
	return logProviderV;
}



/* ****************************************************************************
*
* logProviderListShow - 
*/
void logProviderListShow(const char* why)
{
	LM_F(("---------- LogProvider List: %s ----------", why));
 
	for (unsigned int ix = 0; ix < logProviderMax; ix++)
	{
		if (logProviderV[ix] == NULL)
			continue;

		LM_F(("  %08p  LogProvider %02d: %-30s %-30s  (fd: %d)", logProviderV[ix], ix, logProviderV[ix]->name, logProviderV[ix]->host, logProviderV[ix]->fd));
	}
	LM_F(("--------------------------------"));
}



/* ****************************************************************************
*
* logProviderMsgTreat - 
*/
void logProviderMsgTreat(LogProvider* lpP)
{
	int                        s;
	ss::Message::MessageCode   code;
	ss::Message::MessageType   type;
	void*                      dataP;
	int                        dataLen;
	ss::Message::LogLineData*  logLine;

	s = iomMsgRead(lpP->fd, "supervisor", &code, &type, &dataP, &dataLen);
	if (s == -2) // Connection closed
		lpP->connectionClosed();
	else if (code == ss::Message::Hello)
	{
		ss::Message::HelloData*  helloP = (ss::Message::HelloData*) dataP;

		logProviderNameSet(lpP, helloP->name, helloP->ip);

		if (type == ss::Message::Msg)
		{
			ss::Message::HelloData   hello;
			int                      s;

			hello.type     = ss::Endpoint::LogServer;
			hello.workers  = 0;
			hello.port     = LOG_SERVER_PORT;
			hello.coreNo   = -1;
			hello.workerId = -1;

			strncpy(hello.name,   progName,     sizeof(hello.name));
			strncpy(hello.ip,     ipGet(),      sizeof(hello.ip));
			strncpy(hello.alias,  "logServer",  sizeof(hello.alias));
			
			s = iomMsgSend(lpP->fd, "connectingProcess", "logServer", ss::Message::Hello, ss::Message::Ack, &hello, sizeof(hello));
		}
		else
			LM_M(("Received a Hello Ack - logProvider updated"));
	}
	else if (code == ss::Message::LogLine)
	{
		char line[1024];

		logLine = (ss::Message::LogLineData*) dataP;

		if (dataLen != sizeof(ss::Message::LogLineData))
			LM_X(1, ("bad dataLen: %d (expected %d)", dataLen, sizeof(ss::Message::LogLineData)));

		if (type != ss::Message::Msg)
			LM_X(1, ("bad type of message: 0x%x", type));

		snprintf(line, sizeof(line), "%c: %s[%d]:%s: %s", logLine->type, logLine->file, logLine->lineNo, logLine->fName, logLine->text);
		logLine = (ss::Message::LogLineData*) dataP;

		lpP->list->addItem(QString(line));
		lpP->list->scrollToBottom();

		if ((logLine->type == 'E') || (logLine->type == 'X'))
		{
			char title[128];
			char line[512];

			if (logLine->type == 'W')
				snprintf(title, sizeof(title), "Warning");
			else if (logLine->type == 'E')
				snprintf(title, sizeof(title), "Error");
			else if (logLine->type == 'X')
				snprintf(title, sizeof(title), "Fatal Error");

			snprintf(line, sizeof(line), "%c: %s", logLine->type, logLine->text);
			new Popup(title, line);
		}
	}
	else
		LM_X(1, ("got something else ..."));
}
