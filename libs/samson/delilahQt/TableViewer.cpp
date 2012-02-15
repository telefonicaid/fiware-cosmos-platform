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
    layout = new QVBoxLayout(this);
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
    if (!nameLabels.empty())
    {
    
        nameLabels.clear();
        valueLabels.clear();
    //Labels were already created. Just renew the values.
/*
        for (unsigned int i = 0; i < nameLabels.size(); i++)
        {
            DataValue val;
            val.name = QString(names[i].c_str());
            val.value = QString(values[i].c_str());
            data[i] = val;
            //valueLabels[i]->setText(val.value);
        }
  */
    }
  //  else
    {

        //First time. Create the widgets
        for (unsigned int i = 0; i < names.size(); i++)
        {
            DataValue val;
            val.name = QString(names[i].c_str());
            val.value = QString(values[i].c_str());
            data.push_back(val);
            QLabel* nameLabel =  new QLabel(groupBox);
            nameLabel->setText(val.name);
            nameLabels.push_back(nameLabel);
            QLabel* valueLabel =  new QLabel(groupBox);
            valueLabel->setText(val.value);
            valueLabels.push_back(valueLabel);
            layout->addWidget(nameLabel);
            layout->addWidget(valueLabel);
            groupBox->setLayout(layout);
            
            
            this->show();
            groupBox->show();
        }
        layout->activate();
    
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
