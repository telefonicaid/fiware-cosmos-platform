/* ****************************************************************************
*
* FILE                     LogProvider.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 21 2011
*
*/
#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // LMT_*

#include "Endpoint.h"           // Endpoint
#include "globals.h"            // mainLayout, idleLabel
#include "logProviderList.h"    // logProviderRemove
#include "LogProvider.h"        // Own interface



/* ****************************************************************************
*
* LogProvider::LogProvider
*/
LogProvider::LogProvider(ss::Endpoint* ep, const char* name, const char* host, int fd)
{
	this->name     = strdup(name);
	this->host     = strdup(host);
	this->state    = strdup("connected");
	this->fd       = fd;
	this->folded   = false;
	this->endpoint = ep;
};



/* ****************************************************************************
*
* ~LogProvider::LogProvider
*/
LogProvider::~LogProvider()
{
	free(this->name);
	free(this->host);
	
	close(this->fd);
	this->endpoint->state = ss::Endpoint::Disconnected;
	this->endpoint->rFd   = -1;
	this->endpoint->wFd   = -1;
}



/* ****************************************************************************
*
* LogProvider::connectionClosed
*/
void LogProvider::connectionClosed(void)
{
	stateLabel->setText(QString("dead"));
	stateLabel->show();

	close(fd);
	fd = -1;
}



/* ****************************************************************************
*
* LogProvider::fold
*/
void LogProvider::fold(void)
{
	LM_T(LmtLogProvider, ("Folding/Unfolding Log Provider List"));

	if (folded == true)
	{
		folded = false;
		foldButton->setText("hide");
		list->show();
	}
	else
	{
		folded = true;
		foldButton->setText("show");
        list->hide();
	}
}



/* ****************************************************************************
*
* LogProvider::clear
*/
void LogProvider::clear(void)
{
	LM_T(LmtLogProvider, ("Clearing Log Provider List"));
	list->clear();
}



/* ****************************************************************************
*
* LogProvider::remove
*/
void LogProvider::remove(void)
{
	LM_T(LmtLogProvider, ("Removing Log Provider List"));
	logProviderRemove(this);
}



/* ****************************************************************************
*
* LogProvider::pause
*/
void LogProvider::pause(void)
{
	if (toBottom == true)
	{
		toBottom = false;
		pauseButton->setText("unpause");
	}
	else
	{
		toBottom = true;
		pauseButton->setText("pause");
	}
}



/* ****************************************************************************
*
* LogProvider::stop
*/
void LogProvider::stop(void)
{
	if (endpoint != NULL)
	{
		networkP->endpointRemove(endpoint, "stop pressed");
		endpoint = NULL;

		delete stopButton;
		delete pauseButton;

		stopButton  = NULL;
		pauseButton = NULL;
	}
}
