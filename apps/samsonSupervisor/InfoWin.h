#ifndef INFOWIN_H
#define INFOWIN_H

/* ****************************************************************************
*
* FILE                     InfoWin.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 12 2011
*
*/
#include <time.h>               // struct timeval

#include <QObject>
#include <QDialog>




/* ****************************************************************************
*
* InfoWin - 
*/
class InfoWin : public QDialog
{
	Q_OBJECT

public:
	InfoWin(const char* title, const char* text, unsigned int secs = 3, unsigned int usecs = 0);

	struct timeval dieAt;
	struct timeval bornAt;
};

#endif
