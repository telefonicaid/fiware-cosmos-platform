/*
 * FILE:		LoadExistingOperationDlg.cpp
 *
 * AUTHOR:		Anna Wojdel
 *
 * DESCRIPTION:
 *
 */

#include "LoadExistingOperationDlg.h"
#include "DelilahQtApp.h"
#include "Operation.h"
#include "Misc.h"

void LoadExistingOperationDlg::initNameSelectionWidget()
{
	DelilahQtApp* app = (DelilahQtApp*)qApp;
	QList<Operation*> operations = app->getOperations();

	QStringList names;
	for(int i=0; i<operations.size(); i++ )
		names << operations.at(i)->getName();

	arrangeNamesInTreeWidget(ui.nameTreeWidget, name_column_number, names);
	if(ui.nameTreeWidget->topLevelItemCount()>0)
		ui.nameTreeWidget->setCurrentItem(ui.nameTreeWidget->topLevelItem(0));
}
