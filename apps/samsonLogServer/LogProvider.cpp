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

#include "globals.h"            // mainLayout, idleLabel
#include "logProviderList.h"    // logProviderRemove
#include "LogProvider.h"        // Own interface



/* ****************************************************************************
*
* LogProvider::LogProvider
*/
LogProvider::LogProvider(const char* name, const char* host, int fd)
{
	this->name   = strdup(name);
	this->host   = strdup(host);
	this->fd     = fd;
	this->folded = false;
};



/* ****************************************************************************
*
* ~LogProvider::LogProvider
*/
LogProvider::~LogProvider()
{
	LM_M(("Freeing this->name"));
	free(this->name);
	LM_M(("Freeing this->host"));
	free(this->host);
	
	LM_M(("Closing this->fd"));
	close(this->fd);
}



/* ****************************************************************************
*
* LogProvider::connectionClosed
*/
void LogProvider::connectionClosed(void)
{
	LM_M(("setting state text to 'dead'"));
	stateLabel->setText(QString("dead"));
	stateLabel->show();
	LM_M(("set state text to 'dead'"));
	close(fd);
	fd = -1;
}



/* ****************************************************************************
*
* LogProvider::fold
*/
void LogProvider::fold(void)
{
	LM_M(("Folding/Unfolding Log Provider List"));

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
	LM_M(("Clearing Log Provider List"));
	list->clear();
}



/* ****************************************************************************
*
* LogProvider::remove
*/
void LogProvider::remove(void)
{
	LM_M(("Removing Log Provider List"));
	logProviderRemove(this);
}
