/* ****************************************************************************
*
* FILE                     LogTab.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 02 2011
*
*/
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

#include "logMsg.h"             // LM_*

#include "LogTab.h"             // Own interface



/* ****************************************************************************
*
* LogTab::LogTab - 
*/
LogTab::LogTab(QWidget *parent) : QWidget(parent)
{
	QGridLayout*  mainLayout  = new QGridLayout(parent);
	int           row;
	int           column;

	for (row = 0; row < 10; row++)
	{
		QLabel* label;

		for (column = 0; column < 5; column++)
		{
			char name[64];

			if (column == row)
				continue;

			snprintf(name, sizeof(name), "row %d, col %d", row, column);
			label = new QLabel(tr(name));
			mainLayout->addWidget(label, row, column);
		}
	}
	
	mainLayout->setColumnStretch(column, 100);
	mainLayout->setRowStretch(row, 100);
	mainLayout->setColumnMinimumWidth(0, 300);
	mainLayout->setRowMinimumHeight(0, 300);
	setLayout(mainLayout);
}
