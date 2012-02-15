/* ****************************************************************************
*
* FILE                     QueueViewer.cpp
*
* DESCRIPTION			   Widget to visualize samson queues' data
*
*/

#include "QueueViewer.h"
#include <iostream>

namespace samson{

QueueViewer::QueueViewer(QWidget* parent, std::string title): QWidget(parent)
{
    QVBoxLayout* layout;
    groupBox = new QGroupBox(this);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    groupBox->setSizePolicy(sizePolicy);
        
    name = new QLabel("Name: ", groupBox);
    kvs = new QLabel("KVs: ", groupBox);
    kvssize = new QLabel("KVs Size: ", groupBox);
    size = new QLabel("Size: ", groupBox);
    size_locked = new QLabel("Size Locked: ", groupBox);
    size_on_disk = new QLabel("Size (on disk): ", groupBox);
    size_on_memory = new QLabel("Size (on memory): ", groupBox);
    kvs_rate = new QLabel("KVs Rate: ", groupBox);
    kvs_rate_size = new QLabel("KVs Rate Size: ", groupBox);
    rate = new QLabel("Rate: ", groupBox);
    rate_size = new QLabel("Rate Size: ", groupBox);

    if (data.name.empty())
    {
        setTitle("Empty Queue");
    }
    else
    {
        setTitle(data.name);
    }
    
    layout = new QVBoxLayout(this);
    layout->addWidget(name);
    layout->addWidget(kvs);
    layout->addWidget(kvssize);
    layout->addWidget(size);
    layout->addWidget(size_locked);
    layout->addWidget(size_on_disk);
    layout->addWidget(size_on_memory);
    layout->addWidget(kvs_rate);
    layout->addWidget(kvs_rate_size);
    layout->addWidget(rate);
    layout->addWidget(rate_size);

    groupBox->setLayout(layout);
    
    //resize(minimumSizeHint()); 
}

void QueueViewer::setData(QueueData newData)
{
        data = newData;
        name->setText(QString("Name: ") + QString(data.name.c_str()));
        kvs->setText(QString("KVs: ") + QString(data.kvs.c_str()));
        kvssize->setText(QString("KVs Size: ") + QString(data.kvsize.c_str()));
        size->setText(QString("Size: ") + QString(data.size.c_str()));
        size_locked->setText(QString("Size Locked: ") + QString(data.size_locked.c_str()));
        size_on_disk->setText(QString("Size (on disk): ") + QString(data.size_on_disk.c_str()));
        size_on_memory->setText(QString("Size (on memory): ") + QString(data.size_on_memory.c_str()));
        kvs_rate->setText(QString("KVs Rate: ") + QString(data.kvs_rate.c_str()));
        kvs_rate_size->setText(QString("KVs Rate Size: ") + QString(data.kvs_rate_size.c_str()));
        rate->setText(QString("Rate: ") + QString(data.rate.c_str()));
        rate_size->setText(QString("Rate Size: ") + QString(data.rate_size.c_str()));
        
        setTitle(data.name);
        //groupBox->resize(minimumSizeHint());        
        groupBox->show();
}

void QueueViewer::setTitle(std::string title)
{
    if (groupBox) groupBox->setTitle(QString(title.c_str()));
}

} //namespace
