#ifndef POPUP_H
#define POPUP_H

/* ****************************************************************************
*
* FILE                     Popup.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 21 2011
*
*/
#include <QObject>
#include <QDialog>



/* ****************************************************************************
*
* Popup - 
*/
class Popup : public QDialog
{
	Q_OBJECT

public:
   Popup(const char* title, const char* text, bool die = false, int type = 0);

private slots:
	void die(void);
};

#endif
