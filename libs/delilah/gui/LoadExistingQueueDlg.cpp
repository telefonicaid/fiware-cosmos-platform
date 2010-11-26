/*
 * FILE:		LoadExistingQueueDlg.cpp
 *
 * AUTHOR:		Anna Wojdel
 *
 * DESCRIPTION:
 *
 */

#include <iostream>

#include "LoadExistingQueueDlg.h"
#include "DelilahQtApp.h"
#include "Misc.h"

LoadExistingQueueDlg::LoadExistingQueueDlg(QWidget* parent)
    : QDialog(parent), name_column_number(0)
{
	ui.setupUi(this);
	connect(ui.nameTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(setQueueName(QTreeWidgetItem*)));

	show_error = false;
	connect(ui.nameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(cancelError()));

	// initialize queues in tree widget
	initializeQueueTree();

}

LoadExistingQueueDlg::~LoadExistingQueueDlg()
{

}

void LoadExistingQueueDlg::initializeQueueTree()
{
	DelilahQtApp* app = (DelilahQtApp*)qApp;
	QList<Queue*> queues = app->getQueues();

	QStringList names;
	for(int i=0; i<queues.size(); i++ )
		names << queues.at(i)->getName();

	arrangeNamesInTreeWidget(ui.nameTreeWidget, name_column_number, names);

	if(ui.nameTreeWidget->topLevelItemCount()>0)
		ui.nameTreeWidget->setCurrentItem(ui.nameTreeWidget->topLevelItem(0));
}


void LoadExistingQueueDlg::setQueueName(QTreeWidgetItem* item)
{
	QString name("");

	// Set name of the queue in line edit only if the selected item
	// represents a queue (doesn't have any children).
	if(item->childCount()==0)
	{
		name = item->text(name_column_number);
		QTreeWidgetItem* parent = item->parent();
		while(parent)
		{
			name.prepend(parent->text(name_column_number) + ".");
			parent = parent->parent();
		};
	}

	ui.nameLineEdit->setText(name);
}

void LoadExistingQueueDlg::accept()
{
	if ( ui.nameLineEdit->text().isEmpty() )
	{
		// TODO: Replace with correct message and icon
		std::cout << "Name is not set correctly\n";
		ui.errorPixmapLabel->setPixmap(QPixmap(QString::fromUtf8(":/icons/new_process.png")));
		ui.errorLabel->setText("Queue is not selected");
		show_error = true;
	}
	else
		QDialog::accept();
}

void LoadExistingQueueDlg::cancelError()
{
	if (show_error)
	{
		ui.errorPixmapLabel->setPixmap(QPixmap());
		ui.errorLabel->setText("");
		show_error = false;
	}
}
