/* ****************************************************************************
*
* FILE                     Popup.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 21 2011
*
*/
#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QSize>
#include <QDesktopWidget>
#include <QPlainTextEdit>
#include <QListWidget>

#include "logMsg.h"             // LM_X, ...
#include "globals.h"            // qtAppRunning, ...
#include "Popup.h"              // Own interface



/* ****************************************************************************
*
* Popup::Popup - 
*/
Popup::Popup(const char* title, const char* text, bool die, int type)
{
	QVBoxLayout*       layout;

	QLabel*            label      = NULL;
	QPlainTextEdit*    textEdit   = NULL;
	QListWidget*       listWidget = NULL;

	QSize              size;
	int                screenWidth;
	int                screenHeight;
	int                x;
	int                y;
	QDesktopWidget*    desktop = QApplication::desktop();
	QDialogButtonBox*  buttonBox;

	setModal(true);

	layout    = new QVBoxLayout();

	if (type == 2)
	{
		char* cP;
		char* line;

		listWidget = new(QListWidget);
		line = (char*) text;
		while ((cP = strchr(line, '\n')) != NULL)
		{
			*cP = 0;
			if (strstr(line, "--------") == NULL)
				listWidget->addItem(QString(line));
			line = &cP[1];
		}
		listWidget->addItem(QString(line));
		layout->addWidget(listWidget);
		this->setMinimumSize(500, 600);
	}
	else if (type == 1)
	{
		textEdit = new QPlainTextEdit(text);
		layout->addWidget(textEdit);
		textEdit->setReadOnly(true);
		this->setMinimumSize(800, 900);
	}
	else
	{
		label = new QLabel(text);
		layout->addWidget(label);
	}

	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);

	if (die)
		connect(buttonBox, SIGNAL(accepted()), this, SLOT(die()));
	else
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

	if (qtAppRunning == false)
		qApp->exec();
}



/* ****************************************************************************
*
* die
*/
void Popup::die(void)
{
	exit(0);
}
