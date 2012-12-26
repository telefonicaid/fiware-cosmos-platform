/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
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
    title = _title;
    data.name = _title;
    
   /* scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(groupBox);   
    */
    QFont bigFont;
    QFont boldFont;
    name = new QLabel(QString(title.c_str()), this);
    boldFont = name->font();
    boldFont.setBold(true);
    name->setFont(boldFont);
    kvs = new QLabel("#Kvs: ", this);
    kvsDigits = new QLabel(this);
    bigFont = kvsDigits->font();
    bigFont.setFamily("Helvetica"); 
    bigFont.setPointSize(bigFont.pointSize()*1.2f);
    kvsDigits->setFont(bigFont);
    kvsDigits->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    kvsDigits->setLineWidth(2);
    
    size = new QLabel("Size: ", this);
    sizeDigits = new QLabel(this);
    sizeDigits->setFont(bigFont);
    sizeDigits->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    sizeDigits->setLineWidth(2);

    rate = new QLabel("Rate: ", this);
    rateDigits = new QLabel(this);
    rateDigits->setFont(bigFont);
    rateDigits->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    rateDigits->setLineWidth(2);
    
    kvs_s = new QLabel("Kvs/s", this);
    kvs_sDigits = new QLabel(this);
    kvs_sDigits->setFont(bigFont);
    kvs_sDigits->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    kvs_sDigits->setLineWidth(2);

    detailsButton = new QPushButton("details >>",this);
    connect(detailsButton, SIGNAL(clicked()), this, SLOT(onDetailsClicked()));
}

QueueViewer::~QueueViewer()
{   
    //Javi: for some reason we need to explicitly delete all the widgets 
    // or otherwise the widget does not dissapear even if we call "delete myQueueViewer;". 
    // This doesn't seem right, so I'll keep an eye on it.
    delete name;
    delete kvs;
    delete kvsDigits;
    delete kvs_s;
    delete kvs_sDigits;
    delete size;
    delete sizeDigits;
    delete rate;
    delete rateDigits;
    delete detailsButton;
}

void QueueViewer::setData(QueueData* newData)
{
    setData(*newData);
}

void QueueViewer::setData(QueueData newData)
{
        data = newData;
        name->setText(QString(data.name.c_str() + QString(": ")));
        kvsDigits->setText(QString(au::str(strtoul(data.kvs.c_str(), NULL, 0)).c_str()));
        kvs_sDigits->setText(QString(au::str(strtoul(data.kvs_s.c_str(), NULL, 0)).c_str()));
        sizeDigits->setText(QString(au::str(strtoul(data.size.c_str(), NULL, 0)).c_str()));
        rateDigits->setText(QString((au::str(strtoul(data.bytes_s.c_str(), NULL, 0))).c_str()));
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
    layout->addWidget(kvs_s, row, 11, Qt::AlignRight);
    layout->addWidget(kvs_sDigits, row, 12);
    layout->setColumnMinimumWidth(13,15);
    layout->addWidget(detailsButton, row, 14);
}

void QueueViewer::onDetailsClicked()
{
    emit detailsClicked();
}

} //namespace
