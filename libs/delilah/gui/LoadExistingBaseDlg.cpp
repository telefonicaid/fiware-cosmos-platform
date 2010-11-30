/*
 * FILE:		LoadExistingBaseDlg.cpp
 *
 * AUTHOR:		Anna Wojdel
 *
 * DESCRIPTION:
 *
 */

#include "LoadExistingBaseDlg.h"

LoadExistingBaseDlg::LoadExistingBaseDlg(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	connect(ui.nameTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(setName(QTreeWidgetItem*)));

	error_visible = false;
	connect(ui.nameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(cancelError()));

	name_column_number = 0;
}

LoadExistingBaseDlg::~LoadExistingBaseDlg()
{

}

void LoadExistingBaseDlg::setName(QTreeWidgetItem* item)
{
	QString name("");

	// Set name in line edit only if the selected item represents an operation
	// (doesn't have any children).
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

void LoadExistingBaseDlg::showError(QString error)
{
	ui.errorPixmapLabel->setPixmap(QPixmap(QString::fromUtf8(":/icons/error.png")));
	ui.errorLabel->setText(error);
	error_visible = true;
}

void LoadExistingBaseDlg::cancelError()
{
	if (error_visible)
	{
		ui.errorPixmapLabel->setPixmap(QPixmap());
		ui.errorLabel->setText("");
		error_visible = false;
	}
}

void LoadExistingBaseDlg::accept()
{
	if ( ui.nameLineEdit->text().isEmpty() )
		showError("Name not selected.");
	else
		QDialog::accept();
}

