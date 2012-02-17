/* ****************************************************************************
*
* FILE                     QueueViewer.cpp
*
* DESCRIPTION			   Widget to visualize samson queues' data
*
*/

#include "QueueViewer.h"
#include <iostream>
#include <stdlib.h>

namespace samson{

QueueViewer::QueueViewer(QWidget* parent, std::string title): QWidget(parent)
{
    //QVBoxLayout* layout;
    
    groupBox = new QGroupBox(this);
    QSizePolicy expandingPolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    groupBox->setSizePolicy(expandingPolicy);
   /* scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(groupBox);   
    */
    name = new QLabel("Empty Queue: ", groupBox);
    kvs = new QLabel("KVs: ", groupBox);
    kvsDigits = new QLCDNumber(groupBox);
    //kvssize = new QLabel("KVs Size: ", groupBox);
    size = new QLabel("Size: ", groupBox);
    sizeDigits = new QLCDNumber(groupBox);
    //size_locked = new QLabel("Size Locked: ", groupBox);
    //size_on_disk = new QLabel("Size (on disk): ", groupBox);
    //size_on_memory = new QLabel("Size (on memory): ", groupBox);
    //kvs_rate = new QLabel("KVs Rate: ", groupBox);
    //kvs_rate_size = new QLabel("KVs Rate Size: ", groupBox);
    rate = new QLabel("Mb/s: ", groupBox);
    rateDigits = new QLCDNumber(groupBox);
    detailsButton = new QPushButton("details >>",groupBox);
    //rate_size = new QLabel("Rate Size: ", groupBox);

    if (data.name.empty())
    {
        setTitle("Empty Queue");
    }
    else
    {
        setTitle(data.name);
    }
    
    layout = new QHBoxLayout(this);
    layout->addWidget(name);
    layout->addStretch();
    layout->addWidget(kvs);
    layout->addWidget(kvsDigits);
    layout->addStretch();
    //layout->addWidget(kvssize);
    layout->addWidget(size);
    layout->addWidget(sizeDigits);
    layout->addStretch();
    //layout->addWidget(size_locked);
    //layout->addWidget(size_on_disk);
    //layout->addWidget(size_on_memory);
    //layout->addWidget(kvs_rate);
    //layout->addWidget(kvs_rate_size);
    layout->addWidget(rate);
    layout->addWidget(rateDigits);
    layout->addStretch();
    layout->addWidget(detailsButton);    //layout->addWidget(rate_size);

    groupBox->setLayout(layout);
    

    //resize(minimumSizeHint()); 
}

void QueueViewer::setData(QueueData newData)
{
        data = newData;
        name->setText(QString(data.name.c_str() + QString(": ")));
        kvsDigits->display(atoi(data.kvs.c_str()));
        //kvssize->setText(QString("KVs Size: ") + QString(data.kvsize.c_str()));
        sizeDigits->display(atoi(data.size.c_str()));
        //size_locked->setText(QString("Size Locked: ") + QString(data.size_locked.c_str()));
        //size_on_disk->setText(QString("Size (on disk): ") + QString(data.size_on_disk.c_str()));
        //size_on_memory->setText(QString("Size (on memory): ") + QString(data.size_on_memory.c_str()));
        //kvs_rate->setText(QString("KVs Rate: ") + QString(data.kvs_rate.c_str()));
        //kvs_rate_size->setText(QString("KVs Rate Size: ") + QString(data.kvs_rate_size.c_str()));
        rateDigits->display(atof(data.rate.c_str()));
        //rate_size->setText(QString("Rate Size: ") + QString(data.rate_size.c_str()));
        
        setTitle(data.name);
        groupBox->adjustSize(); 
        adjustSize();       
        groupBox->show();
        //groupBox->resize(minimumSizeHint());        
}

void QueueViewer::setTitle(std::string title)
{
    if (groupBox) groupBox->setTitle(QString(title.c_str()));
}

} //namespace
