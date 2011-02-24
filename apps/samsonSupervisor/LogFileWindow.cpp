/* ****************************************************************************
*
* FILE                     LogFileWindow.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 24 2011
*
*/
#include <QApplication>
#include <QDesktopWidget>
#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>

#include "logMsg.h"             // LM_X, ...
#include "globals.h"            // qtAppRunning, ...
#include "Endpoint.h"           // ss::Endpoint
#include "LogFileWindow.h"      // Own interface



/* ****************************************************************************
*
* LogFileWindow::LogFileWindow - 
*/
LogFileWindow::LogFileWindow(ss::Endpoint* ep, char* text, bool old)
{
	QVBoxLayout*       layout;
	QPlainTextEdit*    textEdit;
	QFont              font("Courier", 10, QFont::Normal);
	char               title[128];

	QSize              size;
	int                screenWidth;
	int                screenHeight;
	int                x;
	int                y;
	QDesktopWidget*    desktop = QApplication::desktop();
	QDialogButtonBox*  buttonBox;

	if (old)
		snprintf(title, sizeof(title), "Old log file for %s@%s", ep->name.c_str(), ep->ip);
	else
		snprintf(title, sizeof(title), "Current log file for %s@%s", ep->name.c_str(), ep->ip);

	layout = new QVBoxLayout();

	textEdit = new QPlainTextEdit(text);
	textEdit->setFont(font);
	layout->addWidget(textEdit);
	textEdit->setReadOnly(true);
	this->setMinimumSize(800, 900);

	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

	setWindowTitle(title);
	layout->addWidget(buttonBox);

	this->setLayout(layout);
	this->show();

	// Window Geometry
	size = this->size();

	screenWidth  = desktop->width();
	screenHeight = desktop->height();

	x = (screenWidth  - size.width())  / 2;
	y = (screenHeight - size.height()) / 2;

	this->move(x, y);
}
