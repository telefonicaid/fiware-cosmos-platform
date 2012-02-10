/* ****************************************************************************
*
* FILE                     QueueViewer.cpp
*
* DESCRIPTION			   Widget to visualize samson queues' data
*
*/

#include "QueueViewer.h"

QueueViewer::QueueViewer(QWidget* parent, std::string title): Qwidget(parent)
{
    groupBox = new QGroupBox(this);
    setTitle(title);
    layout = new QBoxLayout();
    groupBox->setLayout(layout);

}

bool QueueViewer::setData(std::vector<std::string> names, std::vector<std::string> values)
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
        val.value = QString(names[i].c_str());
        data.push_back(val);
    }
    
}

void QueueViewer::updateData(str::vector<std::string> values)
{
}

void setTitle(std::string title)
{
    groupBox->setTitle(QString(title.c_str()));
}
