/*
 * FILE:		CreateKVQueueDlg.cpp
 *
 * AUTHOR:		Anna Wojdel
 *
 * DESCRIPTION:
 *
 */
#include <iostream>

#include "CreateKVQueueDlg.h"
#include "DelilahQtApp.h"
#include "DataType.h"
#include "Misc.h"

CreateKVQueueDlg::CreateKVQueueDlg(QWidget *parent)
    : QDialog(parent), name_column_number(0)
{
	ui.setupUi(this);

	// initialize data types tree list
	initializeDataTypeTree();

	error_visible = false;
	connect(ui.nameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(cancelError()));
	connect(ui.keyLineEdit, SIGNAL(textChanged(QString)), this, SLOT(cancelError()));
	connect(ui.valueLineEdit, SIGNAL(textChanged(QString)), this, SLOT(cancelError()));

}

CreateKVQueueDlg::~CreateKVQueueDlg()
{

}

void CreateKVQueueDlg::initializeDataTypeTree()
{
	DelilahQtApp* app = (DelilahQtApp*)qApp;
	QList<DataType*> data_types = app->getDataTypes();

	QStringList names;
	for(int i=0; i<data_types.size(); i++)
		names << data_types.at(i)->getName();

	arrangeNamesInTreeWidget(ui.dataTypeTreeWidget, name_column_number, names);
	if(ui.dataTypeTreeWidget->topLevelItemCount()>0)
		ui.dataTypeTreeWidget->setCurrentItem(ui.dataTypeTreeWidget->topLevelItem(0));
}

/*
 * Sets the name of the selected data type in key line edit
 */
void CreateKVQueueDlg::selectKeyButtonClicked()
{
	QString key = getSelectedDataTypeName();

	ui.keyLineEdit->clear();
	if(key.isEmpty())
		showError("No data type selected.");
	else
		ui.keyLineEdit->setText(key);
}

/*
 * Sets the name of the selected data type in value line edit
 */
void CreateKVQueueDlg::selectValueButtonClicked()
{
	QString value = getSelectedDataTypeName();

	ui.valueLineEdit->clear();
	if(value.isEmpty())
		showError("No data type selected.");
	else
		ui.valueLineEdit->setText(value);
}


/*
 * Return name of the selected data type or null QString if no data type is selected
 * (there is no item selected or selected item is not "final" for any data type)
 */
QString CreateKVQueueDlg::getSelectedDataTypeName()
{
	QString name;

	QTreeWidgetItem* item = ui.dataTypeTreeWidget->currentItem();
	if(item && item->childCount()==0)
	{
		name = item->text(name_column_number);
		QTreeWidgetItem* parent = item->parent();
		while(parent)
		{
			name.prepend(parent->text(name_column_number) + ".");
			parent = parent->parent();
		}
	}
	return name;
}

void CreateKVQueueDlg::showError(QString error)
{
	ui.errorPixmapLabel->setPixmap(QPixmap(QString::fromUtf8(":/icons/error.png")));
	ui.errorLabel->setText(error);
	error_visible = true;
}

void CreateKVQueueDlg::cancelError()
{
	if (error_visible)
	{
		ui.errorPixmapLabel->setPixmap(QPixmap());
		ui.errorLabel->setText("");
		error_visible = false;
	}
}

void CreateKVQueueDlg::accept()
{
	DelilahQtApp* app = (DelilahQtApp*)qApp;

	QString validation;
	if(ui.nameLineEdit->text().isEmpty())
		validation = "Queue name is empty.";
	else if (ui.keyLineEdit->text().isEmpty())
		validation = "Key type is not selected.";
	else if (ui.valueLineEdit->text().isEmpty())
		validation = "Value type is not selected.";
	else
		validation = app->validateNewQueueName(ui.nameLineEdit->text());

	if ( validation.isNull() )
		QDialog::accept();
	else
		showError(validation);
}
