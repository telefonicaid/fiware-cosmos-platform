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
#include <QtGui>
#include <sstream>


#include "au/Table.h"

#include "ui_SamsonQueryWidget.h"


class SamsonQueryWidget : public QWidget , public Ui_SamsonQueryWidget
{
    
    QStandardItemModel *myModel;
    
    Q_OBJECT
    
public:
    
    SamsonQueryWidget(QWidget *parent = 0);
    
    private slots:
    
    void update();
    
private:
    
    void setModel( QStandardItemModel*model );
    
    static QStandardItemModel* getExampleModel();
    static QStandardItemModel* createModel( au::DataSet* dataSet );
    
};

