/* ****************************************************************************
*
* FILE                     TabManager.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 14 2010
*
*/
#include <unistd.h>             // close
#include <sys/time.h>           // getimeofday
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <QtGui>
#include <QPushButton>
#include <QVBoxLayout>

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "globals.h"            // networkP, ...
#include "ports.h"              // LOG_MESSAGE_PORT
#include "permissions.h"        // UpSeeLogs, ...
#include "Popup.h"              // Popup
#include "ProcessListTab.h"     // ProcessListTab
#include "MrOperationsTab.h"    // MrOperationsTab
#include "LogTab.h"             // LogTab
#include "DelilahRawTab.h"      // DelilahRawTab
#include "ConfigTab.h"          // ConfigTab
#include "OldMrOperationsTab.h" // OldMrOperationsTab
#include "starterList.h"        // starterListShow
#include "TabManager.h"         // Own interface



/* ****************************************************************************
*
* TabManager::TabManager - 
*/
TabManager::TabManager(QWidget* window, QWidget* parent) : QWidget(parent)
{
	QVBoxLayout*     mainLayout = new QVBoxLayout;
	QPushButton*     quit       = new QPushButton("Quit");

	connect(quit, SIGNAL(clicked()), qApp, SLOT(quit()));

	tabWidget       = new QTabWidget();

	processListTab  = new ProcessListTab("Processes", window);
	mrOperationsTab = new MrOperationsTab("MR Operations");
	logTab          = new LogTab();
	delilahRawTab   = new DelilahRawTab("Console");
	configTab       = new ConfigTab();
	delilahOldTab   = new OldMrOperationsTab("Old Delilah");

	tabWidget->addTab(processListTab,      tr("Processes"));
	tabWidget->addTab(mrOperationsTab,     tr("MR Operations"));
	tabWidget->addTab(logTab,              tr("Logging"));
	tabWidget->addTab(delilahRawTab,       tr("Raw Platform access"));
	tabWidget->addTab(configTab,           tr("Preferences"));
	tabWidget->addTab(delilahOldTab,       tr("Old MR Operations"));

	mainLayout->addWidget(tabWidget);
	mainLayout->addWidget(quit);

	if ((userP == NULL) || ((userP->permissions & UpSeeLogs) == 0))
	{
		LM_W(("disabling Log Tab (user: %p)", userP));
		logTab->setDisabled(true);
	}
	if ((userP == NULL) || ((userP->permissions & UpRawPlatformAccess) == 0))
		delilahRawTab->setDisabled(true);

	window->setLayout(mainLayout);

	logReceiverInit(LOG_MESSAGE_PORT);

	LM_T(LmtQtTimer, ("Starting timer for Network polling"));
	startTimer(1000);  // 1 second timer (was 10 ms)
}



/* ****************************************************************************
*
* logReceiverInit - 
*/
void TabManager::logReceiverInit(unsigned short port)
{
	struct sockaddr_in  sAddr;

	logSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (logSocket == -1)
	{
		new Popup("Internal Error", "No log will be available.\nSee log file for more info");
		LM_E(("socket: %s", strerror(errno)));
		return;
	}

	memset((char*) &sAddr, 0, sizeof(sAddr));
	sAddr.sin_family = AF_INET;
	sAddr.sin_port = htons(port);
	sAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(logSocket, (struct sockaddr*) &sAddr, sizeof(sAddr)) == -1)
	{
		new Popup("Internal Error", "No log will be available.\nSee log file for more info");
		LM_E(("bind: %s", strerror(errno)));
		::close(logSocket);
		logSocket = -1;
        return;
	}
}



/* ****************************************************************************
*
* TabManager::timerEvent - 
*/
void TabManager::timerEvent(QTimerEvent* e)
{
	static struct timeval  lastShow = { 0, 0 };
	struct timeval         now;

	gettimeofday(&now, NULL);
	if (now.tv_sec - lastShow.tv_sec > 3)
	{
		if (networkP != NULL)
			networkP->endpointListShow("periodic");

		starterListShow("periodic");

		lastShow = now;
	}



	//
	// Poll Samson Network modules
	//
	if (networkP != NULL)
		networkP->poll();


	//
	// Read Log messages from other Samson processes
	//
	if (logSocket != -1)
		logReceive();


	//
	// Any InfoWin that needs to be killed ?
	//
	if (infoWin != NULL)
	{
		if ((now.tv_sec > infoWin->dieAt.tv_sec) || ((now.tv_sec == infoWin->dieAt.tv_sec) && (now.tv_usec >= infoWin->dieAt.tv_usec)))
		{
			delete infoWin;
			infoWin = NULL;
		}
	}
}



/* ****************************************************************************
*
* logReceive - 
*/
void TabManager::logReceive(void)
{
	struct sockaddr_in        sAddr;
	socklen_t                 sAddrLen = sizeof(sAddr);
	int                       flags;
	ssize_t                   nb;
	unsigned int              tot;
	ss::Message::LogLineData  logLine;
	ss::Message::Header       header;
	char*                     buf;
	unsigned int              bufLen;

	flags = MSG_DONTWAIT;

	while (1)
	{
		//
		// 1. Read header
		//
		tot    = 0;
		bufLen = sizeof(header);
		buf    = (char*) &header;
		memset(buf, 0, bufLen);

		while (tot < bufLen)
		{
			nb = recvfrom(logSocket, &buf[tot], bufLen - tot, flags, (struct sockaddr*) &sAddr, &sAddrLen);
			if (nb == -1)
			{
				if ((errno == EAGAIN) && (tot == 0))
					return;

				new Popup("Internal Error", "No log lines from other processes will be available.\nSee local samsonSupervisor log file for more info.");
				LM_E(("recvfrom: %s", strerror(errno)));
				logSocket = -1;
				LM_TODO(("Inform processes that the logging mechanism has stopped"));
				return;
			}
			else if (nb == 0)
				LM_E(("recvfrom returned 0 bytes ..."));

			tot += nb;
		}



		//
		// Read data
		//
		tot    = 0;
		bufLen = header.dataLen;
		buf    = (char*) &logLine;
		memset(buf, 0, bufLen);
		
		while (tot < bufLen)
		{
			nb = recvfrom(logSocket, &buf[tot], bufLen - tot, flags, (struct sockaddr*) &sAddr, &sAddrLen);
			if (nb == -1)
			{
				if ((errno == EAGAIN) && (tot == 0))
				{
					LM_E(("No data ..."));
					return;
				}

				new Popup("Internal Error", "No log lines from other processes will be available.\nSee local samsonSupervisor log file for more info.");
				LM_E(("recvfrom: %s", strerror(errno)));
				logSocket = -1;
				LM_TODO(("Inform processes that the logging mechanism has stopped"));
				return;
			}
			else if (nb == 0)
				LM_E(("recvfrom returned 0 bytes ..."));

			tot += nb;
		}

		if ((tot == bufLen) && (header.magic == 0xFEEDC0DE))
		{
			logTab->logLineInsert(&sAddr, &header, &logLine);
		}
		else
			LM_W(("skipping log line as its garbage ..."));
	}
}
