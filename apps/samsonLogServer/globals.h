#ifndef GLOBALS_H
#define GLOBALS_H

/* ****************************************************************************
*
* FILE                     globals.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 20 2011
*
*/
#include <QVBoxLayout>
#include <QLabel>

#include "Network.h"
#include "SamsonLogServer.h"



/* ****************************************************************************
*
* mainLayout - 
*/
extern QVBoxLayout*      mainLayout;
extern QLabel*           idleLabel;
extern ss::Network*      networkP;
extern SamsonLogServer*  samsonLogServer;

#endif
