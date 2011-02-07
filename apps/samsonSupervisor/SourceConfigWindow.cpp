/* ****************************************************************************
*
* FILE                     SourceConfigWindow.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 05 2011
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

#include "DelilahSource.h"      // DelilahSource
#include "SourceConfigWindow.h" // Own interface



/* ****************************************************************************
*
* win - 
*/
static SourceConfigWindow* win = NULL;



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

			LM_T(LmtSource, ("Adding item '%s'", line));
			win->outTypeCombo->addItem(QString(line));
		}
		line = &cP[1];
	}

	win->outTypeCombo->addItem(QString(line));

	LM_T(LmtSource, ("Setting outTypeCombo to index %d", win->source->outTypeIndex));

	win->outTypeCombo->setCurrentIndex(win->source->outTypeIndex);
}



/* ****************************************************************************
*
* SourceConfigWindow::SourceConfigWindow - 
*/
SourceConfigWindow::SourceConfigWindow(DelilahSource* source)
{
	QGridLayout*      layout;

	char              textV[128];
	QLabel*           title;
	QFont             titleFont("Times", 20, QFont::Normal);

	QLabel*           displayNameLabel;
	QLabel*           outTypeLabel;

	QDialogButtonBox* buttonBox;

	QLabel*           realNameLabel;
	QLabel*           noOfOutgoingLabel;

	QSize             size;
	int               screenWidth;
	int               screenHeight;
	int               x;
	int               y;
	QDesktopWidget*   desktop = QApplication::desktop();

	int               outgoing;

	win = this;

	this->source = source;

	setModal(true);

	layout = new QGridLayout();

	snprintf(textV, sizeof(textV), "Configuring Source '%s'", source->displayName);
	title = new QLabel(textV);
	title->setFont(titleFont);
	
	displayNameLabel = new QLabel("Display Name");
	displayNameInput = new QLineEdit();
	displayNameInput->setText(source->displayName);

	outTypeLabel = new QLabel("Outgoing Type");
	outTypeCombo = new QComboBox();

    delilahConsole->writeCallbackSet(dataTypesTextReceiver);
    delilahConsole->evalCommand("datas");
	
	outgoing = connectionMgr->outgoingConnections(source, NULL);
	snprintf(textV, sizeof(textV), "%d Outgoing connections", outgoing);
	noOfOutgoingLabel = new QLabel(textV);
	
	snprintf(textV, sizeof(textV), "Platform Source name: '%s'", source->name);
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

	setWindowTitle("Samson Source Configuration");

	layout->addWidget(title,             0, 0, 1, 2);
	layout->addWidget(displayNameLabel,  1, 0);
	layout->addWidget(displayNameInput,  1, 1);
	layout->addWidget(outTypeLabel,      3, 0);
	layout->addWidget(outTypeCombo,      3, 1);
	layout->addWidget(noOfOutgoingLabel, 6, 0);
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
void SourceConfigWindow::die(void)
{
	LM_X(1, ("Dying popup received OK press event"));
}



/* ****************************************************************************
*
* ok
*/
void SourceConfigWindow::ok(void)
{
	LM_T(LmtMouseEvent, ("OK pressed in Source Config Dialog"));
	
	save();
	delete this;
}



/* ****************************************************************************
*
* save
*/
void SourceConfigWindow::save(void)
{
	char* cP;

	LM_T(LmtMouseEvent, ("SAVE pressed in Source Config Dialog"));

	cP = (char*) displayNameInput->text().toStdString().c_str();
	if (cP != NULL)
		source->displayNameSet(cP);

	source->outTypeSet(outTypeCombo->currentText().toStdString().c_str());
	win->source->outTypeIndex = outTypeCombo->currentIndex();
	
	LM_T(LmtSource, ("Set outType to '%s' (current index: %d)", source->outType, win->source->outTypeIndex));
}



/* ****************************************************************************
*
* apply
*/
void SourceConfigWindow::apply(void)
{
	LM_T(LmtMouseEvent, ("APPLY pressed in Source Config Dialog"));
	save();
}



/* ****************************************************************************
*
* cancel
*/
void SourceConfigWindow::cancel(void)
{
	LM_T(LmtMouseEvent, ("CANCEL pressed in Source Config Dialog"));
	delete this;
}
