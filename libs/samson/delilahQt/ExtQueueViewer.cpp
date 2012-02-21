/* ****************************************************************************
*
* FILE                     ExtQueueViewer.cpp
*
* DESCRIPTION			   Widget to visualize samson queues' data
*
*/

#include "ExtQueueViewer.h"

#include "au/string.h"

#include <iostream>
#include <stdlib.h>

namespace samson{

bool ExtQueueData::operator==(const ExtQueueData &other) const 
{
    if(name != other.name) return false;
    if(kvs != other.kvs) return false;
    if(size != other.size) return false;
    if(key != other.key) return false;
    if(value != other.value) return false;
    if(total_kvs != other.total_kvs) return false;
    if(total_size != other.total_size) return false;
    if(kvs_s != other.kvs_s) return false;
    if(bytes_s != other.bytes_s) return false;
    if(blocks != other.blocks) return false;
    if(rate_size != other.rate_size) return false;
    if(on_memory != other.on_memory) return false;
    if(on_disk != other.on_disk) return false;
    if(locked != other.locked) return false;
    if(time_from != other.time_from) return false;
    if(time_to != other.time_to) return false;
    return true;
}

ExtQueueViewer::ExtQueueViewer(std::string _title, QWidget* parent): QWidget(parent)
{
    //QVBoxLayout* layout;
    
    title = _title;
    data.name = _title;
    
    //groupBox = new QGroupBox(this);
    //QSizePolicy expandingPolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //groupBox->setSizePolicy(expandingPolicy);
   /* scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(groupBox);   
    */
    mainLayout = new QVBoxLayout(this);
    
    generalLayout = new QHBoxLayout(this);
    sizeLayout = new QHBoxLayout(this);
    formatLayout = new QHBoxLayout(this);
    rateLayout = new QHBoxLayout(this);
    blocksLayout = new QHBoxLayout(this);

    sizeBox = new QGroupBox("Size", this);
    formatBox = new QGroupBox("Format", this);
    rateBox = new QGroupBox("Rate", this);
    blocksBox = new QGroupBox("Blocks", this);

    QFont bigFont;
    QFont boldFont;
    
    name = new QLabel("Empty", sizeBox);
    boldFont = name->font();
    boldFont.setBold(true);
    name->setFont(boldFont);
    kvsLabel = new QLabel("Kvs: ", sizeBox);
    kvs = new QLabel("0", sizeBox);
    kvs->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    sizeLabel = new QLabel("Size: ", sizeBox);
    size = new QLabel("0", sizeBox);
    size->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    keyLabel = new QLabel("key: ", formatBox);
    key = new QLabel("", formatBox);
    key->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    valueLabel = new QLabel("Value: ", formatBox);
    value = new QLabel("", formatBox);
    value->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    total_kvsLabel = new QLabel("Total Kvs: ", rateBox);
    total_kvs = new QLabel("0", rateBox);
    total_kvs->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    total_sizeLabel = new QLabel("Total Size: ", rateBox);
    total_size = new QLabel("0", rateBox);
    total_size->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    kvs_sLabel = new QLabel("Kvs/s: ", rateBox);
    kvs_s = new QLabel("0", rateBox);
    kvs_s->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    bytes_sLabel = new QLabel("Bytes/s: ", rateBox);
    bytes_s = new QLabel("0", rateBox);
    bytes_s->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    blocksLabel = new QLabel("Blocks: ", blocksBox);
    blocks = new QLabel("0", blocksBox);
    blocks->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    rate_sizeLabel = new QLabel("Size: ", blocksBox);
    rate_size = new QLabel("0", blocksBox);
    rate_size->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    on_memoryLabel = new QLabel("On Memory: ", blocksBox);
    on_memory = new QLabel("0", blocksBox);
    on_memory->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    on_diskLabel = new QLabel("On Disk: ", blocksBox);
    on_disk = new QLabel("0", blocksBox);
    on_disk->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    lockedLabel = new QLabel("Locked: ", blocksBox);
    locked = new QLabel("0", blocksBox);
    locked->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    time_fromLabel = new QLabel("Time from ", blocksBox);
    time_from = new QLabel("0", blocksBox);
    time_from->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    time_toLabel = new QLabel(" to ", blocksBox);
    time_to = new QLabel("0", blocksBox);
    time_to->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    mainLayout->addWidget(name);
    mainLayout->addLayout(generalLayout);
    mainLayout->addWidget(rateBox);
    mainLayout->addWidget(sizeBox);
    mainLayout->addWidget(blocksBox);
    mainLayout->addStretch();
    
    generalLayout->addWidget(sizeBox);
    generalLayout->addWidget(formatBox);
    generalLayout->addStretch();

    sizeLayout->addWidget(kvsLabel, Qt::AlignRight);
    sizeLayout->addWidget(kvs);
    sizeLayout->addWidget(sizeLabel, Qt::AlignRight);
    sizeLayout->addWidget(size);
    sizeLayout->addStretch();
    sizeBox->setLayout(sizeLayout);
    
    formatLayout->addWidget(keyLabel, Qt::AlignRight);
    formatLayout->addWidget(key);
    formatLayout->addWidget(valueLabel, Qt::AlignRight);
    formatLayout->addWidget(value);
    formatLayout->addStretch();
    formatBox->setLayout(formatLayout);
    
    rateLayout->addWidget(total_kvsLabel, Qt::AlignRight);
    rateLayout->addWidget(total_kvs);
    rateLayout->addWidget(total_sizeLabel, Qt::AlignRight);
    rateLayout->addWidget(total_size);
    rateLayout->addWidget(kvs_sLabel, Qt::AlignRight);
    rateLayout->addWidget(kvs_s);
    rateLayout->addWidget(bytes_sLabel, Qt::AlignRight);
    rateLayout->addWidget(bytes_s);
    rateLayout->addStretch();
    rateBox->setLayout(rateLayout);

    blocksLayout->addWidget(blocksLabel, Qt::AlignRight);
    blocksLayout->addWidget(blocks);
    blocksLayout->addWidget(rate_sizeLabel, Qt::AlignRight);
    blocksLayout->addWidget(rate_size);
    blocksLayout->addWidget(on_memoryLabel, Qt::AlignRight);
    blocksLayout->addWidget(on_memory);
    blocksLayout->addWidget(on_diskLabel, Qt::AlignRight);
    blocksLayout->addWidget(on_disk);
    blocksLayout->addWidget(lockedLabel, Qt::AlignRight);
    blocksLayout->addWidget(locked);
    blocksLayout->addWidget(time_fromLabel, Qt::AlignRight);
    blocksLayout->addWidget(time_from);
    blocksLayout->addWidget(time_toLabel, Qt::AlignRight);
    blocksLayout->addWidget(time_to);
    blocksLayout->addStretch();
    blocksBox->setLayout(blocksLayout);
    
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

void ExtQueueViewer::setData(ExtQueueData newData)
{
/*        data = newData;
        name->setText(QString(data.name.c_str() + QString(": ")));
        kvsDigits->setText(QString(data.kvs.c_str()));
        //kvssize->setText(QString("KVs Size: ") + QString(data.kvsize.c_str()));
        sizeDigits->setText(QString(au::str(strtoul(data.size.c_str(), NULL, 0)).c_str()));
        //size_locked->setText(QString("Size Locked: ") + QString(data.size_locked.c_str()));
        //size_on_disk->setText(QString("Size (on disk): ") + QString(data.size_on_disk.c_str()));
        //size_on_memory->setText(QString("Size (on memory): ") + QString(data.size_on_memory.c_str()));
        //kvs_rate->setText(QString("KVs Rate: ") + QString(data.kvs_rate.c_str()));
        //kvs_rate_size->setText(QString("KVs Rate Size: ") + QString(data.kvs_rate_size.c_str()));
        rateDigits->setText(QString((au::str(strtoul(data.rate.c_str(), NULL, 0))).c_str()));
        //rate_size->setText(QString("Rate Size: ") + QString(data.rate_size.c_str()));
        */
}

/*void ExtQueueViewer::setLayout(QGridLayout* layout, int row)
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
}*/


} //namespace
