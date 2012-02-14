/* ****************************************************************************
*
* FILE                     TableViewer.cpp
*
* DESCRIPTION			   Widget to visualize samson queues' data
*
*/

#include "TableViewer.h"
#include <iostream>

namespace samson{

TableViewer::TableViewer(QWidget* parent, std::string title): QWidget(parent)
{
    groupBox = new QGroupBox(this);
    setTitle(title);
    layout = new QGridLayout(this);
    groupBox->setLayout(layout);

}

bool TableViewer::setData(std::vector<std::string> names, std::vector<std::string> values)
{
    if(names.size() != values.size()) return false;
    
    //If the data set is not empty, delete all 
    if (!data.empty())
    {
        data.clear();
    }
    
    //copy the data
    for (unsigned int i = 0; i < names.size(); i++)
    {
        DataValue val;
        val.name = QString(names[i].c_str());
        val.value = QString(values[i].c_str());
        data.push_back(val);
        QLabel* nameLabel =  new QLabel(groupBox);
        QLabel* valueLabel =  new QLabel(groupBox);
        nameLabels.push_back(nameLabel);
        valueLabels.push_back(valueLabel);
        layout->addWidget(nameLabel, i , 0, Qt::AlignLeft);
        layout->addWidget(valueLabel, i , 1);
    }
    
    return true;
}

void TableViewer::updateData(std::vector<std::string> values)
{
    if (values.size() != data.size()) return; 
    for(unsigned int i = 0; i < values.size(); i++)
    {
        data[i].value = QString(values[i].c_str());
        
    }
}

void TableViewer::setTitle(std::string title)
{
    groupBox->setTitle(QString(title.c_str()));
}

} //namespace
