#ifndef CONFIG_WINDOW_H
#define CONFIG_WINDOW_H

/* ****************************************************************************
*
* FILE                     ConfigWindow.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 25 2011
*
*/
#include <QDialog>

#include "Endpoint.h"           // Endpoint



/* ****************************************************************************
*
* ConfigWindow - 
*/
class ConfigWindow : public QDialog
{
public:
	ConfigWindow(ss::Endpoint* endpoint);

private:
	ss::Endpoint* endpoint;
};

#endif
