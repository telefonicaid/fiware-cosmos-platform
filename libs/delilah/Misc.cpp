/*
 * FILE:		Misc.cpp
 *
 * AUTHOR:		ania
 *
 * DESCRIPTION:
 *
 */

#include "Misc.h"

void arrangeNamesInTreeWidget(QTreeWidget* widget, unsigned int column, QStringList names)
{
	for(int i=0; i<names.size(); i++ )
	{
		QStringList name_parts = names.at(i).split(".");

		QTreeWidgetItem* parent_item = 0;
		for(int j=0; j<name_parts.size(); j++)
		{
			QString current_part_name = name_parts[j];
			QTreeWidgetItem* found = 0;

			// Special case for top level
			if(j==0)
			{
				for(int k=0; k<widget->topLevelItemCount(); k++)
				{
					if(widget->topLevelItem(k)->text(column)==current_part_name)
					{
						found = widget->topLevelItem(k);
						break;
					}
				}
				if(found==0)
				{
					found = new QTreeWidgetItem(widget);
					found->setText(column, current_part_name);
				}

			}
			else
			{
				assert(parent_item!=0);
				for(int k=0; k<parent_item->childCount(); k++)
				{
					if(parent_item->child(k)->text(column)==current_part_name)
					{
						found = parent_item->child(k);
						break;
					}
				}
				if (found==0)
				{
					found = new QTreeWidgetItem(parent_item);
					found->setText(column, current_part_name);
				}
			}

			parent_item = found;
		}
	}
}
