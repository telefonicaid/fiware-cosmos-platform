/* ****************************************************************************
*
* FILE                     QueueViewer.cpp
*
* DESCRIPTION			   Widget to visualize samson queues' data
*
*/

#include "QueueViewer.h"

#include "au/string.h"

#include <iostream>
#include <stdlib.h>

namespace samson{

QueueViewer::QueueViewer(std::string _title, QWidget* parent): QWidget(parent)
{
    //QVBoxLayout* layout;
    
    title = _title;
    
    //groupBox = new QGroupBox(this);
    //QSizePolicy expandingPolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //groupBox->setSizePolicy(expandingPolicy);
   /* scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(groupBox);   
    */
    QFont fontTmp;
    name = new QLabel("Empty Queue: ", this);
    kvs = new QLabel("KVs: ", this);
    kvsDigits = new QLabel(this);
    fontTmp = kvsDigits->font();
    fontTmp.setFamily("Helvetica"); 
    fontTmp.setPointSize(fontTmp.pointSize()*1.5f);
    kvsDigits->setFont(fontTmp);
    kvsDigits->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    kvsDigits->setLineWidth(2);
    //kvssize = new QLabel("KVs Size: ", groupBox);
    size = new QLabel("Size: ", this);
    sizeDigits = new QLabel(this);
    sizeDigits->setFont(fontTmp);
    sizeDigits->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    sizeDigits->setLineWidth(2);

    rate = new QLabel("Rate: ", this);
    rateDigits = new QLabel(this);
    rateDigits->setFont(fontTmp);
    rateDigits->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    rateDigits->setLineWidth(2);
    
    detailsButton = new QPushButton("details >>",this);
    //rate_size = new QLabel("Rate Size: ", groupBox);

    if (data.name.empty())
    {
        setTitle("Empty Queue");
    }
    else
    {
        setTitle(data.name);
    }
    
    //layout = new QHBoxLayout(this);
    /*layout->addWidget(name);
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
*/
    //groupBox->setLayout(layout);
    

    //resize(minimumSizeHint()); 
}

void QueueViewer::setData(QueueData newData)
{
        data = newData;
        name->setText(QString(data.name.c_str() + QString(": ")));
        kvsDigits->setText(QString(data.kvs.c_str()));
        //kvssize->setText(QString("KVs Size: ") + QString(data.kvsize.c_str()));
        sizeDigits->setText(QString(au::str(atol(data.size.c_str())).c_str()));
        //size_locked->setText(QString("Size Locked: ") + QString(data.size_locked.c_str()));
        //size_on_disk->setText(QString("Size (on disk): ") + QString(data.size_on_disk.c_str()));
        //size_on_memory->setText(QString("Size (on memory): ") + QString(data.size_on_memory.c_str()));
        //kvs_rate->setText(QString("KVs Rate: ") + QString(data.kvs_rate.c_str()));
        //kvs_rate_size->setText(QString("KVs Rate Size: ") + QString(data.kvs_rate_size.c_str()));
        rateDigits->setText(QString((au::str(atol(data.rate.c_str()))/* + std::string("b/s")*/).c_str()));
        //rate_size->setText(QString("Rate Size: ") + QString(data.rate_size.c_str()));
}

void QueueViewer::setTitle(std::string title)
{
    //if (groupBox) groupBox->setTitle(QString(title.c_str()));
}



void QueueViewer::setHiddenButton(bool hidden)
{
    if (hidden)
    {
        detailsButton->hide();
    }
    else
    {
        detailsButton->show();
    }
}

void QueueViewer::setLayout(QGridLayout* layout, int row)
{
    layout->addWidget(name, row, 0);
    layout->setColumnMinimumWidth(1, 15);
    layout->addWidget(kvs, row, 2, Qt::AlignRight);
    layout->addWidget(kvsDigits, row, 3);
    layout->setColumnMinimumWidth(4,15);
    layout->addWidget(size, row, 5, Qt::AlignRight);
    layout->addWidget(sizeDigits, row, 6);
    layout->setColumnMinimumWidth(7,15);
    layout->addWidget(rate, row, 8, Qt::AlignRight);
    layout->addWidget(rateDigits, row, 9);
    layout->setColumnMinimumWidth(10,15);
    layout->addWidget(detailsButton, row, 11);
}


} //namespace
