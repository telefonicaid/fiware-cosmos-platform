/* ****************************************************************************
*
* FILE                     ConfigTab.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 02 2011
*
*/
#include <QWidget>
#include <QGridLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QTableWidget>

#include "logMsg.h"             // LM_*

#include "globals.h"            // tabManager
#include "ConfigTab.h"          // Own interface



/* ****************************************************************************
*
* ConfigTab::ConfigTab - 
*/
ConfigTab::ConfigTab(QWidget *parent) : QWidget(parent)
{
	QHBoxLayout*  mainLayout        = new QHBoxLayout(parent);
	QGroupBox*    connectBox        = new QGroupBox("Connection Type Checking");
	QVBoxLayout*  col1              = new QVBoxLayout;
	QVBoxLayout*  col2              = new QVBoxLayout;
	QVBoxLayout*  col3              = new QVBoxLayout;
	QVBoxLayout*  col4              = new QVBoxLayout;
	QVBoxLayout*  connectVBox       = new QVBoxLayout;
	QRadioButton* onButton          = new QRadioButton("Error on different types");
	QRadioButton* offButton         = new QRadioButton("Automatic type checking OFF");
	QRadioButton* leftButton        = new QRadioButton("Inherit left side out-type");
	QRadioButton* popupButton       = new QRadioButton("Popup to choose type");
	QLabel*       logFontSizeLabel  = new QLabel("Font size");
	QSlider*      logFontSizeSlider = new QSlider(Qt::Horizontal);
	QPushButton*  logClear          = new QPushButton("Clear Log Window");
	QPushButton*  logFit            = new QPushButton("Fit Log Window");
	QLabel*       mrOpTitle         = new QLabel("MR Operations");
	QLabel*       logTitle          = new QLabel("Logging");

	QFont         titleFont("Times", 12, QFont::Bold);

	autoConfigCBox = new QCheckBox("Config window on SceneItem creation");
	autoConfigCBox->setCheckState(Qt::Unchecked);

	setLayout(mainLayout);
	mainLayout->addLayout(col1);
	mainLayout->addSpacing(100);
	mainLayout->addLayout(col2);
	mainLayout->addSpacing(100);
	mainLayout->addLayout(col3);
	mainLayout->addSpacing(100);
	mainLayout->addLayout(col4);
	mainLayout->addStretch(500);
	
	col1->addWidget(mrOpTitle);
	col1->addSpacing(30);
	col1->addWidget(connectBox);
	col1->addSpacing(100);
	col1->addWidget(autoConfigCBox);
	col1->addStretch(500);

	col2->addWidget(logTitle);
	col2->addSpacing(30);
	col2->addWidget(logFontSizeLabel);
	col2->addWidget(logFontSizeSlider);
	col2->addWidget(logClear);
	col2->addWidget(logFit);
	col2->addStretch(500);

	logFontSizeLabel->setDisabled(true);
	logFontSizeSlider->setDisabled(true);

	logTitle->setFont(titleFont);
	mrOpTitle->setFont(titleFont);

	onButton->setChecked(true);

	connectVBox->addWidget(onButton);
	connectVBox->addWidget(offButton);
	connectVBox->addWidget(leftButton);
	connectVBox->addWidget(popupButton);
	connectVBox->addStretch(1);

	connect(onButton,    SIGNAL(clicked()), this, SLOT(typeCheckOn()));
	connect(offButton,   SIGNAL(clicked()), this, SLOT(typeCheckOff()));
	connect(leftButton,  SIGNAL(clicked()), this, SLOT(typeCheckLeft()));
	connect(popupButton, SIGNAL(clicked()), this, SLOT(typeCheckPopup()));
	connect(logClear,    SIGNAL(clicked()), this, SLOT(logViewClear()));
	connect(logFit,      SIGNAL(clicked()), this, SLOT(logViewFit()));

	connectBox->setLayout(connectVBox);
	connectBox->setFlat(false);
	connectBox->setFlat(false);
	connectBox->setFlat(false);

	typeCheck = On;
}



/* ****************************************************************************
*
* ConfigTab::typeCheckOn - 
*/
void ConfigTab::typeCheckOn(void)
{
	LM_W(("Type Check ON pressed"));
	typeCheck = On;
}



/* ****************************************************************************
*
* ConfigTab::typeCheckOff - 
*/
void ConfigTab::typeCheckOff(void)
{
	LM_W(("Type Check OFF pressed"));
	typeCheck = Off;
}



/* ****************************************************************************
*
* ConfigTab::typeCheckLeft - 
*/
void ConfigTab::typeCheckLeft(void)
{
	LM_W(("Type Check LEFT pressed"));
	typeCheck = Left;
}



/* ****************************************************************************
*
* ConfigTab::typeCheckPopup - 
*/
void ConfigTab::typeCheckPopup(void)
{
	LM_W(("Type Check POPUP pressed"));
	typeCheck = Popup;
}



/* ****************************************************************************
*
* ConfigTab::logViewClear - 
*/
void ConfigTab::logViewClear(void)
{
	tabManager->logTab->clear();
}



/* ****************************************************************************
*
* ConfigTab::logViewFit - 
*/
void ConfigTab::logViewFit(void)
{
	tabManager->logTab->tableWidget->resizeColumnsToContents();
}
