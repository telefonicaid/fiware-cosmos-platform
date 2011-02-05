/* ****************************************************************************
*
* FILE                     ResultConfigWindow.cpp
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
#include <QComboBox>

#include "logMsg.h"             // LM_X, ...
#include "traceLevels.h"        // Trace Levels
#include "globals.h"            // qtAppRunning, ...

#include "DelilahResult.h"       // DelilahResult
#include "ResultConfigWindow.h"  // Own interface



/* ****************************************************************************
*
* win - 
*/
static ResultConfigWindow* win = NULL;



/* ****************************************************************************
*
* dataTypesTextReceiver - 
*/
static void dataTypesTextReceiver(const char* type, const char* text)
{
	char* cP;
	char* line;

	line = (char*) text;
	while ((cP = strchr(line, '\n')) != NULL)
	{
		*cP = 0;

		while (*line == ' ')
			++line;
		while (line[strlen(line) - 1] == ' ')
			line[strlen(line) - 1] = 0;

		if ((line[0] != 'D') && (line[0] != '-') && (line[0] != 0))
		{
			char* basura;

			if ((basura = strstr(line, " Help coming soon")) != NULL)
				*basura = 0;

			LM_T(LmtResult, ("Adding item '%s'", line));
			win->inTypeCombo->addItem(QString(line));
		}
		line = &cP[1];
	}

	win->inTypeCombo->addItem(QString(line));

	LM_T(LmtResult, ("Setting inTypeCombo  to index %d", win->result->inTypeIndex));

	win->inTypeCombo->setCurrentIndex(win->result->inTypeIndex);
}



/* ****************************************************************************
*
* ResultConfigWindow::ResultConfigWindow - 
*/
ResultConfigWindow::ResultConfigWindow(DelilahResult* result)
{
	QGridLayout*      layout;

	char              textV[128];
	QLabel*           title;
	QFont             titleFont("Times", 20, QFont::Normal);

	QLabel*           displayNameLabel;
	QLabel*           inTypeLabel;

	QDialogButtonBox* buttonBox;

	QLabel*           realNameLabel;
	QLabel*           noOfIncomingLabel;

	QSize             size;
	int               screenWidth;
	int               screenHeight;
	int               x;
	int               y;
	QDesktopWidget*   desktop = QApplication::desktop();

	int               incoming;

	win = this;

	this->result = result;

	setModal(true);

	layout = new QGridLayout();

	snprintf(textV, sizeof(textV), "Configuring Result '%s'", result->displayName);
	title = new QLabel(textV);
	title->setFont(titleFont);
	
	displayNameLabel = new QLabel("Display Name");
	displayNameInput = new QLineEdit();
	displayNameInput->setText(result->displayName);

	inTypeLabel = new QLabel("Incoming Type");
	inTypeCombo = new QComboBox();

    delilahConsole->writeCallbackSet(dataTypesTextReceiver);
    delilahConsole->evalCommand("datas");
	
	incoming = connectionMgr->incomingConnections(result);
	snprintf(textV, sizeof(textV), "%d Incoming connections", incoming);
	noOfIncomingLabel = new QLabel(textV);

	snprintf(textV, sizeof(textV), "Platform Result name: '%s'", result->name);
	realNameLabel = new QLabel(textV);

	QPushButton* okButton     = new QPushButton("OK");
	QPushButton* saveButton   = new QPushButton("Save");
	QPushButton* cancelButton = new QPushButton("Cancel");
	QPushButton* applyButton  = new QPushButton("Apply");

	buttonBox = new QDialogButtonBox();
	buttonBox->addButton(okButton,     QDialogButtonBox::AcceptRole);
	buttonBox->addButton(saveButton,   QDialogButtonBox::ActionRole);
	buttonBox->addButton(applyButton,  QDialogButtonBox::ActionRole);
	buttonBox->addButton(cancelButton, QDialogButtonBox::RejectRole);

	connect(okButton,     SIGNAL(clicked()), this, SLOT(ok()));
	connect(saveButton,   SIGNAL(clicked()), this, SLOT(save()));
	connect(applyButton,  SIGNAL(clicked()), this, SLOT(apply()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));

	setWindowTitle("Samson Result Configuration");

	layout->addWidget(title,             0, 0, 1, 2);
	layout->addWidget(displayNameLabel,  1, 0);
	layout->addWidget(displayNameInput,  1, 1);
	layout->addWidget(inTypeLabel,       2, 0);
	layout->addWidget(inTypeCombo,       2, 1);
	layout->addWidget(noOfIncomingLabel, 5, 0);
	layout->addWidget(realNameLabel,     7, 0);
	layout->addWidget(buttonBox,         9, 0, 1, 2);

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
* die
*/
void ResultConfigWindow::die(void)
{
	LM_X(1, ("Dying popup received OK press event"));
}



/* ****************************************************************************
*
* ok
*/
void ResultConfigWindow::ok(void)
{
	LM_T(LmtMouseEvent, ("OK pressed in Result Config Dialog"));
	
	save();
	delete this;
}



/* ****************************************************************************
*
* save
*/
void ResultConfigWindow::save(void)
{
	char* cP;

	LM_T(LmtMouseEvent, ("SAVE pressed in Result Config Dialog"));

	cP = (char*) displayNameInput->text().toStdString().c_str();
	if (cP != NULL)
		result->displayNameSet(cP);

	result->inTypeSet(inTypeCombo->currentText().toStdString().c_str());

	win->result->inTypeIndex  = inTypeCombo->currentIndex();
	
	LM_T(LmtResult, ("Set inType  to '%s' (current index: %d)", result->inType,  win->result->inTypeIndex));
}



/* ****************************************************************************
*
* apply
*/
void ResultConfigWindow::apply(void)
{
	LM_T(LmtMouseEvent, ("APPLY pressed in Result Config Dialog"));
	save();
}



/* ****************************************************************************
*
* cancel
*/
void ResultConfigWindow::cancel(void)
{
	LM_T(LmtMouseEvent, ("CANCEL pressed in Result Config Dialog"));
	delete this;
}
