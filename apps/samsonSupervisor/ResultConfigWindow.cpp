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
#include <QFileDialog>
#include <QLineEdit>
#include <QPushButton>

#include "logMsg.h"             // LM_X, ...
#include "traceLevels.h"        // Trace Levels

#include "globals.h"            // delilahConsole, connectionMgr
#include "DelilahResult.h"      // DelilahResult
#include "ResultConfigWindow.h" // Own interface



/* ****************************************************************************
*
* win - 
*/
static ResultConfigWindow* win = NULL;


#if 0
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

#endif

/* ****************************************************************************
*
* ResultConfigWindow::ResultConfigWindow - 
*/
ResultConfigWindow::ResultConfigWindow(DelilahResult* result)
{
	QGridLayout*      layout;
    QLabel*           imageLabel;
    QPixmap*          image;

	char              textV[128];
	QLabel*           title;
	QFont             titleFont("Times", 20, QFont::Normal);

	QLabel*           displayNameLabel;
	QLabel*           inTypeLabel;
	QLabel*           resultFileNameLabel;
	QPushButton*      resultFileNameBrowseButton;

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

	image      = new QPixmap("images/Result.png");
	imageLabel = new QLabel();
	imageLabel->setPixmap(*image);

	snprintf(textV, sizeof(textV), "Configuring Result '%s'", result->displayName);
	title = new QLabel(textV);
	title->setFont(titleFont);
	
	displayNameLabel = new QLabel("Display Name");
	displayNameInput = new QLineEdit();
	displayNameInput->setText(result->displayName);

	inTypeLabel = new QLabel("Incoming Type");
	inTypeCombo = new QComboBox();

	resultFileNameLabel        = new QLabel("Result File");
	resultFileNameInput        = new QLineEdit();
	resultFileNameBrowseButton = new QPushButton("Browse");
	
	resultFileNameInput->setText(result->resultFileName);

	connect(resultFileNameBrowseButton, SIGNAL(clicked()), this, SLOT(browse()));

	//delilahConsole->writeCallbackSet(dataTypesTextReceiver);
	//delilahConsole->evalCommand("datas");
	
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

    layout->addWidget(imageLabel,                  0, 0, 6, 1);
	layout->addWidget(title,                       0, 1, 1, 2);
	layout->addWidget(displayNameLabel,            1, 1);
	layout->addWidget(displayNameInput,            1, 2);
    layout->addWidget(resultFileNameLabel,         2, 1);
    layout->addWidget(resultFileNameInput,         2, 2, 1, 1);
    layout->addWidget(resultFileNameBrowseButton,  2, 3);
	layout->addWidget(inTypeLabel,                 3, 1);
	layout->addWidget(inTypeCombo,                 3, 2);
	layout->addWidget(noOfIncomingLabel,           4, 1);
	layout->addWidget(realNameLabel,               5, 1);
	layout->addWidget(buttonBox,                   6, 1, 1, 2);

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

	cP = (char*) resultFileNameInput->text().toStdString().c_str();
	if (cP != NULL)
	   result->resultFileNameSet(cP);
	
	result->inTypeSet(inTypeCombo->currentText().toStdString().c_str(), inTypeCombo->currentIndex());

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



/* ****************************************************************************
*
* browse
*/
void ResultConfigWindow::browse(void)
{
	QString fileName;

	LM_T(LmtMouseEvent, ("BROWSE pressed in Result Config Dialog"));

	fileName = QFileDialog::getOpenFileName(this, tr("Result File"), "/");
	if (fileName != NULL)
	{
		result->resultFileNameSet(fileName.toStdString().c_str());
		resultFileNameInput->setText(result->resultFileName);
	}
}
