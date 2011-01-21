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

#include "Popup.h"              // Own interface



Popup::Popup(const char* title, const char* text)
{
	QVBoxLayout*      layout;
	QLabel*           label;
	QSize             size;
	int               screenWidth;
	int               screenHeight;
	int               x;
	int               y;
	QDesktopWidget*   desktop = QApplication::desktop();
	QDialogButtonBox* buttonBox;

	setModal(true);

	layout    = new QVBoxLayout();
	label     = new QLabel(text);
	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

	setWindowTitle(title);

	layout->addWidget(label);
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
