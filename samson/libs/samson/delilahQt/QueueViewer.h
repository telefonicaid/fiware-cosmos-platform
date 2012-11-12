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
#ifndef _H_QueueViewer
#define _H_QueueViewer

/* ****************************************************************************
*
* FILE                     QueueViewer.h
*
* DESCRIPTION			   Widget to visualize samson queues' data
*
*/
#include <QtGui>
#include <vector>
#include <string>

#include "ExtQueueViewer.h"

namespace samson {
        
class QueueViewer: public QWidget
{
        Q_OBJECT
    public:
        
        QueueViewer(std::string _title, QWidget* parent = 0);
        ~QueueViewer();
        std::string title;

        QueueData data;
        
        void setData(QueueData* newData);
        void setData(QueueData newData);
        void setHiddenButton(bool hidden);
        void setLayout(QGridLayout* layout, int row);
        
        QPushButton* detailsButton;

    signals:
        void detailsClicked();

    public slots:
        void onDetailsClicked();

    private:
        //QScrollArea* scrollArea;
        //QGroupBox* groupBox;
        //QHBoxLayout* layout;

        QLabel* name;
        QLabel* kvs;
        QLabel* kvsDigits; 
        QLabel* kvs_s;
        QLabel* kvs_sDigits; 
        QLabel* size;
        QLabel* sizeDigits; 
        QLabel* rate;
        QLabel* rateDigits;
        
};

} //namespace

#endif //_H_Delilah_QT
