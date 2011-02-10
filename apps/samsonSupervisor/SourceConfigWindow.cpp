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
#include <QFileDialog>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>

#include "logMsg.h"             // LM_X, ...
#include "traceLevels.h"        // Trace Levels

#include "globals.h"            // delilahConsole, connectionMgr
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
	QLabel*           imageLabel;
	QPixmap*          image;

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

	image      = new QPixmap("images/Bomba.png");
	imageLabel = new QLabel();
	imageLabel->setPixmap(*image);

	snprintf(textV, sizeof(textV), "Configuring Source '%s'", source->displayName);
	title = new QLabel(textV);
	title->setFont(titleFont);
	
	displayNameLabel = new QLabel("Display Name");
	displayNameInput = new QLineEdit();
	displayNameInput->setText(source->displayName);

	outTypeLabel = new QLabel("Outgoing Type");
	outTypeCombo = new QComboBox();

	sourceFileNameLabel        = new QLabel("Source File");
	sourceFileNameInput        = new QLineEdit();
	sourceFileNameBrowseButton = new QPushButton("Browse");
	fakeLabel                  = new QLabel("Fake Input");
	fakeButton                 = new QPushButton("Fake Source");
	sourceFileButton           = new QPushButton("File Source");
	fakeSizeSpinBox            = new QSpinBox();

	sourceFileNameInput->setText(source->sourceFileName);
	fakeSizeSpinBox->setValue(source->fakeSize);

	connect(sourceFileNameBrowseButton, SIGNAL(clicked()), this, SLOT(browse()));
	connect(fakeButton, SIGNAL(clicked()), this, SLOT(fake()));
	connect(sourceFileButton, SIGNAL(clicked()), this, SLOT(unfake()));

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

	layout->addWidget(imageLabel,                  0, 0, 9, 1);
	layout->addWidget(title,                       0, 2, 1, 2);
	layout->addWidget(displayNameLabel,            1, 1);
	layout->addWidget(displayNameInput,            1, 2);
	layout->addWidget(sourceFileNameLabel,         2, 1);
	layout->addWidget(sourceFileNameInput,         2, 2, 1, 1);
	layout->addWidget(sourceFileNameBrowseButton,  2, 3);
	layout->addWidget(fakeButton,                  2, 4);
	layout->addWidget(fakeLabel,                   2, 1);
	layout->addWidget(fakeSizeSpinBox,             2, 2);
	layout->addWidget(sourceFileButton,            2, 3);
	layout->addWidget(outTypeLabel,                3, 1);
	layout->addWidget(outTypeCombo,                3, 2);
	layout->addWidget(noOfOutgoingLabel,           6, 1);
	layout->addWidget(realNameLabel,               7, 1);
	layout->addWidget(buttonBox,                   9, 1, 1, 2);

	this->setLayout(layout);
	this->show();

	// Window Geometry
	size = this->size();

	screenWidth  = desktop->width();
	screenHeight = desktop->height();

	x = (screenWidth  - size.width())  / 2;
	y = (screenHeight - size.height()) / 2;

	this->move(x, y);

	fakeShow(source->faked);
	sourceFileShow(!source->faked);
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

	cP = (char*) sourceFileNameInput->text().toStdString().c_str();
	if (cP != NULL)
		source->sourceFileNameSet(cP);

	source->outTypeSet(outTypeCombo->currentText().toStdString().c_str());
	win->source->outTypeIndex = outTypeCombo->currentIndex();

	// faked already saved in DelilahSource
	source->fakeSize = fakeSizeSpinBox->value();

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



/* ****************************************************************************
*
* SourceConfigWindow::sourceFileShow - 
*/
void SourceConfigWindow::sourceFileShow(bool show)
{
	if (show == true)
	{
		sourceFileNameLabel->show();
		sourceFileNameInput->show();
		sourceFileNameBrowseButton->show();
		fakeButton->show();
	}
	else
	{
		sourceFileNameLabel->hide();
		sourceFileNameInput->hide();
		sourceFileNameBrowseButton->hide();
		fakeButton->hide();
	}
}



/* ****************************************************************************
*
* SourceConfigWindow::fakeShow - 
*/
void SourceConfigWindow::fakeShow(bool show)
{
	if (show == true)
	{
		sourceFileButton->show();
		fakeSizeSpinBox->show();
		fakeLabel->show();
	}
	else
	{
		sourceFileButton->hide();
		fakeSizeSpinBox->hide();
		fakeLabel->hide();
	}
}



/* ****************************************************************************
*
* fake
*/
void SourceConfigWindow::fake(void)
{
	LM_T(LmtMouseEvent, ("FAKE pressed in Source Config Dialog"));

	sourceFileShow(false);
	fakeShow(true);
	source->faked = true;
}



/* ****************************************************************************
*
* unfake
*/
void SourceConfigWindow::unfake(void)
{
	LM_T(LmtMouseEvent, ("UNFAKE pressed in Source Config Dialog"));

	fakeShow(false);
	sourceFileShow(true);
	source->faked = false;
}



/* ****************************************************************************
*
* browse
*/
void SourceConfigWindow::browse(void)
{
	QString fileName;

	LM_T(LmtMouseEvent, ("BROWSE pressed in Source Config Dialog"));

	fileName = QFileDialog::getOpenFileName(this, tr("Source File"), "/");
	if (fileName != NULL)
	{
		source->sourceFileNameSet(fileName.toStdString().c_str());
		sourceFileNameInput->setText(source->sourceFileName);
	}
}
