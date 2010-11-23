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

LoadExistingQueueDlg::LoadExistingQueueDlg(QWidget* parent)
    : QDialog(parent), name_column_number(0)
{
	ui.setupUi(this);
	connect(ui.nameTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(setQueueName(QTreeWidgetItem*)));

	show_error = false;
	connect(ui.nameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(cancelError()));

	// initialize queues tree list
	listQueues();
	if(ui.nameTreeWidget->topLevelItemCount()>0)
		ui.nameTreeWidget->setCurrentItem(ui.nameTreeWidget->topLevelItem(0));

}

LoadExistingQueueDlg::~LoadExistingQueueDlg()
{

}

void LoadExistingQueueDlg::listQueues()
{
	DelilahQtApp* app = (DelilahQtApp*)qApp;
	QList<Queue*> queues = app->getQueues();

	for(int i=0; i<queues.size(); i++ )
	{
		Queue* queue = queues[i];

		QStringList name_parts = queue->getName().split(".");
		QTreeWidgetItem* parent_item = 0;
		for(int j=0; j<name_parts.size(); j++)
		{
			QString current_part_name = name_parts[j];
			QTreeWidgetItem* found = 0;

			// Special case for top level
			if(j==0)
			{
				for(int k=0; k<ui.nameTreeWidget->topLevelItemCount(); k++)
				{
					if(ui.nameTreeWidget->topLevelItem(k)->text(name_column_number)==current_part_name)
					{
						found = ui.nameTreeWidget->topLevelItem(k);
						break;
					}
				}
				if(found==0)
				{
					found = new QTreeWidgetItem(ui.nameTreeWidget);
					found->setText(name_column_number, current_part_name);
				}

			}
			else
			{
				assert(parent_item!=0);
				for(int k=0; k<parent_item->childCount(); k++)
				{
					if(parent_item->child(k)->text(name_column_number)==current_part_name)
					{
						found = parent_item->child(k);
						break;
					}
				}
				if (found==0)
				{
					found = new QTreeWidgetItem(parent_item);
					found->setText(name_column_number, current_part_name);
				}
			}

			parent_item = found;
		}
	}
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
