#ifndef DELILAH_TAB_H
#define DELILAH_TAB_H

/* ****************************************************************************
*
* FILE                     DelilahTab.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 02 2011
*
*/
#include <QObject>
#include <QWidget>
#include <QGridLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>

#include "Delilah.h"            // ss::Delilah
#include "DelilahConsole.h"     // ss::DelilahConsole



/* ****************************************************************************
*
* DelilahTab -
*/
class DelilahTab : public QWidget
{
	Q_OBJECT

public:
	DelilahTab(const char* name, QWidget *parent = 0);

private slots:
	void send();

public:
	QTextEdit*    output;

private:
	QGridLayout*  mainLayout;
	QLineEdit*    input;
	QPushButton*  sendButton;
};

#endif
