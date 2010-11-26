/*
 * FILE:		CreateKVQueueDlg.cpp
 *
 * AUTHOR:		Anna Wojdel
 *
 * DESCRIPTION:
 *
 */

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
}

