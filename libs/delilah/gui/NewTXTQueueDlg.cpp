#include <iostream>

#include "NewTXTQueueDlg.h"

NewTXTQueueDlg::NewTXTQueueDlg(QList<QString> names, QWidget* parent)
	: QDialog(parent)
{
	existing_names = names;
	init();
}

void NewTXTQueueDlg::init()
{
	ui.setupUi(this);
	connect(ui.nameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setName(QString)));
}

bool NewTXTQueueDlg::nameValid(const QString &name)
{
	// TODO: Validate that name does not already exists
	// (compare with names in existing_names list)

	QString n;
	foreach(n, existing_names)
	{
		if(n.startsWith(name) )
			return false;
	}

	return true;
}

void NewTXTQueueDlg::setName(QString _name)
{
	// TODO: fix me - it's only temporal to test
	if (!nameValid(_name))
	{
		ui.errorPixmapLabel->setPixmap(QPixmap(QString::fromUtf8(":/icons/new_process.png")));
		ui.errorLabel->setText("Such name already exists.");
		name = "";
	}
	else
	{
		ui.errorPixmapLabel->setPixmap(QPixmap());
		ui.errorLabel->setText("");
		name = _name;
	}
}

void NewTXTQueueDlg::accept()
{
	if(name.isEmpty())
		// TODOD: show some message????
		std::cout << "Name is not set correctly\n";
	else
		QDialog::accept();
}

