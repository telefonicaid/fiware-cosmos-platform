/* ****************************************************************************
*
* FILE                     LoginWindow.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 04 2011
*
*/
#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QSize>
#include <QDesktopWidget>

#include "logMsg.h"             // LM_X, ...
#include "traceLevels.h"        // Trace Levels
#include "globals.h"            // qtAppRunning, ...

#include "Popup.h"              // Popup
#include "LoginWindow.h"        // Own interface



/* ****************************************************************************
*
* LoginWindow::LoginWindow - 
*/
LoginWindow::LoginWindow(void)
{
	QGridLayout*      layout;

	char              textV[128];
	QLabel*           title;
	QFont             titleFont("Times", 20, QFont::Normal);

	QLabel*           userNameLabel;
	QLabel*           passwordLabel;

	QDialogButtonBox* buttonBox;

	QSize             size;
	int               screenWidth;
	int               screenHeight;
	int               x;
	int               y;
	QDesktopWidget*   desktop = QApplication::desktop();

	tries = 0;

	setModal(true);

	layout = new QGridLayout();

	snprintf(textV, sizeof(textV), "Login to the Samson platform");
	title = new QLabel(textV);
	title->setFont(titleFont);
	
	userNameLabel = new QLabel("User");
	userNameInput = new QLineEdit();
	userNameInput->setText("");

	passwordLabel = new QLabel("Password");
	passwordInput = new QLineEdit();
	passwordInput->setText("");
	passwordInput->setEchoMode(QLineEdit::Password);

	QPushButton* okButton      = new QPushButton("OK");
	QPushButton* cancelButton  = new QPushButton("Cancel");
	QPushButton* guestButton   = new QPushButton("Enter as 'guest'");

	buttonBox = new QDialogButtonBox();
	buttonBox->addButton(okButton,     QDialogButtonBox::AcceptRole);
	buttonBox->addButton(cancelButton, QDialogButtonBox::RejectRole);
	buttonBox->addButton(guestButton,  QDialogButtonBox::AcceptRole);

	connect(okButton,     SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
	connect(guestButton,  SIGNAL(clicked()), this, SLOT(guest()));

	setWindowTitle("Enter the Samson platform");

	layout->addWidget(title,          0, 0, 1, 2);
	layout->addWidget(userNameLabel,  1, 0);
	layout->addWidget(userNameInput,  1, 1);
	layout->addWidget(passwordLabel,  2, 0);
	layout->addWidget(passwordInput,  2, 1);
	layout->addWidget(buttonBox,      3, 0, 1, 2);

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
	{
		qtAppRunning = true;
		qApp->exec();
	}
}



/* ****************************************************************************
*
* ok
*/
void LoginWindow::ok(void)
{
	char* userName;
	char* password;

	userName = strdup(userNameInput->text().toStdString().c_str());
	password = strdup(passwordInput->text().toStdString().c_str());

	if (userMgr->allowToEnter(userName, password) == true)
	{
		delete this;
		free(userName);
		free(password);
		qApp->exit();
		qtAppRunning = false;
		userP = userMgr->lookup(userName);
		return;
	}

	free(userName);
	free(password);

	if (++tries >= 3)
		new Popup("Samson Login", "Bad password entered three times.\nEntrance not allowed.", true);

	passwordInput->setText("");
}



/* ****************************************************************************
*
* cancel
*/
void LoginWindow::cancel(void)
{
	new Popup("Samson Login Cancelled", "You've decided not to give a user/password to enter the Samson platform.\nEntrance not allowed.", true);
}



/* ****************************************************************************
*
* guest
*/
void LoginWindow::guest(void)
{
	userP = userMgr->lookup("guest");
	delete this;
	qApp->exit();
	qtAppRunning = false;
}
